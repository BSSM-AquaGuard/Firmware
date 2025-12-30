#include "esp_camera.h"
#include <WiFi.h>
#include <HardwareSerial.h>

// 보드 선택(핀맵) - 본인 프로젝트의 board_config.h 사용
#include "board_config.h"

// ===========================
// (선택) AP 설정
// ===========================
#define USE_AP 1
const char *ssid = "ESP32CAM_AP";
const char *password = "12345678";

// ===========================
// UART 설정
// ===========================
static const int UART_BAUD = 115200;
static const int UART_TX_PIN = 14; // 연결 핀은 보드/배선에 맞게 조정
static const int UART_RX_PIN = 15;

HardwareSerial UartToEsp(1);  // UART1

// ===========================
// 튜닝 파라미터
// ===========================

// 1) RGB565 2바이트 순서(가장 흔한 오판 원인)
//   - 빨강을 보여줬는데 B가 뜨면 값을 뒤집어보세요.
//   - 0: px = (buf[idx+1]<<8) | buf[idx]  (기존 코드 방식)
//   - 1: px = (buf[idx]<<8)   | buf[idx+1]
#define RGB565_SWAP_BYTES 1

// 2) 중앙 샘플 영역 반지름(7이면 15x15 평균)
// 배경이 섞여 오판하면 7 -> 2~3으로 줄이는 게 유리합니다.
static const int SAMPLE_RADIUS = 3;

// 3) 안정화: 최근 N프레임 다수결
static const int VOTE_N = 7;

// 4) 색 판정 파라미터(환경에 따라 조정)
static const int MIN_BRIGHTNESS = 40; // 너무 어두우면 N
static const int DOMINANCE      = 25; // 얼마나 우세해야 R/G/B로 인정할지
// 채널 보정: G를 낮추고 B를 살림(색상 오판 줄이기 용도)
static const float G_GAIN = 0.85f;
static const float B_GAIN = 1.10f;

// 5) AWB(자동 화밸) 워밍업 시간(ms) 후 고정
static const uint32_t AWB_WARMUP_MS = 1200;

// ===========================
// RGB565 -> RGB888 변환
// ===========================
static inline void rgb565_to_rgb888(uint16_t p, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = ((p >> 11) & 0x1F) * 255 / 31;
  g = ((p >> 5)  & 0x3F) * 255 / 63;
  b = ( p        & 0x1F) * 255 / 31;
}

// ===========================
// (핵심) RGB565 픽셀 읽기: 바이트 순서 옵션
// ===========================
static inline uint16_t read_rgb565(const uint8_t *buf, int idx) {
#if RGB565_SWAP_BYTES
  return ((uint16_t)buf[idx] << 8) | buf[idx + 1];
#else
  return ((uint16_t)buf[idx + 1] << 8) | buf[idx];
#endif
}

// ===========================
// 평균 RGB 구하기
// ===========================
static bool get_center_avg_rgb(camera_fb_t *fb, int sample_radius, uint8_t &rAvg, uint8_t &gAvg, uint8_t &bAvg) {
  if (!fb || !fb->buf) return false;
  if (fb->format != PIXFORMAT_RGB565) return false;

  int w = fb->width;
  int h = fb->height;
  int cx = w / 2;
  int cy = h / 2;

  int x0 = max(0, cx - sample_radius);
  int x1 = min(w - 1, cx + sample_radius);
  int y0 = max(0, cy - sample_radius);
  int y1 = min(h - 1, cy + sample_radius);

  uint32_t rs = 0, gs = 0, bs = 0;
  uint32_t cnt = 0;

  for (int y = y0; y <= y1; y++) {
    for (int x = x0; x <= x1; x++) {
      int idx = (y * w + x) * 2;
      if (idx + 1 >= (int)fb->len) continue;

      uint16_t px = read_rgb565(fb->buf, idx);
      uint8_t r, g, b;
      rgb565_to_rgb888(px, r, g, b);

      rs += r; gs += g; bs += b;
      cnt++;
    }
  }

  if (cnt == 0) return false;

  rAvg = (uint8_t)(rs / cnt);
  gAvg = (uint8_t)(gs / cnt);
  bAvg = (uint8_t)(bs / cnt);
  return true;
}

