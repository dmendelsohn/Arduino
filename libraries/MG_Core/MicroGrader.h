// Author: Daniel Mendelsohn
// Written for MicroGrader thesis, Summer 2017
//
// This library provides functions for interacting with the x86 MicroGrader application via USB

#ifndef MICROGRADER_H
#define MICROGRADER_H

// Constants
#define MG_VERSION "0.1"
#define TIMEOUT_MICROS_DEFAULT 


// Byte codes for system events
#define MG_INIT 0x00

// Byte codes for GPIO events
#define MG_PIN_MODE 0x20
#define MG_DIGITAL_READ 0x21
#define MG_DIGITAL_WRITE 0x22
#define MG_ANALOG_READ 0x23
#define MG_ANALOG_WRITE 0x24
#define MG_READ_RES 0x25
#define MG_WRITE_RES 0x26

// Byte codes for IMU events
#define MG_IMU_ACC 0x30
#define MG_IMU_MAG 0x31
#define MG_IMU_GYRO 0x32
#define MG_IMU_TEMP 0x33

// Byte codes for OLED events
#define MG_OLED_BUFFER 0x40

// Byte codes for GPS events
#define MG_GPS_FIX 0x50
#define MG_GPS_READ 0x51

// Byte codes for Wifi events
#define MG_WIFI_CONN 0x60
#define MG_WIFI_REQ 0x61
#define MG_WIFI_RESP 0x62

// Byte codes for responses
#define MG_ACK 0x80
#define MG_ERR 0x081
#define MG_DATA 0x82


#include <WString.h>
#include <Arduino.h>
#include "Stream.h"

class MicroGrader { // Essentially a static class to wrap all communication
    public:
        void begin();
        bool sendMessage(uint8_t code, uint8_t *data, uint16_t data_len);
        bool sendMessage(uint8_t code, uint8_t *data, uint16_t data_len,
                                uint8_t *resp, uint8_t resp_len);

        // Pin functions
        /*void pinMode(uint8_t pin, uint8_t mode);

        int digitalRead(uint8_t pin);
        void digitalWrite(uint8_t pin, uint8_t val);

        void analogReadRes(uint32_t bits);
        void analogReadResolution(uint32_t bits);
        int analogRead(uint8_t pin);
        void analogWriteRes(uint32_t bits);
        void analogWriteResolution(uint32_t bits);
        void analogWrite(uint8_t pin, int val);*/

        //TODO: more
    private:
        enum ResponseType {NONE, ERR, ACK, DATA};
        enum ErrorType {TIMEOUT, OTHER};

        void error(ErrorType error_type);

};

// Dummy serial class; in TEST mode, uses of Serial will still compile, but
// will point to this dummy class instead.  We need exclusive control of
// USB serial for MicroGrader
class USBSerialDummy : public Stream
{
  public:
    void begin(long) { };
    void end() { };
    virtual int available() { return 0; }
    virtual int read() { return -1; }
    virtual int peek() { return -1; }
    virtual void flush() { }
    virtual size_t write(uint8_t c) { return 1; }
    virtual size_t write(const uint8_t *buffer, size_t size) { return size; }
    size_t write(unsigned long n) { return 1; }
    size_t write(long n) { return 1; }
    size_t write(unsigned int n) { return 1; }
    size_t write(int n) { return 1; }
    int availableForWrite() { return 0; }
    using Print::write;
    void send_now(void) { }
    uint32_t baud(void) { return 0; }
    uint8_t stopbits(void) { return 1; }
    uint8_t paritytype(void) { return 0; }
    uint8_t numbits(void) { return 8; }
    uint8_t dtr(void) { return 1; }
    uint8_t rts(void) { return 1; }
    operator bool() { return true; }
};
extern USBSerialDummy SerialDummy; // declaration of SerialDummy instance

#if TEST
    #define Serial SerialDummy  // Replace user Serial with SerialDummy
#else
    #define Serial Serial // Allow user to use regular Serial object
#endif

#endif // MICROGRADER_H