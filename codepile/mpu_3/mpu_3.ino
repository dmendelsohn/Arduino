#include <Wire.h>
#include <math.h>
#include <SPI.h>
#include <TinyGPS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define gpsSerial Serial2

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00  
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00  
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

#define MATH_PI 3.14159

String gps_readings = "";
float lat =0.0; 
float lon = 0.0;
int times_through = 0;

TinyGPS gps;
void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
// This function read Nbytes bytes from I2C device at address Address. 
// Put read bytes starting at register Register in the Data array. 
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();
  
  // Read Nbytes
  Wire.requestFrom(Address, Nbytes); 
  uint8_t index=0;
  while (Wire.available())
    Data[index++]=Wire.read();
}


// Write a byte (Data) in device (Address) at register (Register)
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  // Set register address
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}

void get_IMU_readings(int16_t accel_vals[],int16_t gyro_vals[]){
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);
   // Accelerometer
  accel_vals[0]=-(Buf[0]<<8 | Buf[1]);
  accel_vals[1]=-(Buf[2]<<8 | Buf[3]);
  accel_vals[2]=Buf[4]<<8 | Buf[5];

  // Gyroscope
  gyro_vals[0]=-(Buf[8]<<8 | Buf[9]);
  gyro_vals[1]=-(Buf[10]<<8 | Buf[11]);
  gyro_vals[2]=Buf[12]<<8 | Buf[13];

  // Gyroscope
  gyro_vals[0]=-(Buf[8]<<8 | Buf[9]);
  gyro_vals[1]=-(Buf[10]<<8 | Buf[11]);
  gyro_vals[2]=Buf[12]<<8 | Buf[13];
  
}
float ax_cal = 0;
float ay_cal = 0;
float az_cal = 0;
float gx_cal = 0;
float gy_cal = 0;
float gz_cal = 0;


int16_t accel_readings[3];
int16_t gyro_readings[3];
// Initializations

float acc_pitch=0;
float gyro_pitch=0;
float acc_roll=0;
float gyro_roll=0;

float predicted_pitch = 0; //Initial Estimate
float predicted_roll = 0; //Output of Kalman filter
float Q = 0.1; // Prediction Estimate Initial Guess
float R = 5; // Prediction Estimate Initial Guess
float P00 = 0.1; // Prediction Estimate Initial Guess
float P11 = 0.1; // Prediction Estimate Initial Guess
float P01 = 0.1; // Prediction Estimate Initial Guess
float Kk0, Kk1;


void setup()
{
  
  delay(4000);
  // Arduino initializations
  Wire.begin();
    //display setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(250);
  display.clearDisplay();
  Serial.begin(115200);
  //GPS serial setup
  gpsSerial.begin(9600);
  
  // Set accelerometers low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS,29,0x06);
  // Set gyroscope low pass filter at 5Hz
  I2CwriteByte(MPU9250_ADDRESS,26,0x06);
 
  
  // Configure gyroscope range
  I2CwriteByte(MPU9250_ADDRESS,27,GYRO_FULL_SCALE_1000_DPS);
  // Configure accelerometers range
  I2CwriteByte(MPU9250_ADDRESS,28,ACC_FULL_SCALE_4_G);
  // Set by pass mode for the magnetometers
  I2CwriteByte(MPU9250_ADDRESS,0x37,0x02);
  
  // Request continuous magnetometer measurements in 16 bits
  I2CwriteByte(MAG_ADDRESS,0x0A,0x16);
    // Read accelerometer and gyroscope
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS,0x3B,14,Buf);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  for (int i = 5; i>0; i--){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Cal in:");
    display.print(i);
    display.println(" seconds");
    display.display();
    delay(1000);
  }
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("Don't touch");
  display.println("");
  display.print("Calibrating...");
  display.display();
  for (int i = 0; i<100; i++){
    get_IMU_readings(accel_readings,gyro_readings);
    ax_cal+=0.01*accel_readings[0];
    ay_cal+=0.01*accel_readings[1];
    az_cal+=0.01*accel_readings[2];

    gx_cal+=0.01*gyro_readings[0];
    gy_cal+=0.01*gyro_readings[1];
    gz_cal+=0.01*gyro_readings[2];
    delay(30);
    display.print('.');
    display.display(); 
  }

  //az_cal = az_cal- 8192; //maybe take off one g worth of offset
  ay_cal = ay_cal -8192; //because of orientation of device
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println("");
  display.println("");
  display.println(ax_cal);
  display.println(ay_cal);
  display.println(az_cal);
  display.println(gx_cal);
  display.println(gy_cal);
  display.println(gz_cal);
  display.display();
  delay(8000);
}


