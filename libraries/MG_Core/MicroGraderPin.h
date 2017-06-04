#ifndef MICROGRADER_PIN_H
#define MICROGRADER_PIN_H

#define ANALOG_READ_RES_DEFAULT 10
#define ANALOG_WRITE_RES_DEFAULT 8

class MicroGraderPin {
  public:
    MicroGraderPin();

    void pinMode_(uint8_t pin, uint8_t mode);

    int digitalRead_(uint8_t pin);
    void digitalWrite_(uint8_t pin, uint8_t val);

    void analogReadResolution_(uint32_t bits);
    int analogRead_(uint8_t pin);
    void analogWriteResolution_(uint32_t bits);
    void analogWrite_(uint8_t pin, int val);

  private:

    uint8_t req_buffer[1+5*sizeof(int32_t)];
    uint8_t resp_buffer[sizeof(int32_t)];
    uint32_t analog_read_res;
    uint32_t analog_write_res;

    //TODO: add RW analog resolution variables

};
extern MicroGraderPin PinWrapper; // declaration of MicroGraderPin instance

#if TEST

#define pinMode PinWrapper.pinMode_

#define digitalRead PinWrapper.digitalRead_
#define digitalWrite PinWrapper.digitalWrite_

#define analogReadResolution PinWrapper.analogReadResolution_
#define analogReadRes PinWrapper.analogReadResolution_
#define analogRead PinWrapper.analogRead_
#define analogWriteResolution PinWrapper.analogWriteResolution_
#define analogWriteRes PinWrapper.analogWriteResolution_
#define analogWrite PinWrapper.analogWrite_

#endif // TEST

#endif
