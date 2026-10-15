// oled_demo.ino: bench.ino plus a 0.96in OLED readout, for the demo.
//
// Same serial output as bench.ino ("cm 42.5" lines at 115200), so
// sense-serial.js works unchanged. The screen is extra: a big distance, a state
// badge, a sonar ping whose blip sits at the measured range, and a scrolling
// history along the bottom.
//
// Wiring, Arduino Nano:
//   HC-SR04  VCC -> 5V    GND -> GND    TRIG -> D9    ECHO -> D10
//   OLED     VCC -> 5V    GND -> GND    SDA  -> A4    SCL  -> A5
// The OLED is an SSD1306, 128 x 64, I2C at 0x3C. Needs the U8g2 library.

#include <U8g2lib.h>
#include <Wire.h>

// Page mode: draws the screen in eight strips with a 128-byte buffer, because
// the Nano has 2 KB of RAM and a full frame would take half of it.
U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

const int TRIG = 9;
const int ECHO = 10;

const float NEAR_CM = 30.0;      // badge thresholds for the screen only;
const float HERE_CM = 120.0;     // the real State Table lives in the browser
const float MAX_CM = 200.0;      // top of the graph and the sonar range

const int GRAPH_H = 14;
uint8_t hist[128];               // one column per reading, 0..GRAPH_H
uint8_t head = 0;

float smooth = MAX_CM;
float sweep = 0;                 // sonar sweep angle, degrees
int sweepDir = 1;
uint8_t ping = 0;                // expanding ring radius

float readCm() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long us = pulseIn(ECHO, HIGH, 30000);
  return us ? us / 58.0 : 400.0;
}

void splash() {
  for (uint8_t r = 2; r < 70; r += 6) {
    oled.firstPage();
    do {
      oled.drawCircle(64, 32, r);
      if (r > 8) oled.drawCircle(64, 32, r - 8);
      oled.setFont(u8g2_font_6x12_tr);
      oled.drawStr(28, 36, "SENSOR LAYER");
    } while (oled.nextPage());
  }
}

void drawBadge(float cm) {
  const char *label = cm < NEAR_CM ? "NEAR" : cm < HERE_CM ? "HERE" : "EMPTY";
  oled.setFont(u8g2_font_6x12_tr);
  int w = oled.getStrWidth(label) + 6;
  if (cm < NEAR_CM) {              // near: solid badge
    oled.drawRBox(0, 0, w, 12, 2);
    oled.setDrawColor(0);
    oled.drawStr(3, 10, label);
    oled.setDrawColor(1);
  } else {
    oled.drawRFrame(0, 0, w, 12, 2);
    oled.drawStr(3, 10, label);
  }
}

void drawNumber(float cm) {
  char buf[8];
  oled.setFont(u8g2_font_logisoso22_tn);
  if (cm >= 400.0) {
    oled.drawStr(0, 40, "---");
  } else {
    dtostrf(cm, 0, cm < 100 ? 1 : 0, buf);
    oled.drawStr(0, 40, buf);
  }
  oled.setFont(u8g2_font_5x8_tr);
  oled.drawStr(54, 40, "cm");
}

void drawSonar(float cm) {
  const int ox = 100, oy = 46, R = 30;
  // range rings and the frame of the fan
  oled.drawCircle(ox, oy, R / 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  oled.drawCircle(ox, oy, 2 * R / 3, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  oled.drawCircle(ox, oy, R, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  // the outgoing ping
  if (ping < R) oled.drawCircle(ox, oy, ping, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
  // the sweep line
  float a = (sweep - 90) * DEG_TO_RAD;
  oled.drawLine(ox, oy, ox + R * cos(a), oy + R * sin(a));
  // the blip, at the measured range along the sweep
  if (cm < MAX_CM) {
    float r = cm / MAX_CM * R;
    int bx = ox + r * cos(a), by = oy + r * sin(a);
    oled.drawDisc(bx, by, cm < NEAR_CM ? 3 : 2);
  }
}

void drawGraph() {
  const int y0 = 63;
  for (uint8_t x = 0; x < 128; x++) {
    uint8_t h = hist[(head + x) % 128];
    if (h) oled.drawVLine(x, y0 - h + 1, h);
  }
  oled.drawHLine(0, y0 - GRAPH_H, 128);  // ceiling line
}

void setup() {
  Serial.begin(115200);          // sense-serial.js opens the port at this speed
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  oled.begin();
  oled.setBusClock(400000);
  splash();
}

void loop() {
  float cm = readCm();

  Serial.print("cm ");
  Serial.print(cm, 1);
  Serial.println();

  smooth += (min(cm, 400.0f) - smooth) * 0.35;

  // closer reads taller in the graph
  float c = constrain(smooth, 0, MAX_CM);
  hist[head] = (uint8_t)((1.0 - c / MAX_CM) * GRAPH_H);
  head = (head + 1) % 128;

  sweep += 6 * sweepDir;
  if (sweep > 60 || sweep < -60) sweepDir = -sweepDir;
  ping = (ping + 3) % 36;

  oled.firstPage();
  do {
    drawBadge(smooth);
    drawNumber(smooth);
    drawSonar(smooth);
    drawGraph();
  } while (oled.nextPage());
}
