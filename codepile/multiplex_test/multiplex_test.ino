#include "Multiplex.h"

uint8_t numPins = 16;
uint8_t dataPin = 5; // Connects to 74HC595 #14
uint8_t clockPin = 6; // Connects to 74HC595 #11
uint8_t latchPin = 7; // Connects to 74HC595 #12

uint8_t ledPinOffset = 0; //Offset from first multiplex virtual pin
uint8_t firstMultiplexPin = FIRST_PIN;
uint8_t ledPin = firstMultiplexPin + ledPinOffset;

Multiplex m = Multiplex(numPins, dataPin, clockPin, latchPin);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  while(!Serial) {}
  digitalWrite(13, HIGH); //Signal that serial is connected
  m.begin();
}

void loop() {
  m._digitalWrite(ledPin, HIGH);
  delay(500);
  m._digitalWrite(ledPin, LOW);
  delay(500);
}
