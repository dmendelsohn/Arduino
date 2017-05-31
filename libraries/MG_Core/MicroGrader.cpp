#include <WString.h>
#include <Arduino.h>
#include "MicroGrader.h"

USBSerialDummy SerialDummy; // definition of dummy Serial instance

//// INITIALIZE STATIC VARIABLES


//// PUBLIC FUNCTIONS

//TODO: Comment
void MicroGrader::begin() {
    Serial.begin(9600);
    //TODO: wait for Serial connection
    sendMessage(MG_INIT, nullptr, 0);
}

//TODO: Comment
bool MicroGrader::sendMessage(uint8_t code, uint8_t *msg, uint16_t msg_len) {
    return sendMessage(code, msg, msg_len, nullptr, 0);
}

//TODO: Comment
//Later: allow for larger responses (up to 65536)
//Later: make timeout dynamic based on msg_len
bool MicroGrader::sendMessage(uint8_t code, uint8_t *msg, uint16_t msg_len,
                              uint8_t *resp, uint8_t resp_len) {
    // Later: perhaps helpful for robustness: empty Serial RX
    Serial.write(code);
    uint32_t timestamp = millis();
    //TODO: send timestamp as 4 bytes
    Serial.write(msg, msg_len);
    Serial.send_now();
    elapsedMicros t = 0;
    bool has_response = false;
    uint16_t received_bytes = 0; // num characters received in response
    uint8_t expected_data_bytes = 0; // num data chars expected in response
    ResponseType response_type = ERR; // No response received yet
    uint32_t timeout = 10000;
    while (!has_response && t < timeout) {
        if (Serial.available()) { // Check if a character is available
            uint8_t val = Serial.read();
            if (received_bytes == 0) { // Process first char, "code"
                if (val == MG_ACK) {
                    response_type = ACK;
                    has_response = true;
                } else if (val == MG_DATA) {
                    response_type = DATA;
                } else {
                    response_type = ERR;
                    has_response = true;
                }
            } else if (received_bytes == 1) { // Process second char, "size"
                expected_data_bytes = val;
            } else { // Process array of data
                uint8_t index = received_bytes - 2;
                if (index < resp_len) { // Check for overflow
                    resp[index] = val; // Insert new value into response buffer
                }
                // For now, silently drop characters beyond allowed resp_len
            }
            received_bytes++;
            if (received_bytes == expected_data_bytes+2) {  // End of expected message
                    has_response = true;
            }
        }
    }
    if (t > timeout) {
        error(TIMEOUT);
        return false;
    } else if (response_type == ERR) {
        error(OTHER);
        return false;
    } else { // Success
        return true;
    }
}

//// PRIVATE FUNCTIONS

//TODO: comment
void MicroGrader::error(ErrorType error_type) {
    if (error_type == TIMEOUT) {
        // TODO: fast blink LED forever
    } else {
        // TODO: slow blink LED forever
    }
}


