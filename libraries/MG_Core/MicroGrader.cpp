#include <WString.h>
#include <Arduino.h>
#include "MicroGrader.h"

MicroGraderCore MicroGrader; // definition of MicroGraderCore instance

//// PUBLIC FUNCTIONS

// Should be called at the very beginning of setup().
// Begins Serial connection and waits for remote program to begin reading.
void MicroGraderCore::begin() {
    Serial.begin(9600);
    while (!Serial.dtr()); // Wait for Serial connection
    sendMessage(MG_INIT, nullptr, 0); // Let grader know that connection is made
}

// No response expected (just ACK)
uint16_t MicroGraderCore::sendMessage(uint8_t code, uint8_t *msg, uint16_t msg_len) {
    return sendMessage(code, msg, msg_len, nullptr, 0);
}

// Sends the code, followed by a uint32_t timestamp in millis, followed by msg.
// Waits for, and processes, response.  If the response is a DATA response,
// that data is loaded into resp.
// If the response is ERR or there's a timeout, enter error state permanently.
// Returns: the # of bytes received in response (not including header)
//Later: allow for larger responses (up to 65536)
//Later: make timeout dynamic based on msg_len
uint16_t MicroGraderCore::sendMessage(uint8_t code, uint8_t *msg, uint16_t msg_len,
                                  uint8_t *resp, uint8_t resp_len) {
    // Later: perhaps helpful for robustness: empty Serial RX
    Serial.write(code);
    uint32_t timestamp = millis();
    Serial.write((uint8_t *)(&timestamp), sizeof(timestamp));
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
    } else if (response_type == ERR) {
        error(OTHER);
    }

    return expected_data_bytes;
}

//// PRIVATE FUNCTIONS

// Enter permanent error state with blinking LED.  Frequency of
//     blink corresponds to different causes of error.
// Later: more modes of failure (BAD_RESPONSE)
void MicroGraderCore::error(ErrorType error_type) {
    if (error_type == TIMEOUT) { // fast blink LED forever
        pinMode(LED_PIN, OUTPUT);
        while (true) {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
        }
    } else { // slow blink LED forever
        pinMode(LED_PIN, OUTPUT);
        while (true) {
            digitalWrite(LED_PIN, HIGH);
            delay(1000);
            digitalWrite(LED_PIN, LOW);
            delay(1000);
        }
    }
}
