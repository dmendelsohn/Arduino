#include "Multiplex.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

Multiplex::Multiplex(uint8_t numPins, uint8_t data, uint8_t clock, uint8_t latch) {
	init(numPins, data, clock, latch);
}

void Multiplex::init(uint8_t numPins, uint8_t data, uint8_t clock, uint8_t latch) {
	_dataPin = data;
	_clockPin = clock;
	_latchPin = latch;

	_numPins = numPins;
	for (int i = 0; i < PIN_LIMIT; i++) {
		_dataOut[i] = LOW;
	}

	begin();
}

void Multiplex::begin() {
	//Serial.println("Called begin");
	pinMode(_dataPin, OUTPUT);
	pinMode(_clockPin, OUTPUT);
	pinMode(_latchPin, OUTPUT);
	latch(); //Latch initial data
}

void Multiplex::_pinMode(uint8_t pin, uint8_t mode) {
	if (pin < FIRST_PIN) { // regular IO pin
		pinMode(pin, mode);
	} else {
		// virtual pins modes cannot change, no need to do anything
	}
}

void Multiplex::_digitalWrite(uint8_t pin, uint8_t value) {
	_digitalWrite(pin, value, true); //Latch by default
}

void Multiplex::_digitalWrite(uint8_t pin, uint8_t value, boolean doLatch) {
	if (pin >= FIRST_PIN && pin <= LAST_PIN) {
		uint8_t pinIndex = pinNumberToIndex(pin);
		//Serial.print("Writing to pin index: ");
		//Serial.println(pinIndex);
		_dataOut[pinIndex] = value;
		if (doLatch) {
			latch();
		}
	} else {
		digitalWrite(pin, value); //Not multiplex pin, pass along command
	}
}

void Multiplex::latch() {
	//Serial.print("Latching: [");
	for (int i = _numPins-1; i >= 0; i--) {
		//Serial.print(_dataOut[i]);
		//Serial.print(", ");
		digitalWrite(_dataPin, _dataOut[i]);
		digitalWrite(_clockPin, HIGH);
		//delay(10); //Why did I even do this?
		digitalWrite(_clockPin, LOW);
	}
	//Serial.println("]");
	digitalWrite(_latchPin, HIGH);
	//delay(160); //Why did I even do this?
	digitalWrite(_latchPin, LOW);
}

uint8_t Multiplex::pinIndexToNumber(uint8_t pinIndex) {
	return pinIndex + FIRST_PIN;
}

uint8_t Multiplex::pinNumberToIndex(uint8_t pinNumber) {
	return pinNumber - FIRST_PIN;
}
