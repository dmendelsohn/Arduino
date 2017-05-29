#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266.h>


Adafruit_SSD1306 display(4);
LSM9DS1 imu;
#define IOT 1
#define wifiSerial Serial1          // for ESP chip
#define LSM9DS1_M  0x1E // 
#define LSM9DS1_AG  0x6B //

String kerberos = "jamirose";        // UPDATE WITH YOUR KERBEROS
String MAC = "";

#define SSID "Phone"       // network SSID and password
#define PASSWORD "internetz"

ESP8266 wifi = ESP8266(true);  //Change to "true" or nothing for verbose serial output

class Inquirer
{
  char alphabet[45];
  int limit;
  int iterator;
  char message[400]; //preallocate 400 characters for message
  float roll_state;
  float filter_factor = 0.3;
  float hi_threshold = 15;
  float lo_threshold = 5;
  int send_button;
  int select_button;
  int led;
  char c;
  String response;
  
  
  public:
  Inquirer(int button_1, int button_2, int led){
    sprintf(alphabet,"abcdefghijklmnopqrstuvwxyz  ");
    limit = strlen(alphabet);
    iterator = 0;
    send_button = button_1;
    select_button = button_2;
    led = led;
    roll_state = 0;
  }
  void update(float roll){
    if(abs(roll) > lo_threshold){
      roll_state += roll*.01;
    }
    if (roll_state > limit) {
      roll_state = 0;
    }
    if(roll_state < 0){
      roll_state += limit;
    }
    c = alphabet[int(roll_state)];
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.println("Message:");
    display.println(message);
    display.println("Letter To Add:");
    display.print(c);
    display.display();
    if (!digitalRead(select_button)) {
      message[iterator] = c;
      message[iterator+1] = '\0';
      iterator++;
    }
    if (!digitalRead(send_button)) {
      send(message);
      message[0] = '\0';
      iterator = 0;
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(1);
      display.print(message);
      display.println("Response:");
      display.println(response); 
      display.display();
      while (digitalRead(send_button));  
    }
    
  }
  private:
  void send(char *message){
    if (wifi.isConnected() && !wifi.isBusy()) {
      if (wifi.isConnected() && !wifi.isBusy()) {
        wifi.sendRequest(GET, "iesc-s2.mit.edu", 80, "/student_code/jamirose/ex05/ex05e.py", "topic=" + String(message), true);
        while (!wifi.hasResponse());
        if (wifi.hasResponse()) {
          response = wifi.getResponse();
          
        }
    }
    }
    
  }
};


class Angle
{
  float ax_cal;
  float ay_cal;
  float az_cal;
  float gx_cal;
  float gy_cal;
  float gz_cal;

  //float accel_scale = 32768.0//not needed!
  float gyro_scale = 245.0/32768.0;
  
  float acc_pitch;
  float gyro_pitch;
  float acc_roll;
  float gyro_roll;
  float predicted_pitch;
  float predicted_roll;
  unsigned long last_time;
  unsigned long new_time;
  float dt;
  float alpha = 0.95;

  public:
  Angle(){
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
    dt = 0;
    predicted_pitch=0;
    predicted_roll=0;
    
  }

  void calibrate(){
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.println("Don't touch");
    display.println("");
    display.print("Calibrating...");
    display.display();

    
 
    for (int i = 0; i<100 ; ++i){
        imu.readAccel();
        imu.readGyro();
        ax_cal = (imu.ax*0.01 + ax_cal);
        ay_cal = (imu.ay*0.01 + ay_cal);
        az_cal = (imu.az*0.01 + az_cal);
        gx_cal = (imu.gx*0.01 + gx_cal);
        gy_cal = (imu.gy*0.01 + gy_cal);
        gz_cal = (imu.gz*0.01 + gz_cal);
        delay(30);
  }
    az_cal = az_cal-(32768/2);
    
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
    last_time = new_time;
    imu.readAccel();
    imu.readGyro();
    float ax=imu.ax-ax_cal;
    float ay=imu.ay-ay_cal;
    float az=imu.az-az_cal;
    //Gyro:
    float gx=(imu.gx-gx_cal)*gyro_scale;//into dps
    float gy=(imu.gy-gy_cal)*gyro_scale;
    float gz=(imu.gz-gz_cal)*gyro_scale;
    acc_pitch = -atan2(ay,az)*180/PI;
    acc_roll = -atan2(ax,az)*180/PI;
    predicted_pitch = alpha*(predicted_pitch + gx*dt)+(1-alpha)*acc_pitch;
    predicted_roll = alpha*(predicted_roll - gy*dt) + (1-alpha)*acc_roll;  
  }
  float pitch(){
    return predicted_pitch;
  }
  float roll(){
    return predicted_roll;
  }
  
};

int button_1 = 11;
int button_2 = 12;
int led = 13;

Angle angle;
Inquirer inq(button_1,button_2,led);

void setup() {
  Serial.begin(115200);
  delay(2500);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  if (IOT) {
    wifi.begin();
    wifi.connectWifi(SSID, PASSWORD);
    while (!wifi.isConnected()); //wait for connection
    MAC = wifi.getMAC();
  }

  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  if (!imu.begin())
  {
    while (1){
      Serial.println("Comm Failure with LSM9DS1");
      delay(500);
    }
  }

  display.clearDisplay();
  display.setCursor(0,0);

  angle.calibrate();
  randomSeed(analogRead(0));//seed random number
  display.setTextSize(2);
  pinMode(button_1,INPUT);
  pinMode(button_2,INPUT);
  pinMode(led,OUTPUT);
  pinMode(button_1, INPUT_PULLUP);
  pinMode(button_2, INPUT_PULLUP);
  digitalWrite(led,LOW);
}

void loop() {
  angle.update();
  inq.update(angle.roll());
  delay(50);
}
