#include <CANMotorDriver.h>

CANMotorDriver driver(3);

void setup() {
  Serial.begin(9600);
  delay(1000);
  driver.begin();
}

void loop() {
  driver.sendDriveToPosition(2, 5000);
  delay(5000);
  driver.sendDriveToPosition(2, 10000);
  delay(5000);
}
