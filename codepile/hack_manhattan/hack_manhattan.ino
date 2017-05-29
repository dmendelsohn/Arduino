unsigned long timeAcc;
unsigned long lastTime;
unsigned long curTime;
int counter;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  
  counter = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(13, HIGH);
  //delay(200);
  //digitalWrite(13, LOW);
  //delay(200);
  curTime = micros();
  timeAcc += (curTime - lastTime);
  lastTime = curTime;
  counter = (counter+1)%1000;
  if (counter == 0) {
     Serial.println(timeAcc);
     timeAcc = 0;
  }
  
   digitalWrite(13, HIGH);
}
