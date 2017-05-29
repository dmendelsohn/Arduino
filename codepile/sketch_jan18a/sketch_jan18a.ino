// Use ESP8266 to repeatedly send GET and POST commands to server and parse 
// responses.
//
// Written by Joe Steinmeyer, Joel Voldman
// 2015
// Uses some helper functions inspired by ESP8266 library 
// itead/ITEADLIB_Arduino_ESP8266

#include <SPI.h>
//#include <TinyGPS.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define wifiSerial Serial1
#define serialYes true

//const String SSID = "EECS-ConfRooms";
//const String PASSWD = "";
String SSID = "HOME-FD40-2.4";
String PASSWD = "3EHDDRAA7F33AC3N";

const int wifiControlPin = 2;
String wifis = "";
String MAC = "";
String get_response ="";
bool wifi_good = false;



void setup() 
{
  delay(4000);
  Wire.begin();
  //display setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(250);
  display.clearDisplay();
  display.display();

  //usb comm setup
  if (serialYes){
    Serial.begin(9600); //number meaningless on teensy (1MBit/s)
  }

  //wifi serial setup:
  wifiSerial.begin(115200);
  wifiSerial.setTimeout(4000);
  pinMode(wifiControlPin,OUTPUT);
  wifiEnable(true);
  
  if (check()){      //if ESP8266 is present
    resetWifi();    //reset
    MAC = getMAC(2000);
    emptyRx();
  }  
  
}


void loop() 
{
  if (wifi_good != true){   //if we're not connected to a network
    if (check()){       //if ESP8266 present
      listWifis();        
      if (serialYes){
        Serial.println(wifis);
      }
      bool good = connectWifi(SSID,PASSWD); //connect to network
      //bool good = connectWifi("MIT","");  //connect to network
      if (good){
        wifi_good = true;   // connected to a network
      }else{
        wifi_good=false;
      }
    }
  }
    

  String returnData = "";
  String steps = "";
  if (wifi_good){   //if we're connected to a network
  
    // web server parameters
    String domain = "iesc-s2.mit.edu";
    int port = 80;
    String path = "/lab02/weather.php";

    // send a GET command, will return weather info
    String send_comm1 = 
    "GET "+ path + " HTTP/1.1\r\n" +
    "Host: " + domain + "\r\n\r\n";
    get_response = httpComm(domain, port,path, send_comm1);
    Serial.println(get_response);

    // parse response
    // WRITE AND UPDATE THIS CODE
    unsigned int dateStart, dateEnd, locationStart, tempStart;
    dateStart = get_response.indexOf("~D~") + 3;
    locationStart = get_response.indexOf("~L~") + 3;
    tempStart = get_response.indexOf("~T~") + 3;
    String date = get_response.substring(dateStart,locationStart-14);
    String time = get_response.substring(locationStart-11,locationStart-3);
    String tempCString = get_response.substring(tempStart);
    float tempC = tempCString.toFloat();
    float tempF = tempC * 5/9 + 32;

    // update display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(2,0);
    display.print("Date:");
    display.println(date);
    display.print("Time: ");
    display.println(time);
    display.print("Temp: ");
    display.print(tempF);
    display.println(" deg F");
    display.display();
    delay(3000);
    }  
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
    emptyRx();
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
    emptyRx();
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
    Serial.print("Response: ");
    Serial.println(response);
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
  emptyRx();
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


// Send an http command
String httpComm(String domain, int port, String path, String comm) {  

  String response;  
  emptyRx();          // empty buffer
  if (setMux(0)) {    // set mux
    emptyRx();
    
    if (startComm(domain, port)) {  // set up tcp
      emptyRx();
      
      
      if (sendComm(comm, comm.length())) {  //send command
        response = receiveData(5000);       //receive response
      } else {
        Serial.println("Send failed");
      } 
    } else {
      Serial.println("Unable to start connection");
    }
  } else {
    Serial.println("MUX command failed");
  }
  wifiSerial.println("AT+CIPCLOSE");        //close tcp connection
  return response;
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


// Empty ESP8266 buffer
void emptyRx() {
    while(wifiSerial.available() > 0) {
        wifiSerial.read();
    }
}


// Read data from the wifi and test it for any of three target strings
bool readTest(String target1, String target2, String target3, uint32_t timeout) {

    String data_tmp;
    data_tmp = readString(target1, target2, target3, timeout);
    if (data_tmp.indexOf(target1) != -1) {
        return true;
    } else if (data_tmp.indexOf(target2) != -1) {
        return true;
    } else if (data_tmp.indexOf(target3) != -1) {
        return true;
    } else {
    return false;
    }
}

// Read data from the wifi and return that data once any of three target 
// strings are encountered
String readString(String target1, String target2, String target3, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(wifiSerial.available() > 0) {
            a = wifiSerial.read();
      if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        } else if (data.indexOf(target3) != -1) {
            break;
        }
    }
    return data;
}


// Set the multiplexing
bool setMux(int m) {
  
  String data;
  wifiSerial.print("AT+CIPMUX=");  
  wifiSerial.println(m);  
  data = readString("OK","ERROR","XX",5000);
   
  if (data.indexOf("OK") != -1) {
        return true;
  }
  return false;
}


// Set up TCP connection
bool startComm(String domain, int port) {

  String data;
  String start_comm = "AT+CIPSTART=\"TCP\",\"" + domain + "\","+String(port); //443 is HTTPS, still to do
//  String start_comm = "";     //UPDATE THIS LINE
  wifiSerial.println(start_comm);
  
  data = readString("OK", "ERROR", "ALREADY CONNECT", 10000);
    if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
        return true;
    }
  return false;
}


// Send a GET or POST command
bool sendComm(String buffer, int len) {
    wifiSerial.print("AT+CIPSEND=");    //send length command
    wifiSerial.println(len);
    if (readTest(">", "XX", "XX", 5000)) {    //if we get '>', send rest
        emptyRx();
        for (uint32_t i = 0; i < len; i++) {
            wifiSerial.write(buffer[i]);
        }
        return readTest("SEND OK", "XX", "XX", 10000);
    }
    return false;
}


// Read data from wifi and place into string
String receiveData(uint32_t timeout) {
  String response;
  unsigned long start = millis();
  while (millis() - start <timeout){
    if (wifiSerial.available()>0){
      char c = wifiSerial.read();
      Serial.print(c);
      response=String(response+c);
    }
  }
  return response;
}


String getMAC(uint32_t timeout) {
  String response;
  wifiSerial.println("AT+CIPSTAMAC?");    //send MAC query
  unsigned long start = millis();
  
  while (millis() - start <timeout){
    if (wifiSerial.available()>0){
      char c = wifiSerial.read();
      Serial.print(c);
      response=String(response+c);
    }
  }
  int stringStart = response.indexOf(":") + 2;
  return response.substring(stringStart,stringStart+17);
}
