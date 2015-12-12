#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in strand:
int nLEDs = 160;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 2;
int clockPin = 3;

// Initialize the strip.
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

uint32_t makeColor(float *rgb) {
  uint8_t red = (int) rgb[0];
  uint8_t green = (int) rgb[1];
  uint8_t blue = (int) rgb[2];
  return Strip.color(red, green, blue);
}

float *darker(float *rgb, float scale) {
  float newRgb[3];
  newRgb[0] = rgb[0]/scale;
  newRgb[1] = rgb[1]/scale;
  newRgb[2] = rgb[2]/scale;
  return &newRgb;
}

float *brighter(float *rgb, float scale){
  float newRgb[3];
  newRgb[0] = min(rgb[0]*scale, 127);
  newRgb[1] = min(rgb[1]*scale, 127);
  newRgb[2] = min(rgb[2]*scale, 127);  
  return &newRgb;
}

float *addRgb(float rgb, float r, float g, float b)
{
  float newRgb[3];
  int newRgb[0] = max(min(rgb[0]+r,127),0);
  int newRgb[1] = max(min(rgb[1]+g,127),0);
  int newRgb[2] = max(min(rgb[2]+b,127),0);
  return &newRgb;
}

//Get the color at a specific "pos" in a fade sequence of length "steps"
float *colorInFadeSequence(float *rgbStart, float *rgbFinish, int steps, int pos)
{
  if (steps == 0){
    return finish;
  }
  float deltaR = (rgbFinish[0] - rgbStart[0])*pos/steps;
  float deltaG = (rgbFinish[1] - rgbStart[1])*pos/steps;
  float deltaB = (rgbFinish[2] - rgbStart[2])*pos/steps;
  return addRGB(rgbStart, deltaR, deltaG, deltaB);
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

float (*red)[] = {16,0,0};
float (*green)[] = {0,16,0};
float (*blue)[] = {0,0,16};
float (*purple)[] = {16,0,16};
float (*teal)[] = {0,16,16};
float (*yellow)[] = {16,16,0};
float (*white)[] = {16, 16, 16};

float *colors[]={blue, teal, green, yellow, red, purple}; //array of pointers to floats

int currentIdx = 0;
void loop() {
    fadeFillContinuous(red, red, red ,5);
}

// Chase one dot down the full strip.
int trailLen=6;
int trailMultiplier=25;
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier; i++) {
    int j;
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels())
        strip.setPixelColor(j, darker(c, pow(2, (i-j)/trailMultiplier)));
    }
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      strip.setPixelColor(j, 0);
    }
    delay(wait/(pow(trailMultiplier,2)));
  }

  strip.show(); // Refresh to turn off last pixel
  trailMultiplier++;
}

void chaseFill(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen; i++) {
    int j;
    for (j=i; j>=0 && j>=i-trailLen; j--){
      if (j<strip.numPixels())
        strip.setPixelColor(j, darker(c, pow(2, i-j)));
    }
    strip.show();              // Refresh LED states
    

    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

void fadeChase(uint32_t c1, uint32_t c2, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier; i++) {
    int j;
    uint32_t current = c1;
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels()){
        int numSteps = trailMultiplier*trailLen - (i - j);
        strip.setPixelColor(j, current);
        current = nextInFadeSequence(current, c2, numSteps);
      }
    }
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      strip.setPixelColor(j, 0);
    }
    delay(wait/(pow(trailMultiplier,2)));
  }

  strip.show(); // Refresh to turn off last pixel
}
void fadeFill(uint32_t c1, uint32_t c2,  uint32_t c3, uint8_t wait) {
  int i;

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    int j;
    uint32_t current = c1;
    if (i>=strip.numPixels()+trailLen*trailMultiplier/2-1){
      current=c2;
    }
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels()){
        int numSteps = trailMultiplier*trailLen - (i - j);
        if (numSteps>trailMultiplier*trailLen/2){
          strip.setPixelColor(j, current);
          current = nextInFadeSequence(current, c2, numSteps-trailMultiplier*trailLen/2);
        }
        else{
          strip.setPixelColor(j, current);
          current = nextInFadeSequence(current, c3, numSteps);
        }
      }
    }
    strip.show();              // Refresh LED states

    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

void fadeFillContinuous(uint32_t c1, uint32_t c2,  uint32_t c3, uint8_t wait) {
  int i;

  uint32_t current1=c1;
  uint32_t current2=c2;
  uint32_t current3=c3;
  int offset=0;
  while (true){
    for(i=0; i<strip.numPixels()/3; i++) {
      strip.setPixelColor((i+offset)%strip.numPixels(), colorInFadeSequence(current1,current2,strip.numPixels()/3,i));
    }
    for(i=strip.numPixels()/3; i<2*strip.numPixels()/3; i++) {
      strip.setPixelColor((i+offset)%strip.numPixels(), colorInFadeSequence(current2,current3,strip.numPixels()/3,i-strip.numPixels()/3));
    }
    for(i=2*strip.numPixels()/3; i<strip.numPixels(); i++) {
      strip.setPixelColor((i+offset)%strip.numPixels(), colorInFadeSequence(current3,current1,strip.numPixels()/3,i-2*strip.numPixels()/3));
    }
    delay(wait);
    strip.show(); // Refresh to turn off last pixel
    offset++;
    
  }
}


void fadeFillClear(uint32_t c1, uint32_t c2,  uint32_t c3, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels()+trailLen*trailMultiplier/2; i++) {
    int j;
    uint32_t current = c1;
    if (i>=strip.numPixels()+trailLen*trailMultiplier/2-1){
      current=c2;
    }
    for (j=i; j>=0 && j>=i-trailMultiplier*trailLen; j--){
      if (j<strip.numPixels()){
        int numSteps = trailMultiplier*trailLen - (i - j);
        if (numSteps>trailMultiplier*trailLen/2){
          strip.setPixelColor(j, current);
          current = nextInFadeSequence(current, c2, numSteps-trailMultiplier*trailLen/2);
        }
        else{
          strip.setPixelColor(j, current);
          current = nextInFadeSequence(current, c3, numSteps);
        }
      }
    }
    strip.show();              // Refresh LED states

    delay(wait);
  }

  strip.show(); // Refresh to turn off last pixel
}

// Chase one dot down the full strip.  Good for testing purposes.
void movingLights(uint32_t c, uint8_t wait) {
  int i;
  
  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if (i<strip.numPixels()/2){
       if (i%3 ==0) strip.setPixelColor(i, c);
     }
     else if ((i+2)%3==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);
   strip.show(); 
   
   for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if ((i+1)%3 ==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);

  strip.show(); // Refresh to turn off last pixel
  
   for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

   for(i=0; i<strip.numPixels(); i++) {
     if (i<strip.numPixels()/2){
       if ((i+2)%3 ==0) strip.setPixelColor(i, c);
     }
     else if (i%3==0) strip.setPixelColor(i, c);
   }
   
   delay(wait);

  strip.show(); // Refresh to turn off last pixel
}

