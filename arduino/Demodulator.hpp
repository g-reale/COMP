#ifndef DEMODULATOR_HPP
#define DEMODULATOR_HPP

#include <Arduino.h>
#include "Constants.hpp"
#include "Filter.hpp"
#include "filters/complex.hpp"
#include "filters/analytic.hpp"

template<size_t N>
class Demodulator{
    private:
        complex_t frequency;
        complex_t shifted[N];
        Filter<ANALYTIC_FIR_SIZE,ANALYTIC_IIR_SIZE,complex_t,complex_t,complex_t> analytic;
    public:
        Demodulator():analytic(ANALYTIC_FIR_COEFFS,ANALYTIC_IIR_COEFFS){
            float frequency_rad = 2.0f * M_PI * ((float)CHANNEL_START / (float)SAMPLE_RATE);
            frequency = complex_t(cosf(frequency_rad), -sinf(frequency_rad));
        }

        const complex_t * execute(const float original[N]){
            complex_t phase = 1.0f;
            for(size_t i = 0; i < N; i++, phase *= frequency){
                complex_t filtered = analytic.execute(original[i] * phase);
                shifted[i] = filtered;
            }
            return shifted;
        }

        void clear(){
            analytic.clear();
        }
};

#endif