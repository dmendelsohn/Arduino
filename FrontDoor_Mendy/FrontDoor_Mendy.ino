//initializations for RFID component
#include <LiquidCrystal.h>
#include <Wire.h>
#include <avr/pgmspace.h>

//Initializations for Text component
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>

#define TAG 17 // A3

//Pins
LiquidCrystal lcd(1, 0, 2); //(Data, Clock, Latch), sets up LCD driver on given pins
byte backupPin = 7; // NO relay, also protects POE from fault on startup
byte relayPin = 8; //tied to greenpin, NC relay
byte redpin = 6; //defines digital out pin for red LCD backlight
byte greenpin = 5; //defines digital out pin for green LCD backlight
byte bluepin = 3; //defines digital out pin for blue LCD backlight

//Definitions for RFID component
byte i=0;
char fulltag[5];
byte data = 0;
byte command = 0;
byte status = 0;
byte len = 0;

//Save regular messages in the program memory to save var memory space
char buff1[17];
char buff2[17];
prog_char msgs[][17] PROGMEM = 
{
  "Initializing... ",  //0
  "Drevo likes dick",  //1
  "No Response     ",  //2
  "Welcome to      ",  //3
  "Welcome brother ",  //4
  "User Not        ",  //5
  "Authorized      ",  //6
  "                ",  //7
  "User Already    ",  //8
  "Exists          ",  //9
  "Could Not Find  ",  //10
  "Removed         ",  //11
  "Couldn't Read   ",  //12
  "Tap Card to Add ",  //13
  "Add Successful  ",  //14
  "Can't Connect   ",  //15
  "To Back Door    ",  //16
  "Time Out        ",  //17
  "User Not Added  "  //18
};
byte nCount = 0;

byte sigma[8] = { //defines character for sigma
  B11111,
  B10000,
  B01000,
  B00100,
  B01000,
  B10000,
  B11111,
  B00000
};

//Definitions for Text component
// network info
byte mac[]  = { 0x90, 0xA2, 0xDA, 0x00, 0x94, 0x9F};
byte ip[]   = { 10, 10, 10, 2 }; 
byte serv[] = { 10, 10, 10, 1 }; //sae.mit.edu
byte back[] = { 10, 10, 10, 3 }; //back door

// texting system vars
EthernetServer server(80);
EthernetClient front;
char recd[40];  //made this slightly smaller
byte count=0; //combined counter variables, will combine with nCount as well once remRFID is added
char c='0';
char name[17];
boolean authFlag = 0; // flag for various uses

// SD variables
//File logFile;
File cardID; //card ID numbers
char cardIDpath[] = "cardID.txt";
File backupID; //backup file
char backupIDpath[] = "backupID.txt";
//File phNums; //for future use, already on the SD Card
unsigned long starttime=0; //to be used in timeout for rfid user adding
unsigned long curtime=0;

void setup() { //took out all the delay(200) statements, works fine and saves space
  //define pins for the relays and LEDs
  pinMode(relayPin, OUTPUT);
  pinMode(backupPin, OUTPUT); 
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
  pinMode(redpin, OUTPUT);
  //LCD Setup
  lcd.begin(16, 2);
  setcolor('w');
  delay(200);
  strcpy_P(buff1,msgs[0]); //writing standard msgs to lcdshow() this way saves memory and prevents crashes
  strcpy_P(buff2,msgs[1]);
  lcdshow(buff1,buff2);
  lcd.createChar(6,sigma);
  //startup sequence
  digitalWrite(relayPin, HIGH); //unlock while booting 
  digitalWrite(backupPin, LOW);
  delay(5000); //wait
  digitalWrite(relayPin, LOW); //now lock 
  digitalWrite(backupPin, HIGH);
  //Setup for text system
  //start ethernet functions
  Ethernet.begin(mac,ip);
  delay(1000); //give it a second to ensure connection
  //initialize the server function
  server.begin();  
  //start SD card 
  SD.begin(4);  
  //Setup for RFID system
  Wire.begin();
  ledOn(true);
  delay(500);
  ledOn(false);
  //Restore from backup on restart
  if (restore()) lcdshow((char *) "Restored", (char *) ""); //restore is a function i wrote to re-create cardID from the backup on startup
  else lcdshow((char *) "Not Restored", (char *) ""); //there were issues with accessing cardID if the program crashed in the middle of a read/write
  delay(1000);
  welcomescreen();
}

