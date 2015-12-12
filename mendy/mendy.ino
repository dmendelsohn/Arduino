#include "LPD8806.h"
#include "SPI.h"
#include "Keypad.h"
#include <math.h>

const int SOUND_SENSOR = A5; //pin for the Grove Sound Sensor
const int THRESHOLD = 370; //the sound level that will be treated as a 'clap'
const int SOUND_SAMPLE_LENGTH = 10; //the amount of ms to wait before determining to turn off/on
const int CLAP_DURATION_WINDOW = 90; //the amount of ms max to make the number of claps specified (ms)
const int CLAPS_FOR_TRIGGER = 2; //the number of claps for the relay to trigger
int soundSensorReading = 0;
int soundLength = 0;
int previousSoundLength = 0;
int soundSampleLength = SOUND_SAMPLE_LENGTH;
int clapDurationWindow = CLAP_DURATION_WINDOW;
int currentClaps = 0;
int clapperEnabled = 1;

// Number of RGB LEDs in strand:
int nLEDs = 96;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// Initialize the Strip and the Raster and the step counter
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);
int step_count = 0;
int tick_count = 0;
int brightness_counter = 0;
double brightness_multiplier = 1.5;
uint8_t max_brightness = 127;
int mode = 1;
int lastMode = 1;
int tuner = 0;
// 0 = OFF
// 1 = Susskind lights
// 2 = Blue mode
// 3 = Teal mode
// 4 = Green mode
// 5 = Yellow mode
// 6 = Red mode
// 7 = Purple Mode
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[rows] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 9, 10}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols);

void keypadEvent(KeypadEvent key){
  Serial.println("keypadEvent");
  switch (keypad.getState()) {
    case PRESSED:
      switch (key) {
        case '*': tuner = 1;
          break;
        case '#': tuner = -1;
          break;
        default:
          mode = int(key) - 48; //converts to number
          break;
      }
      break;
     case RELEASED:
       break;
     case HOLD:
       break;
  }
}

int num_sound_samples_high = 0;

struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct RGB makeRGB(uint8_t red, uint8_t green, uint8_t blue) {
 struct RGB rgb = {red, green, blue};
 return rgb;
}

struct RGB applyTuning(struct RGB rgb) {
  float scaling_factor = float(pow(brightness_multiplier, abs(brightness_counter)));
  float saturation_factor = 1.0 * max_brightness / max(rgb.r, max(rgb.g, rgb.b));
  uint8_t newR;
  uint8_t newG;
  uint8_t newB;
  struct RGB result;
  if (brightness_counter == 0) {
    return rgb;
  } else if (brightness_counter > 0) { //making it brighter
    float active_factor = min(scaling_factor, saturation_factor); //we don't want to change the lights white necessarily
    newR = int(active_factor * rgb.r);
    newG = int(active_factor * rgb.g);
    newB = int(active_factor * rgb.b);
    result = makeRGB(newR, newG, newB);
    return result;
  } else { //making it darker
    newR = int(1.0 * rgb.r / scaling_factor);
    newG = int(1.0 * rgb.g / scaling_factor);
    newB = int(1.0 * rgb.b / scaling_factor);
    result = makeRGB(newR, newG, newB);
    return result;
  }
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
  if (tuner == 1) {
    brightness_counter += 1;
    tuner = 0;
  } else if (tuner == -1 ) {
    brightness_counter -= 1;
    tuner = 0;
  }
  setMonochromePixelRange(applyTuning(rgb), 0, nLEDs - 1);
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
  
  keypad.addEventListener(keypadEvent);
}

void loop() {
  mode = 0;
  soundSensorReading = analogRead(SOUND_SENSOR);

  if (soundSensorReading >= THRESHOLD) { 
    soundLength++;
    Serial.println("soundSensorReading: ");
    Serial.println(soundSensorReading);
    Serial.println("soundLength");
    Serial.println(soundLength);
  } else {
    if (soundLength > 0) {
      Serial.println("Previous sound length:");
      Serial.println(soundLength);
      previousSoundLength = soundLength;
    }
    soundLength = 0;
  }
  
  if (soundSampleLength == SOUND_SAMPLE_LENGTH) {
    soundSampleLength = 0;

    if (previousSoundLength > 0) { //Used to be 1
      clapDurationWindow = 0;
      currentClaps++;
      Serial.println("Clap");
      Serial.println(currentClaps);

      if (currentClaps == CLAPS_FOR_TRIGGER) {
        if (clapperEnabled) {
          Serial.println("Enough Claps");
          if (mode == 0) {
            mode = lastMode;
          } else {
            lastMode = mode;
            mode = 0;
          }
        }
      }

      previousSoundLength = 0;
    }
  }
  
  if (clapDurationWindow >= CLAP_DURATION_WINDOW) {
    if (currentClaps != 0) {
      currentClaps = 0; 
      Serial.println("Resetting current claps");
    }
  }

  if (clapDurationWindow <= CLAP_DURATION_WINDOW) {
    clapDurationWindow++;
  }

  if (soundSampleLength < SOUND_SAMPLE_LENGTH) {
    soundSampleLength++; 
  }
  
  char c = keypad.getKey();
  switch(mode) {
    case 0:
      offDisplay();
      break;
    case 1:
      susskindDisplay(blue_, green_, red_, step_count);
      tick_count++;
      if (tick_count == 5) {
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
