#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

#include "LPD8806.h"
#include "SPI.h"

//Constants
int nLEDs = 160;
int dataPin = 2;
int clockPin = 3;
int loop_delay = 1; //controls animation speeds

//Global variables
int step_count = 0;
int displayMode = 1;
float brightness = 1.0;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
YunServer server;

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

struct RGB scaleColor(struct RGB rgb, float scale) { //scale is expected to be from 0 to 1, inclusive
  uint8_t red = (int)(rgb.r*scale);
  uint8_t green = (int)(rgb.g*scale);
  uint8_t blue = (int)(rgb.b*scale);
  return makeRGB(red, green, blue);
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
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(3000);
  
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  
  server.listenOnLocalhost();
  server.begin();
  
  strip.begin();
  strip.show();
}

void loop() {
  YunClient client = server.accept();
  if (client) {
    process(client);
    client.stop();
  }
  
  struct RGB redAdjusted;
  struct RGB greenAdjusted;
  struct RGB blueAdjusted;
  struct RGB solidColorAdjusted;
  switch(displayMode) {
    case 0:
      offDisplay();
      break;
    case 1:
      blueAdjusted = scaleColor(blue_, brightness);
      greenAdjusted = scaleColor(green_, brightness);
      redAdjusted = scaleColor(red_, brightness);
      susskindDisplay(blueAdjusted, greenAdjusted, redAdjusted, step_count);
      step_count+=1;
      step_count = step_count % nLEDs;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      solidColorAdjusted = scaleColor(colors_[displayMode-2], brightness);
      solidColorDisplay(solidColorAdjusted);
      break;
  }
  strip.show();
  delay(loop_delay);
}

void process(YunClient client) {
  String command = client.readStringUntil('/');
  
  if (command == "digital") {
    digitalCommand(client);
  }
  if (command == "analog") {
    analogCommand(client);
  }
  if (command == "pin") {
    pinModeCommand(client);
  }
  if (command == "mode") {
    displayModeCommand(client);
  }
  if (command == "brightness") {
    brightnessCommand(client);
  }
}

void digitalCommand(YunClient client) {
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
    
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to digital "));
    client.println(value);
    
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    value = digitalRead(pin);
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" reads digital "));
    client.println(value);
    
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
}

void analogCommand(YunClient client) {
  int pin, value;
  pin = client.parseInt();
  if (client.read() == '/') {
    value = client.parseInt();
    analogWrite(pin, value);
    
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);
    
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    value = analogRead(pin);
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);
  
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  
  }
}

void pinModeCommand(YunClient client) {
  int pin;
  pin = client.parseInt();
  
  if (client.read() != '/') {
    client.println(F("error"));
    return;
  }
  
  String mode = client.readStringUntil('\r');
  
  if (mode == "input") {
    pinMode(pin, INPUT);
    client.print(F("Pin D"));
    client.print(pin);
    client.println(F(" configured as INPUT!"));
    return;
  }
  
  if (mode == "output") {
    pinMode(pin, OUTPUT);
    client.print(F("Pin D"));
    client.print(pin);
    client.println(F(" configured as OUTPUT!"));
    return;
  }
  
  client.print(F("error: invalid mode "));
  client.println(mode);
}

void displayModeCommand(YunClient client) {
    String s = client.readStringUntil('\r');
    
    if (s == "off") {
      displayMode = 0;
    } else if (s == "pretty") {
      displayMode = 1;
    } else if (s == "blue") {
      displayMode = 2;
    } else if (s == "teal") {
      displayMode = 3;
    } else if (s == "green") {
      displayMode = 4;
    } else if (s == "yellow") {
      displayMode = 5;
    } else if (s == "red") {
      displayMode = 6;
    } else if (s == "purple") {
      displayMode = 7;
    }
    
    client.print(F("Setting display to mode: "));
    client.println(s);
    client.print(F("New mode ID: "));
    client.println(displayMode);
}

void brightnessCommand(YunClient client) {
  float f = client.parseFloat();
  if (f > 1.0) {
    brightness = 1.0;
    client.println(F("Input too high, setting brightness to 1.0"));
  } else if (f < 0.0) {
    brightness = 0.0;
    client.println(F("Input too low, setting brightness to 0.0"));
  } else {
    brightness = f;
    client.print(F("Setting brightness to "));
    client.println(f);
  }
}
  
