#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 6,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);

Adafruit_DCMotor *motor1 = AFMS.getMotor(3);
Adafruit_DCMotor *motor2 = AFMS.getMotor(4);
Servo servo1;
Servo servo2;

int pos = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting motor suite"));
  AFMS.begin();
  
  //motor1->setSpeed(150);
  //motor1->run(FORWARD);
  
  //motor2->setSpeed(150);
  //motor2->run(BACKWARD);
  
  //servo1.attach(9);
  //servo2.attach(10);
}

void loop() {
  /*matrix.fillScreen(matrix.Color(25, 0, 0));
  matrix.setPixel(
  servo1.write(180);
  delay(2000);
  servo1.write(90);
  delay(2000);
  servo1.write(0);
  delay(2000);
  servo1.write(90);
  delay(2000);*/
}
