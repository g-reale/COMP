#ifndef DEMODULATOR_HPP
#define DEMODULATOR_HPP

#include <Arduino.h>
#include "Constants.hpp"
#include "filters/complex.hpp"

template<size_t N>
class Demodulator{
    private:
        complex_t frequency;
        complex_t shifted[N];

    public:
        Demodulator() {
            float frequency_rad = 2.0f * M_PI * ((float)CHANNEL_START / (float)SAMPLE_RATE);
            frequency = complex_t(cosf(frequency_rad), -sinf(frequency_rad));
        }

        const complex_t * execute(const float original[N]){
            complex_t phase = 1.0f;
            for(size_t i = 0; i < N; i++, phase *= frequency)
                shifted[i] = original[i] * phase;
            return shifted;
        }
};

#endif