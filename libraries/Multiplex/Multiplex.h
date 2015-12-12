#ifndef Multiplex_h
#define Multiplex_h

#include "Arduino.h"

//Define ranges for virtual IO pins
#define FIRST_PIN 0x40
#define LAST_PIN 0x7F

#define NULL_PIN 0xFF
#define PIN_LIMIT (LAST_PIN - FIRST_PIN + 1)

class Multiplex {
	public:
		Multiplex(uint8_t numPins, uint8_t data, uint8_t clock, uint8_t latch);
		void init(uint8_t numPins, uint8_t data, uint8_t clock, uint8_t latch);
		void begin();

		void _pinMode(uint8_t pin, uint8_t mode);
		void _digitalWrite(uint8_t pin, uint8_t value);
		void _digitalWrite(uint8_t pin, uint8_t value, boolean doLatch);
		void latch();

	private:
		uint8_t _dataPin;
		uint8_t _clockPin; // Data is shifted in on rising edge
		uint8_t _latchPin; // Data latches whenever latch pin is high

		uint8_t _numPins;
		uint8_t _dataOut[PIN_LIMIT];
		
		//Pin index is position in internal array
		//Pin number is user-facing reference number
		uint8_t pinIndexToNumber(uint8_t pinIndex);
		uint8_t pinNumberToIndex(uint8_t pinNumber);
};

#endif
