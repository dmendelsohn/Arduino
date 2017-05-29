#include <Adafruit_GPS.h>

// You should make the following connections with the Teensy and GPS module:
// GPS power pin to Teensy 3.3V output.
// GPS ground pin to Teensy ground.
// GPS TX to Teensy RX1
// GPS RX to Teensy TX1
#define mySerial Serial1

Adafruit_GPS GPS(&mySerial);

// No interrupts for Teensy use
boolean usingInterrupt = false;
void useInterrupt(boolean);

void setup()
{
  Serial.begin(115200);
  Serial.println("GPS Library Initialized");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  mySerial.begin(9600);

  // RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  usingInterrupt = false;  //NOTE - we don't want to use interrupts on the Teensy

  delay(1000);
  // Ask for firmware version
  //mySerial.println(PMTK_Q_RELEASE);

  /*
   * Uncomment a line below
   */

  // Toggles logger on
  //GPS.sendCommand(PMTK_LOCUS_STARTLOG);
  // Toggles logger off
  //GPS.sendCommand(PMTK_LOCUS_STOPLOG);
}

uint32_t timer = millis();
void loop()
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  if (c) Serial.print(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();
}
