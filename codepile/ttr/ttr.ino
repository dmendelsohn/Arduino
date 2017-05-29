#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

LSM9DS1 imu;
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

#define wifiSerial Serial1
#define serialYes true
#define buzzerPin 3

const String SSID = "TP-LINK_C8DB6E";
const String PASSWD = "16622098";
const int wifiControlPin = 2;
const unsigned long STEPTIME = 4000;
const String kerb = "lapentab";

enum Phase {
  DOWNLOADING,
  PREPPING,
  PLAYING,
  UPLOADINGRESULTS,
  DONE
};

//Quick class to implement a Kalman filter
class Kalman
{
  float ax_cal;
  float ay_cal;
  float az_cal;
  float gx_cal;
  float gy_cal;
  float gz_cal;


  float accel_scale = 2*9.81/32768.0;
  float gyro_scale = 245.0/32768.0;
  
  float acc_pitch;
  float gyro_pitch;
  float acc_roll;
  float gyro_roll;

  float acc_turn_angle;
  float gyro_turn_angle;
  float total_flat_turn;
  
  unsigned long last_time;
  unsigned long new_time;
  float dt;
  
  float predicted_pitch; //Initial Estimate
  float predicted_roll; //Output of Kalman filter
  float Q = 0.1; // Prediction Estimate Initial Guess
  float R = 5; // Prediction Estimate Initial Guess
  float P00 = 0.1; // Prediction Estimate Initial Guess
  float P11 = 0.1; // Prediction Estimate Initial Guess
  float P01 = 0.1; // Prediction Estimate Initial Guess
  float Kk0, Kk1;
  public:
  Kalman(){
    ax_cal=0;
    ay_cal=0;
    az_cal=0;
    gx_cal=0;
    gy_cal=0;
    gz_cal=0;
    acc_pitch=0;
    gyro_pitch=0;
    acc_roll=0;
    gyro_roll=0;
    last_time = 0;
    new_time = 0;
    total_flat_turn = 0;
    dt = 0;
    
  }
  void calibrate(){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.println("Don't touch");
    display.println("");
    display.print("Calibrating...");
    display.display();
    for (int i = 0; i<50; i++){
      imu.readAccel();
      imu.readGyro();
      ax_cal+=0.01*imu.ax;
      ay_cal+=0.01*imu.ay;
      az_cal+=0.01*imu.az;
  
      gx_cal+=0.01*imu.gx;
      gy_cal+=0.01*imu.gy;
      gz_cal+=0.01*imu.gz;
      delay(30);
      display.print('.');
      display.display(); 
    }
    az_cal-=16384.0; //gravitycorrection assuming 2g full scale!!!
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(ax_cal);
    display.println(ay_cal);
    display.println(az_cal);
    display.println(gx_cal);
    display.println(gy_cal);
    display.println(gz_cal);
    display.display();
    delay(800); 
  }

