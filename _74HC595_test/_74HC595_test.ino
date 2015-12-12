// This script propagates alternation HIGH and LOW
// Through one or more 74HC595 shift register

uint8_t dataPin = 5; // Connects to 74HC595 #14
uint8_t clockPin = 6; // Connects to 74HC595 #11
uint8_t latchPin = 7; // Connects to 74HC595 #12

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, LOW);
}

void loop() {

  digitalWrite(dataPin, HIGH);
  digitalWrite(clockPin, HIGH);
  delay(100); //So I can see it
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, HIGH);
  delay(100); //So I can see it
  digitalWrite(latchPin, LOW);
  delay(500);
  
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, HIGH);
  delay(100); //So I can see it
  digitalWrite(clockPin, LOW);
  digitalWrite(latchPin, HIGH);
  delay(100); //So I can see it
  digitalWrite(latchPin, LOW);
  delay(500);
}
