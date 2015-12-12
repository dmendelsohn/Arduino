int pin = 0;
void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(9600);
  /*digitalWrite(13, HIGH);
  while(!Serial.available()) {
  }
  digitalWrite(13, LOW);
  Serial.println("foo");*/
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(125);
  //Serial.println("foo");
  for (int i = 0; i <= 13; i++) {
    digitalWrite(i, HIGH);
  }
  delay(250);
  for (int i = 0; i <= 13; i++) {
    digitalWrite(i, LOW);
  }
}