  void update(){
    new_time = millis();
    dt = (new_time-last_time)*0.001; //in seconds
    //Serial.println(new_time - last_time);
    last_time = new_time;
    //Serial.println(last_time);
    
    
    imu.readAccel();
    imu.readGyro();
    float ax=imu.ax-ax_cal;
    float ay=imu.ay-ay_cal;
    float az=imu.az-az_cal;
    //Gyro:
    float gx=(imu.gx-gx_cal)*gyro_scale;//into dps
    float gy=(imu.gy-gy_cal)*gyro_scale;
    float gz=(imu.gz-gz_cal)*gyro_scale;
    acc_pitch = atan2(az,ay)*180/PI;
    gyro_pitch = gyro_pitch + gx*dt;
    acc_roll = atan2(az,ax)*180/PI;
    gyro_roll = gyro_roll - gy*dt;
  
    predicted_pitch = predicted_pitch + gx*dt;
    predicted_roll = predicted_roll - gz*dt;
  
    P00 += dt * (2 * P01 + dt * P11); // Projected error covariance terms from derivation result:
    P01 += dt * P11; // Projected error covariance terms from derivation result
    P00 += dt * Q; // Projected error covariance terms from derivation result
    P11 += dt * Q; // Projected error covariance terms from derivation result
    Kk0 = P00 / (P00 + R); // Measurement Update step 1
    Kk1 = P01 / (P01 + R); // Measurement Update step 1
  
    predicted_pitch += (acc_pitch - predicted_pitch) * Kk0; // Measurement Update step 2
    predicted_roll += (acc_roll - predicted_roll) * Kk0; // Measurement Update step 2
  
    P00 *= (1 - Kk0); // Measurement Update step 3
    P01 *= (1 - Kk1); // Measurement Update step 3
    P11 -= Kk1 * P01; // Measurement Update step 3 
  }
  float pitch(){
    return predicted_pitch-90;
  }
  float roll(){
    return predicted_roll-90;
  }
  void reset_turn(){
    Q = 0.1; // Prediction Estimate Initial Guess
    R = 5; // Prediction Estimate Initial Guess
    P00 = 0.1; // Prediction Estimate Initial Guess
    P11 = 0.1; // Prediction Estimate Initial Guess
    P01 = 0.1; // Prediction Estimate Initial Guess
    total_flat_turn = 0; 
    last_time = millis();
    new_time = millis();
  }
  float update_angle(){
    new_time = millis();
    dt = (new_time-last_time)*0.001; //in seconds
    //Serial.println(new_time - last_time);
    last_time = new_time;
    //Serial.println(last_time);
    imu.readGyro();
    imu.readAccel();
    float ax=(imu.ax-ax_cal)*accel_scale;

    //Gyro:

    float gz=(imu.gz-gz_cal)*gyro_scale; //dps
    //acc_turn_angle = (180.0/3.14159)*ax*dt*dt/2.0 + acc_turn_angle;
    float acc_turn = (180.0/3.14159)*ax*dt/0.5; //dps (assumes 3 foot arm outstretch...maybe not right
    //acc_turn_angle = 0;
    //gyro_turn_angle = gyro_turn_angle + gz*dt;
    float angle_rate = 90.0/4000*(0.98*gz + acc_turn*0.02);
    
    //Serial.println(acc_turn_angle);
    //Serial.println(gyro_turn_angle);
    total_flat_turn = total_flat_turn + angle_rate*dt;
    return total_flat_turn*180.0/PI;
    
  }
};

String wifis = ""; //TODO: Remove this I think
bool wifi_good = false; //TODO: Remove this I think
Kalman angle;

void setup() 
{
  delay(2500);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.display();
  delay(2500);

  //usb comm setup
  if (serialYes){
    Serial.begin(9600); //number meaningless on teensy (1MBit/s)
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Doing IMU setup...");
  delay(250);
  setupImu();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Doing Wifi setup...");
  delay(250);
  setupWifi();

  display.clearDisplay();
  display.setCursor(0, 0);
  angle.calibrate();
  angle.reset_turn();
}

void setupImu() {
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1)
      ;
  }
}

void setupWifi() {
  wifiSerial.begin(115200);
  wifiSerial.setTimeout(4000);
  if (check()){
    resetWifi(); 
    //delay(4000);//letting system get happy
  } 
}

String target_steps = "";
String recorded_steps = "";
int step_count = 0;
float ref_angle;
Phase phase = DOWNLOADING;
unsigned long reset_timer = millis();
unsigned long phase_timer = millis();
void loop() 
{
  if (phase == DOWNLOADING) {
    maintainWifi(); //checks and resets the wifi connection if needed
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Downloading Steps");
    display.display();
    delay(1000);
    target_steps = getSteps();
    //display.println(target_steps);
    //display.display();
    //delay(4000);
    angle.reset_turn(); //Reset the Kalman filter
    reset_timer = millis();
    phase = PREPPING;
  } else if (phase == PREPPING) {
    displayStartSequence();
    ref_angle = angle.update_angle();
    phase = PLAYING;
    phase_timer = millis();
  } else if (phase == PLAYING) {
    float heading = angle.update_angle() - ref_angle;
    int cur_step = (millis() - phase_timer)/STEPTIME;
    if (cur_step > step_count) { //We just finished step w/ index step_count
      recorded_steps += convertHeadingToCardinal(heading);
      step_count = cur_step;
      //TODO: make a sound effect
    }
    if (step_count < target_steps.length()) {
      float target = convertCardinalToHeading(target_steps[step_count]);
      displayCurrentStep(heading, target);
    } else {
      //TODO: make a sound effect
      phase = UPLOADINGRESULTS;   // We're done!
    }
  } else if (phase == UPLOADINGRESULTS) {
    maintainWifi(); //checks and resets the wifi connection if needed
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Uploading Results");
    display.display();
    delay(1000);
    String result = sendSteps(target_steps, recorded_steps);
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(result);
    display.display();
    phase = DONE;
  }
}