void loop() {
  while (digitalRead(TAG)){  //no card present, listen for text msgs
    //wait for client connection
    EthernetClient client = server.available();
    if (client) {
      //re-initialize
      memset(recd,'\0',sizeof(recd)); //re-initializations
      memset(name,'\0',sizeof(name));
      i = 0;
      count = 0;
      while (client.connected()) {
        // Record message to recd[] variable
        if (client.available()) {
          c = client.read();
          recd[i] = c;
          if (c == ' ') {// save the last space to be used for name parsing
            count = i;
          }
          else if (c == '\n') {//first line is over, respond to the server
            client.println("HTTP/1.0 200 OK");
            client.println();
            delay(1000); //give the message time to send
            break;
          }
          i++;
        }
        
      }
      //parse out name
      for (i=6; i<count; i++) { //always starts at index 6 and goes to the last space
        name[i-6] = recd[i];
      }
      if (recd[4] == 'o') { // o is for open
          authuser(name); //unlock
      } else if (recd[4] == 'a') { // a is for add
          addRFID(name); //add the rfid tag to the cardID file
      } else if (recd[4] == 'r') {
          remRFID(name); //remove rfid tag from file by name
      }
      client.stop();
      welcomescreen();
    }
  }
  if (readID()) {
    if (checkID(fulltag)){
      authuser(name);
    } else {
      notauthuser(fulltag);
    }
  } else {  //errors from readID()
      strcpy_P(buff1,msgs[12]);
      strcpy_P(buff2,msgs[7]);
      lcdshow(buff1,buff2);
      delay(1000);
  }
  welcomescreen();
  while(!digitalRead(TAG)){};
}  

void setcolor(char color) { //sets given color to LCD.
 if (color=='g') { 
   digitalWrite(greenpin, LOW);
   digitalWrite(bluepin, HIGH);
   digitalWrite(redpin, HIGH);
 }
 if (color=='p') { 
   digitalWrite(greenpin, HIGH);
   digitalWrite(bluepin, LOW);
   digitalWrite(redpin, LOW);
 }if (color=='y') { 
   digitalWrite(greenpin, LOW);
   digitalWrite(bluepin, HIGH);
   digitalWrite(redpin, LOW);
 }
 if (color=='b') {
   digitalWrite(bluepin, LOW);
   digitalWrite(greenpin, HIGH);
   digitalWrite(redpin, HIGH);
 }
 if (color=='r') {
   digitalWrite(redpin, LOW);
   digitalWrite(bluepin, HIGH);
   digitalWrite(greenpin, HIGH);
 }
 if (color=='o') {
   digitalWrite(bluepin, HIGH);
   digitalWrite(greenpin, HIGH);
   digitalWrite(redpin, HIGH);
 }
 if (color=='w') {
   digitalWrite(bluepin, LOW);
   digitalWrite(greenpin, LOW);
   digitalWrite(redpin, LOW);
 }
}

void ledOn(boolean on) //sets red LED on RFID reader
{
  // Send LED command to RFID reader
  Wire.beginTransmission(0x50);
  Wire.write(2);
  Wire.write(0x40);
  Wire.write(on);
  Wire.endTransmission();
}

void welcomescreen() { //Sets LCD to default screen
  lcd.clear();
  setcolor('b');
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
  lcd.print(firstline);
  lcd.setCursor(0,1);
  lcd.print(secondline);
}

void authuser(char name[]){ //allows the specified user into the house and prints given name onto LCD screen
  digitalWrite(relayPin, HIGH); //Unlocks door
  digitalWrite(backupPin, LOW);
  strcpy_P(buff1,msgs[4]);
  lcdshow(buff1,name);
  setcolor('g');
  delay(15000);
  digitalWrite(relayPin, LOW); //relocks door
  digitalWrite(backupPin, HIGH);
}

void notauthuser(char tag[]){ //denies user access to the house
  strcpy_P(buff1,msgs[5]);
  strcpy_P(buff2,msgs[6]);
  lcdshow(buff1,buff2);
  ledOn(true);
  delay(500);
  setcolor('o');
  delay(500);
  setcolor('r');
  delay(500);
  setcolor('o');
  delay(500);
  setcolor('r');
  delay(500);
  setcolor('o');
  delay(500);
  setcolor('r');
  delay(500);
  setcolor('o');
  delay(500);
  setcolor('r');
  delay(500);
  setcolor('o');
  delay(500);
  ledOn(false);
}  

