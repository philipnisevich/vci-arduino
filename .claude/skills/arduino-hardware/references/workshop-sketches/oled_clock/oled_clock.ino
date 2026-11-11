// WiFi clock: fetches time from the internet (NTP) and shows the date and
// time on a 0.96" 128x64 SSD1306 I2C OLED (SDA -> A4, SCL -> A5).
// Times are US Pacific, with automatic daylight-saving switching.
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include "arduino_secrets.h"

const long STANDARD_OFFSET = -8L * 3600;  // PST (UTC-8)
const long DST_OFFSET      = -7L * 3600;  // PDT (UTC-7)

Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiUDP udp;
NTPClient ntp(udp, "pool.ntp.org", 0, 60UL * 60 * 1000);  // UTC, resync hourly

const char *DAYS[]   = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
const char *MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void showMessage(const char *line1, const char *line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(line1);
  display.setCursor(0, 34);
  display.println(line2);
  display.display();
}

// US rule: DST runs from 2:00 local on the 2nd Sunday of March
// to 2:00 local on the 1st Sunday of November.
bool isDst(time_t utc) {
  struct tm t;
  time_t std = utc + STANDARD_OFFSET;
  gmtime_r(&std, &t);
  int month = t.tm_mon + 1, day = t.tm_mday, wday = t.tm_wday;
  if (month < 3 || month > 11) return false;
  if (month > 3 && month < 11) return true;
  int prevSunday = day - wday;
  if (month == 3) {
    if (prevSunday < 8) return false;       // before the 2nd Sunday
    if (prevSunday > 14 || wday != 0) return true;
    return t.tm_hour >= 2;                  // switch day itself
  }
  // November
  if (prevSunday < 1) return true;          // before the 1st Sunday
  if (wday != 0 || prevSunday > 7) return false;
  return t.tm_hour < 1;                     // 2:00 PDT == 1:00 PST
}

void connectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    showMessage("Connecting to WiFi:", SECRET_SSID);
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) delay(500);
    if (WiFi.status() != WL_CONNECTED) {
      showMessage("WiFi failed.", "Check arduino_secrets.h");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Most modules are at 0x3C; some are jumpered to 0x3D.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C) &&
      !display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println("OLED not found - check wiring");
    while (true) delay(1000);
  }
  display.setTextColor(SSD1306_WHITE);

  connectWiFi();
  showMessage("Getting time...");
  ntp.begin();
  while (!ntp.forceUpdate()) delay(1000);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  ntp.update();

  time_t utc = ntp.getEpochTime();
  bool dst = isDst(utc);
  time_t local = utc + (dst ? DST_OFFSET : STANDARD_OFFSET);
  struct tm t;
  gmtime_r(&local, &t);

  int hour12 = t.tm_hour % 12;
  if (hour12 == 0) hour12 = 12;
  char hhmm[6], secs[3], date[20];
  snprintf(hhmm, sizeof(hhmm), "%d:%02d", hour12, t.tm_min);
  snprintf(secs, sizeof(secs), "%02d", t.tm_sec);
  snprintf(date, sizeof(date), "%s, %s %d %d",
           DAYS[t.tm_wday], MONTHS[t.tm_mon], t.tm_mday, t.tm_year + 1900);

  display.clearDisplay();

  // Date across the top
  display.setTextSize(1);
  display.setCursor((128 - strlen(date) * 6) / 2, 2);
  display.print(date);
  display.drawFastHLine(0, 13, 128, SSD1306_WHITE);

  // Big HH:MM, with seconds and AM/PM to the right
  int bigWidth = strlen(hhmm) * 18;
  int x = (128 - bigWidth - 16) / 2;
  display.setTextSize(3);
  display.setCursor(x, 26);
  display.print(hhmm);
  display.setTextSize(1);
  display.setCursor(x + bigWidth + 4, 26);
  display.print(t.tm_hour < 12 ? "AM" : "PM");
  display.setCursor(x + bigWidth + 4, 40);
  display.print(secs);

  // Timezone label at the bottom
  display.setCursor(52, 56);
  display.print(dst ? "PDT" : "PST");

  display.display();
  delay(200);
}