void maintainWifi() {
  if (wifi_good != true){
    if (check()){
      listWifis();
      if (serialYes){
        Serial.println(wifis);
      }
      bool good = connectWifi(SSID,PASSWD);
      if (good){
        wifi_good = true;
      }else{
        wifi_good=false;
      }
    }
  }
}

// Sends GET request to get the steps and retries until successful
String getSteps() {
  String domain = "iesc-s2.mit.edu";
  int port = 80;
  String path = "/student_code/" + kerb + "/lab03/lab03.py?gen=1";
  String response;
  while (!httpGET(domain, port, path, &response)) {
    maintainWifi(); //Try to reconnect if GET failed
  }
  return getHtmlBody(response);
}

// Sends POST request to report the results, and retries until successful
String sendSteps(String target_steps, String recorded_steps) {
  String domain = "iesc-s2.mit.edu";
  int port = 80;
  String path = "/student_code/" + kerb + "/lab03/lab03.py";
  String postdata = String("calculate=1")
    + String("&kerb=") + kerb
    + String("&actual=") + target_steps
    + String("&player=") + recorded_steps;
  String data = "";
  String response;
  while (!httpPOST(domain, port, path, postdata, &response)) {
    maintainWifi(); //Try to reconnet if POST failed
  }
  return getHtmlBody(response);
  //return String(target_steps + "," + recorded_steps); // For debugging
}

// Displays "Ready/Set/Go" to the screen, accompanied with sound effects
void displayStartSequence() {
  display.clearDisplay();
  display.setCursor(0,0);
  tone(buzzerPin,440,200);
  display.println("Ready");
  display.display();
  delay(1000);
  tone(buzzerPin,440,200);
  display.println("Set");
  display.display();
  delay(1000);
  tone(buzzerPin,880,1000);
  display.println("Go!!!");
  display.display();
  delay(1000);
}

void displayCurrentStep(float heading, float target) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Target: ");
  display.println(target);
  display.print("Actual: ");
  heading = fmod(heading, 360);
  if (heading < 0) {
    heading += 360;
  }
  display.println(heading);
  display.display();
}

//Converts a heading (not necessarily between 0 and 360) to cardinal direction
String convertHeadingToCardinal(float heading) {
  heading = float(int(heading)%360);
  if (heading < 0) {
    heading += 360; //Make sure we're in 0-360 range
  }

  if (heading >= 45 && heading < 135) {
    return "e";
  } else if (heading >= 135 && heading < 225) {
    return "s";
  } else if (heading >= 225 && heading < 315) {
    return "w";
  } else {
    return "n";
  }
}

// Converts cardinal ("n","e", "s", "w") to heading (0, 90, 180, 270)
float convertCardinalToHeading(String cardinal) {
  if (cardinal=="N" || cardinal=="n") {
    return 0;
  } else if (cardinal=="E" || cardinal=="e") {
    return 90;
  } else if (cardinal=="S" || cardinal=="s") {
    return 180;
  } else {
    return 270;
  }
}

String getHtmlBody(String html) {
  int startIndex = html.indexOf("<body>");
  int endIndex = html.indexOf("</body>");
  String body = "";
  if (startIndex != -1 && endIndex != -1) {
    startIndex += 7; //Chop off tag and new line char
    endIndex -= 1;  //Chop off new line char
    body = html.substring(startIndex, endIndex);
  }
  return body;
}

//////
//////
///Wifi Functions:

void wifiEnable(bool yn){
  digitalWrite(wifiControlPin,LOW);
  if (yn){
    delay(250);
    digitalWrite(wifiControlPin,HIGH);
    delay(1000);
  } 
}
bool check() {
    wifiSerial.println("AT");
    if (serialYes) Serial.println("checking..");
    boolean ok = false;
    if (wifiSerial.find("OK")) {
        if (serialYes) Serial.println("ESP8266 present");
        ok = true;
    }
    return ok;
}
void resetWifi() {
  // set station mode
    wifiSerial.println("AT+CWMODE=3");
    delay(1000);//give some breathing room
    wifiSerial.println("AT+RST"); //reset required to take effect
    delay(2000);
    if (wifiSerial.find("ready")){
      if (serialYes) Serial.println("ESP8266 restarted and ready");
    }
    printWifiResponse();
}

