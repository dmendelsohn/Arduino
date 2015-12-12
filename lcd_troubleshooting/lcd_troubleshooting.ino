#include <LiquidCrystal.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <SPI.h>

//LiquidCrystal lcd(1, 0, 2);
LiquidCrystal lcd(5,6,0,1,2,3);
void setup() {
  pinMode(7, OUTPUT);
  pinMode(4 ,OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  
  lcd.begin(16,2);
  lcd.print("Fuck you");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  lcd.clear();
  lcd.print("Fuck you");
}
