uint8_t out = 4;
long lastSwitch = 0;
long halfPeriod; //in micros
boolean isHigh;

long lastNoteSwitch = 0;
long curNote;
long noteLength = 1000000; //in micros

//The below half periods are a C scale, C4-C5
const int NUM_NOTES = 8;
long halfPeriods[NUM_NOTES] = {1911, 1703, 1517, 1432, 1276, 1136, 1012, 956};

void setup() {
  pinMode(out, OUTPUT);
  digitalWrite(out, LOW);
  isHigh = false;
  Serial.begin(9600);
  lastSwitch = micros();
  lastNoteSwitch = lastSwitch;
  halfPeriod = 1136; // f = 440Hz
  curNote = 0;
}

void loop() {
  long curTime = micros();
  if (curTime - lastSwitch > halfPeriod) {
    //if (random(100) == 1) {
    //  Serial.println(curTime - lastSwitch);
    //}
    isHigh = !isHigh;
    digitalWrite(out, isHigh);
    lastSwitch = curTime;
  }
  
  if (curTime - lastNoteSwitch > noteLength) {
    curNote = (curNote+1)%NUM_NOTES;
    halfPeriod = halfPeriods[curNote];
    lastNoteSwitch = curTime; 
  }
}
