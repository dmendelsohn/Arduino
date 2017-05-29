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

    
    //Your code here from ex03a!
    
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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
