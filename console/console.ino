#include <Console.h>

const int ledPin = 13;
int incomingByte;

void setup() {
  // initialize serial communication
  Bridge.begin();
  Console.begin();
  
  while(!Console) {
    ; // wait for Console port to connect
  }
  
  Console.println("You're connected to the Console!");
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // see if there's incoming serial data:
  if (Console.available() > 0) {
    //read the oldest byte in the serial buffer:
    incomingByte = Console.read();
    if (incomingByte == 'H') {
      digitalWrite(ledPin, HIGH);
    }
    if (incomingByte == 'L') {
      digitalWrite(ledPin, LOW);
    }
  }
  delay(100);
}
