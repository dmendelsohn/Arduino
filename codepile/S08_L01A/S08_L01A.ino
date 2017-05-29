#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int tempPin = 38;

void setup() {
  pinMode(tempPin, INPUT);
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // I2C addr 0x3D
  // init done
  
  // Clear the buffer.
  display.clearDisplay();

  // Display a custom message.
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("I love");
  display.println("6.S08");
  display.display();

  delay(2000);
}

void loop() {
  //Display the temperature
  int sensorValue = analogRead(tempPin);
  float voltage = float(sensorValue)*1.2/4095;
  float tempC = -583.1*(voltage-0.719) + 25;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Sensor Value: ");
  display.println(sensorValue);
  display.print("Voltage: ");
  display.println(voltage);
  display.print("Temperature: ");
  display.println(tempC);
  display.display();
}