// ===========================
// 색상 판별
// ===========================
static char classify_color(uint8_t r, uint8_t g, uint8_t b) {
  int rAdj = r;
  int gAdj = (int)(g * G_GAIN);
  int bAdj = (int)(b * B_GAIN);

  if (gAdj < 0) gAdj = 0;
  if (gAdj > 255) gAdj = 255;
  if (bAdj < 0) bAdj = 0;
  if (bAdj > 255) bAdj = 255;

  // 너무 어두우면 의미 없는 값이 튀는 경우가 많아서 컷
  if (rAdj < MIN_BRIGHTNESS && gAdj < MIN_BRIGHTNESS && bAdj < MIN_BRIGHTNESS) {
    return 'N';
  }

  // 단순 우세 + 여유(dominance)
  if (rAdj > gAdj + DOMINANCE && rAdj > bAdj + DOMINANCE) return 'R';
  if (gAdj > rAdj + DOMINANCE && gAdj > bAdj + DOMINANCE) return 'G';
  if (bAdj > rAdj + DOMINANCE && bAdj > gAdj + DOMINANCE) return 'B';

  return 'N';
}

// ===========================
// 다수결 안정화
// ===========================
static char vote_buf[VOTE_N];
static int  vote_idx = 0;
static bool vote_inited = false;

static char stable_vote(char c) {
  if (!vote_inited) {
    for (int i = 0; i < VOTE_N; i++) vote_buf[i] = 'N';
    vote_inited = true;
  }

  vote_buf[vote_idx] = c;
  vote_idx = (vote_idx + 1) % VOTE_N;

  int r = 0, g = 0, b = 0, n = 0;
  for (int i = 0; i < VOTE_N; i++) {
    if (vote_buf[i] == 'R') r++;
    else if (vote_buf[i] == 'G') g++;
    else if (vote_buf[i] == 'B') b++;
    else n++;
  }

  // 최다 득표 색 선택(동점이면 N)
  int maxv = max(max(r, g), max(b, n));
  int ties = 0;
  if (r == maxv) ties++;
  if (g == maxv) ties++;
  if (b == maxv) ties++;
  if (n == maxv) ties++;

  if (ties >= 2) return 'N';
  if (r == maxv) return 'R';
  if (g == maxv) return 'G';
  if (b == maxv) return 'B';
  return 'N';
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // UART1 시작
  UartToEsp.begin(UART_BAUD, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  config.frame_size   = FRAMESIZE_QQVGA;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.fb_count     = 1;
  config.jpeg_quality = 12; // RGB 모드에선 큰 의미 없음

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // 카메라 센서 설정: 먼저 자동으로 잠깐 적응시키고, 이후 고정(색 인식 안정화)
  sensor_t *s = esp_camera_sensor_get();
  if (s != NULL) {
    // 워밍업 동안은 자동
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_wb_mode(s, 0);      // Auto
    s->set_gain_ctrl(s, 1);    // AGC on
    s->set_exposure_ctrl(s, 1);// AEC on
  }

#if USE_AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP: ");
  Serial.println(ip);
#endif

  // 워밍업: 센서가 초반에 튀는 걸 줄이기 위해 몇 프레임 버리고, 그 뒤 AWB를 고정
  uint32_t t0 = millis();
  while (millis() - t0 < AWB_WARMUP_MS) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) esp_camera_fb_return(fb);
    delay(30);
  }

  // 워밍업 후 화이트밸런스 고정(핵심 안정화 포인트)
  if (s != NULL) {
    s->set_whitebal(s, 0);
    s->set_awb_gain(s, 0);
  }

  Serial.println("Setup done.");
  Serial.printf("RGB565_SWAP_BYTES=%d, SAMPLE_RADIUS=%d, VOTE_N=%d\n", RGB565_SWAP_BYTES, SAMPLE_RADIUS, VOTE_N);
}

void loop() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(200);
    return;
  }

  if (fb->format != PIXFORMAT_RGB565) {
    Serial.printf("Unexpected format: %d\n", fb->format);
    esp_camera_fb_return(fb);
    delay(200);
    return;
  }

  uint8_t r, g, b;
  bool ok = get_center_avg_rgb(fb, SAMPLE_RADIUS, r, g, b);

  esp_camera_fb_return(fb);

  if (!ok) {
    Serial.println("RGB read failed");
    delay(200);
    return;
  }

  char raw = classify_color(r, g, b);
  char stable = stable_vote(raw);

  // USB 시리얼 출력
  Serial.printf("RGB(%u,%u,%u) raw=%c stable=%c\n", r, g, b, raw, stable);

  // UART로 전송(원하면 stable만 보내는 게 일반적으로 더 좋습니다)
  // UartToEsp.write(stable);
  // UartToEsp.write('\n');

  delay(200);
}
