
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <SparkFunLSM9DS1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 display(4);
LSM9DS1 imu;

#define LSM9DS1_M  0x1E // 
#define LSM9DS1_AG  0x6B //

//PASTE IN YOUR FUNCTIONING COPIES OF ANGLE AND BALL CLASSES!!
/*In the Ball class sure to change your x_pos,y_pos and four limit variables
to be public variables so the Game class below can utilize them easily!
*/
    top_limit = 16+ radius;
    float spring = .2;
    float k_friction = 2.0;
    x_pos = x_pos + dt*x_vel;
    y_pos = y_pos + dt*y_vel;
    if (x_pos > right_limit) { //Check right boundary
      x_pos = 2*right_limit - x_pos; //Reflect about right
      x_vel *= -spring;
    } else if (x_pos < left_limit) { //Check left boundary
      x_pos = 2*left_limit - x_pos; //Reflect about left
      x_vel *= -spring;
    }
    if (y_pos > lower_limit) { //Check lower boundary
      y_pos = 2*lower_limit - y_pos; //Reflect about lower
      y_vel *= -spring;
    } else if (y_pos < top_limit) { //Check top boundary
      y_pos = 2*top_limit - y_pos; //Reflect about top
      y_vel *= -spring;
Ball ball;
class Game{
  int score; //score of game
  int food_x_pos; //upper-left position of food
  int food_y_pos; //""
  int food_width; //size of food (is square)
  Ball ball; //instance of ball class associated with Game
  public:
  Game(){
    score= 0; //starting score = 0
    food_x_pos = 40; //start food at 40,40 by default
    food_y_pos = 40;
    food_width = 2; //food is 2 pixels by 2 pixels
  }
  void update(float pitch,float roll){
    display.clearDisplay();
    ball.update(pitch,roll); //update ball position
    collision_detect(); //look for ball hitting food:
    display.fillRect(food_x_pos,food_y_pos, 2,2, WHITE); //draw food
    //show score at top
    display.setCursor(0,0); 
    display.print("Score: ");
    display.println(score);
    display.display();
  }
  private:
  void collision_detect(){
    //Your code here
  }
};



Angle angle; //Create instance of our angle monitor class!
Game awesome; //create instance of our Game class...call it awesome

void setup() {
  Serial.begin(115200);
  delay(2500);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2500);
  display.clearDisplay();
  display.setTextColor(WHITE);

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
}

void loop() {
  angle.update();
  awesome.update(angle.pitch(),angle.roll());
  delay(40);
}
