#include <WString.h>
#include <Arduino.h>
#include "MicroGrader.h"

MicroGraderCore MicroGrader; // definition of MicroGraderCore instance
const uint32_t MicroGraderCore::HI_MILLIS[] = {100, 500, 2000};
const uint32_t MicroGraderCore::LO_MILLIS[] = {100, 500, 2000};


//// PUBLIC FUNCTIONS

// Should be called at the very beginning of setup().
// Begins Serial connection and waits for remote program to begin reading.
void MicroGraderCore::begin() {
    Serial.begin(9600);
    while (!Serial.dtr()); // Wait for Serial connection
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // So we can visually see when connection has been made
    delay(10); // Small delay is necessary, not sure why
    sendMessage(MG_INIT, nullptr, 0); // Let grader know that connection is made
}

// No response expected (just ACK)
uint16_t MicroGraderCore::sendMessage(code_t code, uint8_t *msg, msg_size_t msg_len) {
    return sendMessage(code, msg, msg_len, nullptr, 0);
}

// Sends the message header followed by message body.
// Waits for, and processes, response.  Loads the response body into resp
// If the response is ERR or there's a timeout, enter error state permanently.
// Returns: the number of bytes received in response body
uint16_t MicroGraderCore::sendMessage(code_t code, uint8_t *msg, msg_size_t msg_len,
                                      uint8_t *resp, msg_size_t resp_len) {
    // Later: perhaps helpful for robustness: empty Serial RX

    // First, send the header (code, timestamp, body_size)
    timestamp_t timestamp = millis();
    Serial.write((uint8_t *)(&code), sizeof(code));
    Serial.write((uint8_t *)(&timestamp), sizeof(timestamp));
    Serial.write((uint8_t *)(&msg_len), sizeof(msg_len));

    // Next, send the body
    Serial.write(msg, msg_len);
    Serial.send_now();

    // Wait for and process response
    elapsedMicros t = 0;
    bool has_response = false;
    msg_size_t received_bytes = 0; // includes header
    msg_size_t expected_body_bytes = 0;
    code_t resp_code = MG_ERR; // Assume error at first
    uint32_t timeout = 10000; // Later: make this dynamic based on msg_len

    while (!has_response && t < timeout) {
        if (Serial.available()) { // Check if a character is available
            uint8_t val = Serial.read();
            if (received_bytes < RESP_HEADER_SIZE) {
                header_buffer[received_bytes] = val;
            } else {
                msg_size_t index = received_bytes - RESP_HEADER_SIZE;
                if (index < resp_len) { // Check for overflow
                    resp[index] = val; // Insert new value into response buffer
                }
                // For now, silently drop characters beyond allowed resp_len
            }
            received_bytes++;

            // If header is completely received, process it
            if (received_bytes == RESP_HEADER_SIZE) {
                uint8_t *ptr = header_buffer; // Get pointer to beginning of array
                resp_code = *((code_t *)ptr);
                ptr += sizeof(code_t); // Increment to next part of header
                expected_body_bytes = *((msg_size_t *)ptr);
            }

            // Check if message is done
            if (received_bytes == expected_body_bytes+RESP_HEADER_SIZE) {
                    has_response = true;
            }
        }
    }

    if (t > timeout) {
        error(TIMEOUT_ERROR);
    } else if (resp_code != MG_ACK) {
        error(RESP_ERROR);
    }

    return expected_body_bytes;
}

// Enter permanent error state with blinking LED.  Frequency of
//     blink corresponds to different causes of error.
void MicroGraderCore::error(MG_ErrorType error_type) {
    pinMode(LED_PIN, OUTPUT);
    while (true) {
        digitalWrite(LED_PIN, HIGH);
        delay(HI_MILLIS[error_type]);
        digitalWrite(LED_PIN, LOW);
        delay(LO_MILLIS[error_type]);
    }       
}

//Send a debug statement to the host
void MicroGraderCore::debug(String str) {
    sendMessage(MG_PRINT, (uint8_t *)str.c_str(), str.length());
}
