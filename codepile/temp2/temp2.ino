void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  while (!Serial);
  while (!Serial1);
  Serial1.println("AT+CIPSTART=\"TCP\",\"www.adafruit.com\",80");
  displaySerial(2000);
  Serial1.println("AT+CIPSEND=67");
  displaySerial(2000);
  Serial1.println("GET /testwifi/index.html? HTTP/1.1\r\nHost: www.adafruit.com:80\r\n\r\n");
  displaySerial(2000);
}

void displaySerial(unsigned long timeout) {
  unsigned long t = millis();
  while (millis() - t < timeout) {
    if (Serial1.available() > 0) {
      char c = Serial1.read();
      Serial.print(c);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial1.available() > 0) {
    char c = Serial1.read();
    Serial.print(c);
  }
}