boolean checkID(char id[]) {  //if allowed user returns name, return "FALSE" otherwise
  cardID = SD.open(cardIDpath,FILE_READ); //open the ID's file for reading
  authFlag = 0;
  i = 0;
  for (count=0; id[count]!='\0'; count++); //get the length of the id string (should always be 4, but just in case it ever changes)
  while(cardID.available()) { 
    c = cardID.read(); //read and compare byte by byte
    if (c == id[i] && i < count-1){ //if it matches, authflag and increase counter
      authFlag = 1;
      i++;
    } else if (i == count-1 && (char) cardID.peek() == '\t'){ //if counter reaches id length and the next one is a tab, you have a match! stop checking
      break;
    } else if (c != id[i]) { //if you find a non-match, reset the counter and authFlag
      authFlag = 0;
      i = 0;
    }
  }
  if (authFlag == 1) { //authorized
    memset(name,'\0',sizeof(name)); //reset name to null
    i=0;
    while(cardID.available()){ //pick up where you left off in the last while loop, i.e. right after the id string was found
      c = cardID.read(); 
      if (c=='\t') { //skip the tab
        continue;
      } else if (c == '\n' || c == '\r') { //you have the full name if you reach a line break
        cardID.close(); //close the file, prevents corruption
        return 1; //success!
      } else { //append the character to name, increment
        name[i] = c;
        i++;
      }
    }
  } else{
    cardID.close(); 
    return 0; //not authorized
  }
  cardID.close(); //should never get here, but just in case it does it returns a failure
  return 0;
}

boolean readID()
{
  memset(fulltag,'\0',sizeof(fulltag));
  // Send SELECT command
  Wire.beginTransmission(0x50);
  Wire.write(1);
  Wire.write(1);
  Wire.endTransmission();

  // Wait for response
  while(!digitalRead(TAG))//card present
  {
    // Allow some time to respond
    delay(5);
    
    // Anticipate maximum packet size
    Wire.requestFrom(0x50, 11);
    if(Wire.available())
    {
      // Get length of packet
      len = Wire.read();
      
      // Wait until whole packet is received
      while(Wire.available() < len)
      {
        // Quit if no response before tag left
        if(digitalRead(TAG)) {
          strcpy(fulltag,"nore"); //write error to fulltag (global)
          return 0; //fail
        }
      }

      // Read command code, should be same as request (1)
      command = Wire.read();
      if(command != 1) {
        strcpy(fulltag,"badc"); //same thing, error to global fulltag
        return 0; //fail
      }

      // Read status
      status = Wire.read();
      switch(status)
      {
        case 0: // Success, read ID and tag type
        {
          len -= 2;
          // Get tag ID
          i=0;
          while(--len)
          {
            data = Wire.read();
            fulltag[i] = data; //since fulltag is of type char*, it converts the byte to ascii-encoded text
            i++;
          }
          fulltag[i] = '\0'; //put a null terminator at the end just to be safe
          // Get tag type
        }
        return 1; //success!

      case 0x0A: // Collision
        strcpy(fulltag,"coll"); //err to fulltag

      case 1: // No tag
          strcpy(fulltag,"notf"); //err to fulltag

      default: // Unexpected
        strcpy(fulltag,"unre"); //err to fulltag
      }
      return 0; //fail if any of those errors
    }
  }
  // No tag found or no response
  strcpy(fulltag,"unav");
  return 0; //another fail
}

