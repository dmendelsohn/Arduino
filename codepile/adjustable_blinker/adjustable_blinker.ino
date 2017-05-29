int period = 1000;
int lastSwitch;
int currentState;
int minPeriod = 100;
int maxPeriod = 2000;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);
  digitalWrite(13, LOW);
  currentState = LOW;
  lastSwitch = millis();
}

void loop() {
  int val = analogRead(A0);
  int period = minPeriod + (maxPeriod - minPeriod) * (float(val) / 1023);
  long curTime = millis();
  if ((curTime - lastSwitch) >= period/2) { //Switch
    if (currentState == HIGH) {
      currentState = LOW;
      lastSwitch = curTime;
      digitalWrite(13, LOW);
    } else {
      currentState = HIGH;
      lastSwitch = curTime;
      digitalWrite(13, HIGH);
    }
  }
}