unsigned long last = millis();
float dt = 0;
// Main loop, read and display data
void loop()
{
  // ____________________________________
  // :::  accelerometer and gyroscope ::: 
  last = millis() - last;
  dt = last*0.001; //in seconds

  get_IMU_readings(accel_readings,gyro_readings);
  //Accel:
  float ax=accel_readings[0]-ax_cal;
  float ay=accel_readings[1]-ay_cal;
  float az=accel_readings[2]-az_cal;
  //Gyro:
  float gx=gyro_readings[0]-gx_cal;
  float gy=gyro_readings[1]-gy_cal;
  float gz=gyro_readings[2]-gz_cal;
  acc_pitch = atan2(az,ay)*180/MATH_PI;
  gyro_pitch = gyro_pitch + 0.001*gx*dt/32.8;
  acc_roll = atan2(ax,ay)*180/MATH_PI;
  gyro_roll = gyro_roll - 0.001*gy*dt/32.8;

  predicted_pitch = predicted_pitch + 0.001*gx*dt/32.8;
  predicted_roll = predicted_roll - 0.001*gz*dt/32.8;

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

  // :::  Magnetometer ::: 

  
  // Read register Status 1 and wait for the DRDY: Data Ready
  
  uint8_t ST1;
  do
  {
    I2Cread(MAG_ADDRESS,0x02,1,&ST1);
  }
  while (!(ST1&0x01));

  // Read magnetometer data  
  uint8_t Mag[7];  
  I2Cread(MAG_ADDRESS,0x03,7,Mag);
  

  // Create 16 bits values from 8 bits data
  
  // Magnetometer
  int16_t mx=-(Mag[3]<<8 | Mag[2]);
  int16_t my=-(Mag[1]<<8 | Mag[0]);
  int16_t mz=-(Mag[5]<<8 | Mag[4]);
  
  mx +=20;
  my-=40;
  mz+=-300;
  // Magnetometer
  Serial.println("Mag Data:");
  Serial.print("X: ");
  Serial.print (mx,DEC); 
  Serial.print (" Y: ");
  Serial.print (my,DEC);
  Serial.print (" Z: ");
  Serial.println (mz,DEC);
  float dir = 0;
  if (my >0){
    dir = 90 - atan(mx*1.0/my)*180/MATH_PI;
  }
  else if( my <0){
    dir = 270 - atan(mx*1.0/my)*180/MATH_PI;
  }
  else if(my == 0 && mx<0){
    dir = 180; 
  }
  else if(my == 0 && mx>=0){
    dir = 0;
  }
  dir = dir - 16;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(dir);
  display.setTextSize(2);
  printFloat(predicted_pitch,4);
  display.println();
  printFloat(predicted_roll,4);

  display.display();
  //delay(1000);
  
  
  // End of line
  //Serial.println("");
  //delay(1000);    
}

void direction(float direct, String *dir){
  if (-45 < direct && direct < 45){
    
  }
}

///GPS Specific Functions:
bool getGPS(){
  Serial.println("Looking for Love");
  return true;
}


void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) {
     Serial.print('-');
     display.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);
  display.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 
    display.print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    display.print(toPrint);
    remainder -= toPrint;
  }
}







