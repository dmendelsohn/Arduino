#define wifiSerial Serial1
#define serialYes true

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

//
// WIFI functions
//

void printWifiResponse(){
  while (wifiSerial.available()>0){
    char cByte = wifiSerial.read();
    Serial.write(cByte);
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
  } else if (serialYes) {
    Serial.println("ESP8266 not present");
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

  String cmd = "AT+CWJAP=?";
  wifiSerial.println(cmd);
  if (readTest("SEND OK", "XX", "XX", 10000)) return true;

  emptyRx();
  cmd = "AT+CWJAP=\"" + ssid + "\",\"" + password + "\"";
  wifiSerial.println(cmd);
  unsigned long start = millis();
  String response="";
  
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