boolean addRFID(char n[]) { //return 0 if user already exists, 1 if successfully added
  memset(fulltag,'\0',sizeof(fulltag));
  authFlag = 0;
  for (count=0; n[count]!='\0'; count++); //count the length of name
  cardID = SD.open(cardIDpath, FILE_WRITE); //open IDs file for writing to the end (default)
  //check for pre-existing name
  cardID.seek(0); //move the cursor to the beginning
  i = 0;
  while(cardID.available()) {
    c = cardID.read();
    if (c == n[i] && i<count) { //increment on match
        i++;
    } else if (i == count && (char) cardID.peek() == '\n') {
        //end if i is the same length and the next char is a newline
        strcpy_P(buff1,msgs[8]);
        strcpy_P(buff2,msgs[9]);
        lcdshow(buff1,buff2);
        cardID.close();
        delay(5000);
        return 0;
    } else if (c != n[i]) { //reset on non-match
        i = 0;
    }
  }
  //get RFID tag from reader
  strcpy_P(buff1,msgs[13]);
  lcdshow(buff1,n);
  setcolor('p');
  delay(500);
  starttime = millis(); //start time for timeout
  i=0;
  while (digitalRead(TAG)){ //no card yet
    if (millis()-starttime > (unsigned long) 20000) { //check for timeout
      i=1; //flag for timeout
      break;
    }
  }
  if (i == 0) {authFlag = readID();} //returns 0 or 1 for whether or not it read
  if (authFlag) {
      cardID.seek(0); //go to the beginning of the file. this should make newer users be recognized faster
      cardID.print(fulltag); //print the tag 1st
      cardID.print('\t'); //then a tab
      cardID.print(n); //then the name
      cardID.println(); //then a newline, which is "\r\n"
      setcolor('g');
      strcpy_P(buff1,msgs[14]);
      lcdshow(n,buff1);
      backupID = SD.open(backupIDpath,FILE_READ); //cardID.print() overwrites current chars in the file, so re-write all of them from the backup
      while (backupID.available()){
        cardID.write((char) backupID.read());
      }
      backupID.close(); //close both to prevent corruption
      cardID.close();
      backup(); //create a new backup from the new cardID
      delay(3000);
      if(front.connect(back,80)){ //send the tag to the back door
        for (i=0; i<4; i++){
          front.print(fulltag[i]); //tag is the 1st 4 bytes
        }
        front.print("z ");
        for (i=0; i<17 ; i++){
          front.print(name[i]); //include the name, as usual
          if (name[i] == '\0') break;
        }
        front.println(' ');
        front.println();
        delay(5000); //wait to disconnect
        //if(!front.connected()) lcdshow("Not Connected",""); //debug
        //delay(5000); //debug
        front.flush(); 
        front.stop(); //close the connection
      } else { //notify of this failure
        setcolor('r');
        strcpy_P(buff1,msgs[15]);
        strcpy_P(buff2,msgs[16]);
        lcdshow(buff1,buff2);
        delay(3000);
      }
      return 1; //success!
  } else { //couldnt read card
      setcolor('y');
      lcdshow(fulltag,(char *) "");
      delay(5000);
      strcpy_P(buff1,msgs[17]);
      strcpy_P(buff2,msgs[12]);
      if (fulltag[0] == '\0'){
        strcpy_P(buff1,msgs[17]);
        strcpy_P(buff2,msgs[18]); 
      }
      lcdshow(buff1,buff2);
      delay(1000);
      cardID.close();
      return 0; //fail :(
  }
  cardID.close();
  return 0; //shouldn't get here, but just in case
}

boolean remRFID(char n[]) {
  i = 0; //iterator
  nCount = 0; // line counter
  authFlag = 0; //flag indicating found the user
  backupID = SD.open(backupIDpath,FILE_READ);
  //find name length
  for (count=0; n[count]!='\0'; count++);
  //Count line breaks until name found
  while(backupID.available()) {
    c = backupID.read();
    if (c == n[i] && i<count) { //increment on match
        i++;
        authFlag = 1;
    } else if (i == count && (char) backupID.peek() == '\n') { //end if i is the same length and the next char is a newline
        break;
    } else if (c != n[i]) { //reset on non-match
        i = 0; 
        authFlag = 0;
        if (c == '\n') {
          nCount++;
        }
    }
  }
  if (authFlag) {
      backupID.seek(0); //go to the beginning of the file
      if (SD.exists(cardIDpath)) SD.remove(cardIDpath); //delete old file
      cardID = SD.open(cardIDpath,FILE_WRITE); //create new one
      i = 0; //used to keep track of lines and skip the necessary line
      while (backupID.available()) {
        c = backupID.read();
        if (i != nCount) cardID.print(c); //don't write if on the relevant line
        if (c == '\n') i++; //increment i on newlines
      }
      cardID.close(); //close and flush
      backupID.close();
      setcolor('y');
      strcpy_P(buff1,msgs[11]); //write success to screen
      lcdshow(buff1,n);
      backup(); //create a new backup file
      delay(5000);
      return 1;
  } else {
      backupID.close(); //close backup
      setcolor('r');
      strcpy_P(buff1,msgs[10]); //write failure to screen
      lcdshow(buff1,n);
      delay(5000);
      return 0;
  }
  return 0; //shouldnt get here
}

void backup() {
  if (SD.exists(backupIDpath)) SD.remove(backupIDpath); //delete old file
  backupID = SD.open(backupIDpath,FILE_WRITE); //re-create
  cardID = SD.open(cardIDpath,FILE_READ); //open original
  while(cardID.available()) {
    backupID.write(cardID.read()); //write each byte from cardID to backupID
  }
  backupID.close();
  cardID.close();
}

boolean restore() { //created this to protect from a corrupted cardID.txt file, under the assumption that both are hardly ever open at the same time
  if (SD.exists(backupIDpath)) { //make sure the backup exists first
    if (SD.exists(cardIDpath)) SD.remove(cardIDpath); //remove cardID.txt if it already exists
    cardID = SD.open(cardIDpath,FILE_WRITE); //re-create and open cardID for writing
    backupID = SD.open(backupIDpath,FILE_READ); //open backup file
    while(backupID.available()){
      cardID.write(backupID.read()); //write each byte from backup to cardID
    }
    cardID.close();
    backupID.close();
    return 1; //success!
  } else {return 0;}  //fail if backup doesn't exist
}