bool connectWifi(String ssid, String password) {
    String cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
    wifiSerial.println(cmd);
    unsigned long start = millis();
    String response="";
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Seeking Connection to:");
    display.println(ssid);
    display.display();
    while (millis() - start <9000){ //probably can rewrite this thing if needed.
      if (wifiSerial.available()){
        char c = wifiSerial.read();
        if (serialYes) Serial.print(c);
        response = String(response+c);
      }
      if (response.indexOf("OK") != -1){
        break;
      }
    }
    if(response.indexOf("OK") !=-1) {
        wifiSerial.println("AT+CIFSR");
        String resp2 = "";
        start = millis();
        while(millis()-start < 7000){
          if(wifiSerial.available()){
            char c = wifiSerial.read();
            //Serial.print(c);
            resp2 = String(resp2+c);
          }
        }
        if (serialYes){
          Serial.println("Device IP Info: ");
          Serial.println(resp2);
          Serial.println("Connected!");
          return true;
        }
    }
    else {
        if (serialYes) Serial.println("Cannot connect to wifi");
        return false;
    }
}

void listWifis(){
  wifis="";
  if (serialYes) Serial.println("Checking for Wifis!!!");
  wifiSerial.println("AT+CWLAP");
  unsigned long start = millis();
  while (millis() - start <5000){
    if (wifiSerial.available()){
      char c = wifiSerial.read();
      wifis = String(wifis+c);
      //Serial.write(c);
    }
  }
}

void printWifiResponse(){
  while (wifiSerial.available()>0){
    char cByte = wifiSerial.read();
    Serial.write(cByte);
  }
}

bool httpGET(String domain, int port, String path, String* response){
  bool is_good = false;
  String start_comm = "AT+CIPSTART=\"TCP\",\"" + domain + "\","+String(port); //443 is HTTPS, still to do
  Serial.println(start_comm);
  String send_comm = "GET " + path + " HTTP/1.1\r\nHost: " 
    + domain + "\r\n\r\n\r\n";  //The domain part here is necessary at times, it seems
  //Serial.println(send_comm);
  int get_length = send_comm.length();
  //Serial.println(get_length);
  wifiSerial.println(start_comm);
  delay(200);
  if (wifiSerial.find("OK")) {
    String length_comm = String("AT+CIPSEND=" +String(get_length));
    Serial.println(length_comm);
    wifiSerial.println(length_comm);
    Serial.println("waiting for > symbol...");
    String dump="";
    unsigned long start = millis();
    delay(2000);
    if (wifiSerial.find(">")) {
      Serial.println("We're good");
      Serial.println(send_comm);
      wifiSerial.println(send_comm);
      start = millis();
      while (millis() - start <2000){ //probably can shorten this thing if needed.
        if (wifiSerial.available()){
          char c = wifiSerial.read();
          Serial.print(c);
          *response=String(*response+c);
        }
      }
      is_good = true;
    }
  }
  return is_good;
}

bool httpPOST(String domain, int port, String path, String data, String* response) {
  bool is_good = false;
  // initiate TCP connection
  String start_comm = "AT+CIPSTART=\"TCP\",\"" + domain + "\","+String(port); //443 is HTTPS, still to do
  // prepare the data to be posted
  String send_comm =
      "POST " + path + " HTTP/1.1\r\n" +
      "Host: " + domain + ":" + String(port) + "\r\n" +
      "Accept: *" + "/" + "*\r\n" +
      "Content-Length: " + data.length() + "\r\n" +
      "Content-Type: application/x-www-form-urlencoded\r\n" +
      "\r\n" +
      data;
  int get_length = send_comm.length();
  wifiSerial.println(start_comm);
  delay(200);
  if (wifiSerial.find("OK")) {
    String length_comm = String("AT+CIPSEND=" +String(get_length));
    Serial.println(length_comm);
    wifiSerial.println(length_comm);
    Serial.println("waiting for > symbol...");
    String dump="";
    unsigned long start = millis();
    if (wifiSerial.find(">")) {
      Serial.println("We're good");
      Serial.println(send_comm);
      wifiSerial.println(send_comm);
      start = millis();
      while (millis() - start <2000){ //probably can shorten this thing if needed.
        if (wifiSerial.available()){
          char c = wifiSerial.read();
          Serial.print(c);
          *response=String(*response+c);
        }
      }
      is_good = true;
    }
  }
  return is_good;
}

