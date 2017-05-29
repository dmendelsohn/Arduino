const int dot_length = 50; //milliseconds
const int oversampling_rate = 10;
const int loop_time = dot_length/oversampling_rate;

bool listen_state = false; //false nothing
int mid_threshold =230;
int meas_since_sig = 0; //used for counting how long since last High reading
const int TIMEOUT = 9*oversampling_rate; //number of measurements before stops listening
unsigned long point_time = millis();

void setup() {
  Serial.begin(115200);
  analogReadRes(12); //set the analog-input to 12 bits of resolution!
}

void loop() {
  point_time = millis();
  int input = analogRead(0);
  //Serial.println(input);
  if (listen_state){
    if (input >mid_threshold){
      Serial.print(1);
      meas_since_sig = 0;
    }
    else {
      Serial.print(0);
      meas_since_sig +=1;
    }
    if (meas_since_sig >=TIMEOUT){
      listen_state = false;
      meas_since_sig = 0;
    }
    
  }else{
    if (input >mid_threshold){
      listen_state = true;
    }
  }
  //Determine how much time has been spent in the loop
  /*delay dot_length - that time to ensure each run through
   * the loop takes approximately dot_length milliseconds!
   */
  unsigned long wait_time = millis() - point_time;
  if (wait_time < loop_time) {
    delay(loop_time - wait_time);
  }
}
