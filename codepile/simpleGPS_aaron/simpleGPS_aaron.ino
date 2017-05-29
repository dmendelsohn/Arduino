#include <SPI.h>
#include <Adafruit_GPS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define GPSSerial Serial2

Adafruit_SSD1306 display(4);
Adafruit_GPS GPS(&GPSSerial);

String data = "";


class GPSParser
{
  public:
  String hrs, min, sec;
  String valid;
  String lat, NS, lon, EW;
  String day, month, year;


  GPSParser(){
    hrs = "";
    min = "";
    sec = "";
    valid = "";   //Should contain "A" or "V"
    lat = "";
    NS = "";
    lon = "";
    EW = "";
    day = "";
    month = "";
    year = "";
  }

  //Finds commas in "s" and fills in pre-initialized array commaLoc with 
  //the indices of those commas, up to a total of "maxCommas" commas.
  //If there are fewer that "maxCommas" commas, this function pads the
  //remainder of commaLoc (up to index "maxCommas") with the value -1
  void findCommas(String s, int commaLoc[], int maxCommas) {
    commaLoc[0] = s.indexOf(",");
    for (int i=1; i<maxCommas; i++) {
      commaLoc[i] = s.indexOf(",", commaLoc[i-1]+1);
    }
  }

void extractData(String d) {
      int commaLoc[11];
      findCommas(data,commaLoc,11);
      int i = commaLoc[0];
      hrs = d.substring(i+1,i+3);
      min = d.substring(i+3, i+5);
      sec = d.substring(i+5, i+7);
      valid = d.charAt(commaLoc[1]+1);
      i = commaLoc[2];
      if(valid == 'A'){
        lat = d.substring(i+1, commaLoc[3]);
        NS = d.charAt(commaLoc[3]+1);
        lon = d.substring(commaLoc[4]+1,commaLoc[5]);
        EW = d.charAt(commaLoc[6]+1);
      }
      i = commaLoc[8];
      day = d.substring(i+1, i+3);
      month = d.substring(i+3, i+5);
      year = d.substring(i+5, i+7);
  }
};


//Create instance of GPSParser class
GPSParser gps_data;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(250);
  display.clearDisplay();
  display.setTextColor(WHITE);


  Serial.begin(115200);
  

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's
  GPS.begin(9600);
  GPSSerial.begin(9600);

  // RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  

  // Set the update rate to 1 Hz
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  
 
  delay(1000);

  Serial.println("-----------------------------------------------------------");
}

void loop()
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  if(c){
    if(c == '$'){
      Serial.println();
      if(data.indexOf("GPRMC")!=-1){ 
        gps_data.extractData(data);
        updateDisplay();
      }
      data = "";
    }
    data += c;
    Serial.print(c);
  }
}
  


void updateDisplay() {

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);

  if (gps_data.valid=="A") {
    display.println("Date: " + gps_data.month + "/" + gps_data.day + "/" + gps_data.year);
    display.println("Time: " + gps_data.hrs + ":" + gps_data.min + ":" + gps_data.sec);
    display.println("Lat: " + gps_data.lat.substring(0,2) + "' " + gps_data.lat.substring(2) + " " + gps_data.NS);
    display.println("Lon: " + gps_data.lon.substring(0,3) + "' " + gps_data.lon.substring(3) + " " + gps_data.EW);
  
  }
  else if (gps_data.valid=="V") {
    display.println(" No valid fix");
  }
  display.display();
}
