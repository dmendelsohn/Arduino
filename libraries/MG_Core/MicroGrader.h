// Author: Daniel Mendelsohn
// Written for MicroGrader thesis, Summer 2017
//
// This library provides functions for interacting with the x86 MicroGrader application via USB

#ifndef MICROGRADER_H
#define MICROGRADER_H

// Constants
#define MG_VERSION "0.1"
#define LED_PIN 13

// Byte codes for system events
#define MG_INIT 0x00
#define MG_PRINT 0x01

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
#define MG_OLED_INIT 0x40
#define MG_OLED_FULL 0x41

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


#include <WString.h>
#include <Arduino.h>
#include "USBSerialDummy.h"
#include "MicroGraderPin.h"

typedef uint8_t code_t;
typedef uint32_t timestamp_t;
typedef uint16_t msg_size_t;

#define REQ_HEADER_SIZE (sizeof(code_t)+sizeof(timestamp_t)+sizeof(msg_size_t))
#define RESP_HEADER_SIZE (sizeof(code_t)+sizeof(msg_size_t))

enum MG_ErrorType {TIMEOUT_ERROR=0, RESP_ERROR=1, DATA_ERROR=2};

class MicroGraderCore { // Essentially a static class to wrap all communication
  public:
    void begin();
    uint16_t sendMessage(code_t code, uint8_t *data, msg_size_t data_len);
    uint16_t sendMessage(code_t code, uint8_t *data, msg_size_t data_len,
                         uint8_t *resp, msg_size_t resp_len);
    void error(MG_ErrorType error_type);
    void debug(String str);

  private:
    uint8_t header_buffer[RESP_HEADER_SIZE];

    static uint32_t const HI_MILLIS[]; // blink times for error processing
    static uint32_t const LO_MILLIS[]; // blink times for error processing

};
extern MicroGraderCore MicroGrader; // declaration of MicroGraderCore instance


#if TEST
    #define Serial SerialDummy  // Replace user Serial with SerialDummy
#endif

#endif // MICROGRADER_H