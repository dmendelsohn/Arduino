//Hello World!
void setup() {
  //Serial.begin(9600);
  pinMode(13, OUTPUT);
  //Serial.println("Hi there");
}

void loop() {
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);
}

