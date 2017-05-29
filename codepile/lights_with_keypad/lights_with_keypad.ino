#include "LPD8806.h"
#include "SPI.h"
#include <math.h>

// Number of RGB LEDs in strand:
int nLEDs = 160;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
int step_count = 0;
int tick_count = 0;
int ticks_per_step = 5; //Constant
//int brightness_counter = 0;
//double brightness_multiplier = 1.5;
//uint8_t max_brightness = 127;
int mode = 1;

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct RGB makeRGB(uint8_t red, uint8_t green, uint8_t blue) {
 struct RGB rgb = {red, green, blue};
 return rgb;
}

uint32_t makeColor(struct RGB rgb) {
  return strip.Color(rgb.r, rgb.g, rgb.b);
}

struct RGB blue_ = makeRGB(0,0,16);
struct RGB teal_ = makeRGB(0,16,16);
struct RGB green_ = makeRGB(0,16,0);
struct RGB yellow_ = makeRGB(16,16,0);
struct RGB red_ = makeRGB(16,0,0);
struct RGB purple_ = makeRGB(16,0,16);
struct RGB off_ = makeRGB(0, 0, 0);
struct RGB colors_[6] = {blue_, teal_, green_, yellow_, red_, purple_};

void setPixel(struct RGB rgb, int pos) {
  strip.setPixelColor(pos, makeColor(rgb));
}

//includes i and j
void setMonochromePixelRange(struct RGB rgb, int i, int j) {
  if (j >= i && i >= 0 && j < nLEDs) {
    for (int index = i; index <= j; index++) {
      setPixel(rgb, index);
    }
  }
}

//includes i AND j
void setFadePixelRange(struct RGB startColor, struct RGB endColor, int i, int j) {
  i = i%nLEDs;
  j = j%nLEDs;
  struct RGB middleColor;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  if (i <= j) {
    for (int index = i; index <= j; index++) {
      red = startColor.r + (endColor.r - startColor.r) * (index - i) / (j - i);
      green = startColor.g + (endColor.g - startColor.g) * (index - i) / (j - i);
      blue = startColor.b + (endColor.b - startColor.b) * (index - i) / (j - i);
      middleColor = makeRGB(red, green, blue);
      setPixel(middleColor, index);
    }
  } else {
    int newJ = j + nLEDs;
    for (int index = i; index <= newJ; index++) {
      red = startColor.r + (endColor.r - startColor.r) * (index - i) / (newJ - i);
      green = startColor.g + (endColor.g - startColor.g) * (index - i) / (newJ - i);
      blue = startColor.b + (endColor.b - startColor.b) * (index - i) / (newJ - i);
      middleColor = makeRGB(red, green, blue);
      setPixel(middleColor, (index%nLEDs));
    }
  }
}

void susskindDisplay(struct RGB c1, struct RGB c2,  struct RGB c3, int offset) {
  offset = offset%nLEDs;
  setFadePixelRange(c1, c2, offset, offset + strip.numPixels()/3 - 1);
  setFadePixelRange(c2, c3, offset + strip.numPixels()/3, offset + 2*(strip.numPixels()/3) - 1);
  setFadePixelRange(c3, c1, offset + 2*(strip.numPixels()/3), offset - 1);
}

void offDisplay() {
  struct RGB off = makeRGB(0, 0, 0);
  setMonochromePixelRange(off, 0, nLEDs - 1);
}

void solidColorDisplay(struct RGB rgb) {
  setMonochromePixelRange(rgb, 0, nLEDs - 1);
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Start up the LED strip
  strip.begin();

  strip.show();
}

void loop() {
  switch(mode) {
    case 1:
      susskindDisplay(blue_, green_, red_, step_count);
      tick_count++;
      if (tick_count == ticks_per_step) {
        step_count++;
        tick_count = 0;
      }
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      solidColorDisplay(colors_[mode - 2]);
      break;
  }
  strip.show();
  delay(1);
}
