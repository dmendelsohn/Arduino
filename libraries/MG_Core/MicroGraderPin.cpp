#include <Arduino.h>
#include "MicroGrader.h"
#include "MicroGraderPin.h"

MicroGraderPin PinWrapper; // definition of MicroGraderPin instance

MicroGraderPin::MicroGraderPin() {
    analog_read_res = ANALOG_READ_RES_DEFAULT;
    analog_write_res = ANALOG_WRITE_RES_DEFAULT;
}

// The functions below are substitutes for GPIO functions
void MicroGraderPin::pinMode_(uint8_t pin, uint8_t mode) {
    req_buffer[0] = pin;
    req_buffer[1] = mode;
    MicroGrader.sendMessage(MG_PIN_MODE, req_buffer, 2);
}

int MicroGraderPin::digitalRead_(uint8_t pin) {
    req_buffer[0] = pin;
    int data_bytes = MicroGrader.sendMessage(MG_DIGITAL_READ, req_buffer, 1,
                                             resp_buffer, RESP_BUFFER_SIZE);
    if (data_bytes < 1) {
        MicroGrader.error(DATA_ERROR); // Hang forever, something broke
    } 
    return resp_buffer[0];
}

void MicroGraderPin::digitalWrite_(uint8_t pin, uint8_t val) {
    digitalWrite(pin, val); // Do actual write
    req_buffer[0] = pin;
    req_buffer[1] = val;
    MicroGrader.sendMessage(MG_DIGITAL_WRITE, req_buffer, 2);
}


void MicroGraderPin::analogReadResolution_(uint32_t bits) {
    analog_read_res = bits;
}

int MicroGraderPin::analogRead_(uint8_t pin) {
    req_buffer[0] = pin;
    req_buffer[1] = (uint8_t)analog_write_res;
    int data_bytes = MicroGrader.sendMessage(MG_ANALOG_READ, req_buffer, 2, 
                                             resp_buffer, RESP_BUFFER_SIZE);
    if (data_bytes < 4) {
        MicroGrader.error(DATA_ERROR); // Hang forever, something broke
    } 
    return *((int *)(resp_buffer)); // Return first four bytes as int
}

void MicroGraderPin::analogWriteResolution_(uint32_t bits) {
    analogWriteResolution(bits); // Do action for real
    analog_write_res = bits;
}

void MicroGraderPin::analogWrite_(uint8_t pin, int val) {
    analogWrite(pin, val); // Do actual write
    req_buffer[0] = pin;
    req_buffer[1] = (uint8_t)analog_write_res;
    *((int *)(req_buffer + 1)) = val;
    MicroGrader.sendMessage(MG_ANALOG_WRITE, req_buffer, 2+sizeof(val));
}