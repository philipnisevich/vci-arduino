// Scrolls "Philip" across the UNO R4 WiFi's built-in 12x8 LED matrix.
// ArduinoGraphics must be included before Arduino_LED_Matrix.
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();
}

void loop() {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(60);   // ms per pixel step; lower = faster
  matrix.textFont(Font_5x7);
  matrix.beginText(12, 1, 0xFFFFFF);  // start just off the right edge
  matrix.println("  Philip  ");
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}
