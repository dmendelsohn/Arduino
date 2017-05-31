#include <Arduino.h>
#include "MicroGrader.h"
#include "MicroGraderPin.h"

MicroGraderPin PinWrapper; // definition of MicroGraderPin instance

// The functions below are substitutes for GPIO functions
void MicroGraderPin::pinMode(uint8_t pin, uint8_t mode) {
    req_buffer[0] = pin;
    req_buffer[1] = mode;
    MicroGrader.sendMessage(MG_PIN_MODE, req_buffer, 2);
}

int MicroGraderPin::digitalRead(uint8_t pin) {
    req_buffer[0] = pin;
    int received_bytes = MicroGrader.sendMessage(MG_DIGITAL_READ, req_buffer, 1,
                                                resp_buffer, RESP_BUFFER_SIZE);
    if (received_bytes < 1) {
        // Hang forever, something broke
        MicroGrader.error(BAD_RESPONSE);
    } 
    return resp_buffer[0];
}

void MicroGraderPin::digitalWrite(uint8_t pin, uint8_t val) {
    req_buffer[0] = pin;
    req_buffer[1] = val;
    MicroGrader.sendMessage(MG_DIGITAL_WRITE, req_buffer, 2);
}


void MicroGraderPin::analogReadResolution(uint32_t bits) {
    *((uint32_t *)req_buffer) = bits;
    MicroGrader.sendMessage(MG_READ_RES, req_buffer, sizeof(bits));
}

int MicroGraderPin::analogRead(uint8_t pin) {
    req_buffer[0] = pin;
    int received_bytes = MicroGrader.sendMessage(MG_ANALOG_READ, req_buffer, 1, 
                                                resp_buffer, RESP_BUFFER_SIZE);
    if (received_bytes < 4) {
        // Hang forever, something broke
        MicroGrader.error(BAD_RESPONSE);
    } 
    return *((int *)(resp_buffer)); // Return first four bytes as int
}

void MicroGraderPin::analogWriteResolution(uint32_t bits) {
    *((uint32_t *)req_buffer) = bits;
    MicroGrader.sendMessage(MG_WRITE_RES, req_buffer, sizeof(bits));
}

void MicroGraderPin::analogWrite(uint8_t pin, int val) {
    req_buffer[0] = pin;
    *((int *)(req_buffer + 1)) = val;
    MicroGrader.sendMessage(MG_ANALOG_WRITE, req_buffer, 1+sizeof(val));
}