#include <Wire.h>
#include <Adafruit_MotorShield.h>

#include "LPD8806.h"
#include "SPI.h"
#include <math.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);

// Number of RGB LEDs in strand:
int nLEDs = 96;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 4;
int clockPin = 5;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
int color_index = 0;
int num_colors = 6;

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

int nextColorIndex(int c) {
  return ((c+1)%num_colors);
}

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
  
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  myMotor->setSpeed(120);
  
  // Start up the LED strip
  strip.begin();

  strip.show();
}

void loop() {
  myMotor->step(1000, FORWARD, DOUBLE); 
  
  solidColorDisplay(colors_[color_index]);
  color_index = nextColorIndex(color_index);
  strip.show();
  
  myMotor->step(1000, BACKWARD, DOUBLE);
    
  solidColorDisplay(colors_[color_index]);
  color_index = nextColorIndex(color_index);
  strip.show();
}
