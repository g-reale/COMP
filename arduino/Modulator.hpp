#ifndef MODULATOR_HPP
#define MODULATOR_HPP

#include <Arduino.h>
#include "Constants.hpp"

class Modulator{ //square wave modulator
    private:
        uint8_t pin;
        unsigned long half_period;
        unsigned long timestamp;
        bool state;
    public:
        
        Modulator(uint8_t new_pin, float new_frequency) : pin(new_pin), state(false), timestamp(0) {
            pinMode(pin, OUTPUT);
            setFrequency(new_frequency);
        }
        
        void setFrequency(float new_frequency){
            half_period = 0.5e6f/new_frequency;
        }

        void modulate() {
            if (micros() - timestamp >= (unsigned long)half_period) {
                state = !state;
                digitalWrite(pin, state);
                timestamp = micros();
            }
        }
};

#endif