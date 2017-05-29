#include <LiquidCrystal.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include<SPI.h>
#include<SD.h>
#include "utils.h"

// Object for LCD text driver
LiquidCrystal lcd(lcd_data_pin, lcd_clock_pin, lcd_latch_pin);

//Buffers for line 1 and line 2 of LCD screen, respectively
char buff1[17];
char buff2[17];

void setup() {
    Serial.begin(9600);
     while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }  
    //output mode for output pins
    pinMode(red_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    
    lcd.begin(16,2);
    setColor(BLUE);
    //maybe need a delay
    strcpy_P(buff1, msgs[0]); //This is memory efficient
    strcpy_P(buff2, msgs[1]); //This is memory efficient
    lcdshow(buff1, buff2);
    lcd.createChar(6, sigma);
    
    SD.begin(4);
    FileSystem.begin();
    bool hasFile = SD.exists("arduino/CARDID.txt");
    Serial.println(hasFile);
    
    user_t user;
    strcpy(user.id, "shit");
    Serial.println("About to check the ID");
    checkID(user);
    Serial.println("Checked the ID");
    
    File f = SD.open("arduino/Fuck.txt", FILE_WRITE);
    f.close();
    
    hasFile = SD.exists("arduino/Fuck.txt");
    Serial.println(hasFile);
}

void loop() {
}

void welcomescreen() { //Sets LCD to default screen
  lcd.clear();
  setColor(BLUE);
  strcpy_P(buff1,msgs[3]);
  strcpy_P(buff2,msgs[7]);
  lcdshow(buff1,buff2);
  lcd.setCursor(11,0);
  lcd.write(6);
  lcd.print("AE");
  lcd.setCursor(0,1);
  lcd.print("Tap/Txt to Enter");
}

void lcdshow(char firstline[], char secondline[]) { //takes two strings and prints to lcd display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("I am a test");
  lcd.setCursor(0,1);
  lcd.print("Still just a test");
}

void setColor(int lcd_color) { //sets given color to LCD
  switch(lcd_color) {  //remember this is active low
    case RED:
       write_lcd_pins(LOW, HIGH, HIGH);
       break;
    case GREEN:
       write_lcd_pins(HIGH, LOW, HIGH);
       break;
    case BLUE:
       write_lcd_pins(HIGH, HIGH, LOW);
       break;
    case PURPLE:
       write_lcd_pins(LOW, HIGH, LOW);
       break;
    case TEAL:
       write_lcd_pins(HIGH, LOW, LOW);
       break;
    case YELLOW:
       write_lcd_pins(LOW, LOW, HIGH);
       break;
    case WHITE:
       write_lcd_pins(LOW, LOW, LOW);
       break;
    case BLACK:
       write_lcd_pins(HIGH, HIGH, HIGH);
       break;
    default:
       write_lcd_pins(HIGH, HIGH, HIGH);
       break;
  }
}

void write_lcd_pins(uint8_t r, uint8_t g, uint8_t b) {
  digitalWrite(red_pin, r);
  digitalWrite(green_pin, g);
  digitalWrite(blue_pin, b);
}


//// AUTHORIZATION CODE STARTS HERE
//looks for id in file, loads associated name into name.  Returns if it succeeded or not.
boolean checkID(user_t user) {
  bool valid_user;
  File file = SD.open(USER_FILEPATH, FILE_READ);
  char id[ID_LENGTH];
  strcpy(id, user.id);
  while (file.available()) {
    char line[LINE_LENGTH];
    readLine(file, line);
    if (isMatch(id, line)) {
      char name[NAME_LENGTH];
      getName(line, name); //Populate the name array, using line
      strcpy(user.name, name); //Put the name in the user object
      return true;  // We found the match!
    }
  }
  return false; //No match in this file
}

void readLine(File file, char line[]) {
  Serial.println("readLine() has been called.");
  int i;
  while(file.available()) { //stops once we've reached limit of line's length
    char c = file.read();
    if (i < LINE_LENGTH) { //will only copy the first LINE_LENGTH characters into line
      line[i] = c;
      i++;
    }
    if (c == '\n') { //will only leave this function once we've found the new line character
      break;
    }
  }
  Serial.print("Here is a line: ");
  Serial.println(line);
}

boolean isMatch(char id[], char line[]) {
  int i = 0;
  while (i < ID_LENGTH) {
    if (line[i] == '\0' || line[i] != id[i]) {
     return false; 
    }
    i++;
  }
  return true;
}

//Finds the name field in line, assumes name is everything after the first ID_LENGTH+1 characters
void getName(char line[], char name[]) {
  int i = 0;
  while (line[i] != '\n' && line[i] != '\0' && i < LINE_LENGTH) {
    if (i >= ID_LENGTH+1) { //Check to make sure we're past the first ID_LENGTH+1 characters
      name[i-ID_LENGTH-1] = line[i];  //Populate character into name array
    }
    i++;
  }
}
////AUTHORIZATION CODE ENDS HERE
