#ifndef MICROGRADER_PIN_H
#define MICROGRADER_PIN_H

#define REQ_BUFFER_SIZE 4
#define RESP_BUFFER_SIZE 4

class MicroGraderPin {
  public:

    void pinMode(uint8_t pin, uint8_t mode);

    int digitalRead(uint8_t pin);
    void digitalWrite(uint8_t pin, uint8_t val);

    void analogReadResolution(uint32_t bits);
    int analogRead(uint8_t pin);
    void analogWriteResolution(uint32_t bits);
    void analogWrite(uint8_t pin, int val);

  private:

    uint8_t req_buffer[REQ_BUFFER_SIZE];
    uint8_t resp_buffer[RESP_BUFFER_SIZE];

};
extern MicroGraderPin PinWrapper; // declaration of MicroGraderPin instance

#if TEST

#define pinMode PinWrapper.pinMode

#define digitalRead PinWrapper.digitalRead
#define digitalWrite PinWrapper.digitalWrite

#define analogReadResolution PinWrapper.analogReadResolution
#define analogReadRes PinWrapper.analogReadResolution
#define analogRead PinWrapper.analogRead
#define analogWriteResolution PinWrapper.analogWriteResolution
#define analogWriteRes PinWrapper.analogWriteResolution
#define analogWrite PinWrapper.analogWrite

#endif // TEST

#endif
