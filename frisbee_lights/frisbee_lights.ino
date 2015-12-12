#include "LPD8806.h"
#include "SPI.h"

int nLEDs = 4;

int dataPin = 4;
int clockPin = 5;
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

uint32_t c[] = {
  strip.Color(0, 25, 127),
  strip.Color(127, 25, 25),
  strip.Color(25, 127, 25),
  strip.Color(127, 127, 10) };

void setup() {
  strip.begin();
}

void loop() {
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < nLEDs; i++) {
      strip.setPixelColor(i, c[j]);
    }
    strip.show();
    delay(5000);
  }
}
