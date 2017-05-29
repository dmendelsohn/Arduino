void setup() {
  Serial.begin(9600);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
}

void loop() {
  Serial.print("11: ");
  Serial.print(digitalRead(11));
  Serial.print(", 12: ");
  Serial.println(digitalRead(12));
  delay(50);
}
