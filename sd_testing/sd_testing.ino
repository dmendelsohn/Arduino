#include <Wire.h>
#include <avr/pgmspace.h>
#include<SPI.h>
#include<SD.h>

void setup() {
  Serial.begin(9600);
  while(!Serial);  
  Serial.println("Beginning test");

  boolean hasFile = SD.exists("cardID.txt");
  Serial.println(hasFile);
  
}

void loop() {
  
}
