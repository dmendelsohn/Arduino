We designed a custom Teensy library for this class to interface with the ESP8266 chip.  The main issue with existing code snippets and libraries for this is that they all use blocking functions.  That is to say, while a request is out, or while the ESP8266 is trying to establish a network connection, the Teensy is doing nothing else.  This proved problematic for applications in which we want things to continue to happen during these lengthy periods of time. For example, the IOT step counter from Lab 2B worked really poorly with blocking wifi code, since many steps would get missed entirely.

This library's main advantage is that requests and network connections are done in a non-blocking fashion using interrupts, and consume a very low percentage of the Teensy's computing cycles.

##Summary of Public Methods

### ESP8266(bool verboseSerial)

* Initializes the ESP8266 class.

* Library will produce verbose outputs over USB serial if and only if `verboseSerial==true`.

* Argument is optional, and defaults to `true`.

## void begin()

* Checks for ESP8266 connection and resets the ESP8266.

* Call this before any other functions.

### void connectWifi(String ssid, String password)

* Attempts to connect to a network with the given SSID, using the given password.

* If the network is not password protected, password should be the empty string ("").

* Times out after 15 seconds.

### bool isConnected()

* Returns `true` if ESP8266 was connected to a network at the time of the last status check, and `false` otherwise.  Status checks occur every 10 seconds.


### void sendRequest(int type, String domain, int port, String path, String data, bool auto_retry)

* Sends an HTTP request directed at the given domain, port, and path.

* For the first argument, use "GET" and "POST" (without the quotes), which are macros defined in the library.

* Put GET parameters or POST data in the `data` input.  Do not attempt to put GET parameters in the `path` input.

* This function exits after the request is sent, and it does <strong>not</strong> wait for a response.

* The `auto_retry` flag is optional and defaults to `false`.  If `auto_retry` is `true`, the library will repeatedly attempt this request until an HTML response is received.

* Times out after roughly 15 seconds, though this will be shortened in later versions.

* If a request is already in progress, this function does nothing.

### void clearRequest()

* Clears the current request

* The main use case is to cease the repeated requests that occur when `sendRequest()` is called with `auto_retry==true`.

### bool hasResponse()

* Returns `true` if the library has received a valid HTML response since the last call to `sendRequest()`, and false otherwise.

* Note that the current version of the library only works with HTML responses.

### String getResponse()

* Returns the current HTML response as an Arduino `String`.

* You should check that `hasResponse()==true` before calling this.

* After calling this, the response is "cleared out" of the library.  That is to say, immediately after `getResponse()` is called, `hasResponse()` will return `false`.

### bool isBusy() 

* Returns `true` if there's is currently a request "in flight", and `false` otherwise.

### bool reset()

* Sends the following commands to the ESP8266: "AT+CWMODE_DEF=1", "AT+CWAUTOCONN=0", "AT+RST".

* Returns `true` if this operation was successful.

* Unlike request-sending or connecting to a network, this is a blocking operation.

### bool restore()

* Sends the command "AT+RESTORE" to the ESP8266, and then calls `reset()`. 

* Returns `true` if this operation was succesful.

* Unlike request-sending or connection to a network, this is a blocking operation.

### String getMAC() 

* Returns the MAC address of the ESP8266

### String sendCustomCommand(String command, unsigned long timeout)

* Sends the given command to the ESP8266 over serial.

* Blocks for `timeout` milliseconds, and returns any result that has come back from the ESP8266 during this time.

* Unlike request-sending or connection to a network, this is a blocking operation.

### bool isAutoConn()

* Returns `true` if the system is periodically checking for a network connection, and attempting reconnection if the connection is lost.  This behavior is enabled by default.

### void setAutoConn(bool value)

* Turns the "autoconnection" feature on if `value==true` and turns it off otherwise.

### int getTransmitCount()

* Returns the number of HTTP requests transmitted by the ESP8266 chip.

### void resetTransmitCount()

* Resets the transmit count to 0.

### int getReceiveCount()

* Returns the number of valid HTML messages received by the ESP8266 chip.

### void resetReceiveCount()

* Resets the receive count to 0.

##Troubleshooting
If you're trying to load a large webpage (>8kb) and you're seeing an error like "Warning: Input buffer full" in the Serial Monitor, try going into `ESP8266.h` and increasing the `BUFFERSIZE` and `RESPONSESIZE` constants.
