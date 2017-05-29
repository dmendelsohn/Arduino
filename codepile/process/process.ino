#include <Process.h>

void setup() {
  // initialize Bridge
  Bridge.begin();
  // initialize Serial
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  
  // Wait until a Serial Monitor is connected.
  while (!Serial);
  
  Serial.println("Serial is connected!!");
  
  //run curl
  runCurl();
  runCpuInfo();
}

void loop() {
  delay(250);
  digitalWrite(13, HIGH);
  delay(250);
  digitalWrite(13, LOW);
}

void runCurl() {
  // Launch "curl" command and get Arduino ascii art logo from network
  // curl is command line program for transferring data using different internet protocols
  Process p;    // Create a process, p
  p.begin("curl");  //Process that launch the "curl" command
  p.addParameter("http://arduino.cc/asciilogo.txt");  //Add the URL parameter to "curl"
  p.run();    // Run the process and wait for termination
  
  Serial.println("Ran curl");
  
  // Print arduino logo over the Serial
  // A process output can be read with the stream methods
  while (p.available()>0) {
    Serial.println("Yay looping");
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent
  Serial.flush();
}

void runCpuInfo() {
  // Launch "cat /proc/cpuinfo" command (shows info on Atheros CPU)
  Process p;
  p.begin("cat");
  p.addParameter("/proc/cpuinfo");
  p.run();
  
  while (p.available()>0) {
    char c =  p.read();
    Serial.print(c);
  }
  Serial.flush();
}
