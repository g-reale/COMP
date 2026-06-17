#ifndef DOWNSAMPLER_HPP
#define DOWNSAMPLER_HPP

#include <math.h>
#include "filters/complex.hpp"

template<size_t N, size_t M, typename sampletype>
class Downsampler{
    private:
        sampletype downsampled[M];
        float step;
    public:

        Downsampler(){
            step = N / (float)M;
        }

        sampletype * execute(const sampletype upsampled[N]){
            float time = 0.5f;
            size_t i = 0;

            while(i < M){
                size_t lower = (size_t)floorf(time);
                size_t upper = (size_t)ceilf(time);
                if(upper >= N) break;
                float decimal = time - lower;
                downsampled[i] = upsampled[lower] * (1.0f - decimal) + upsampled[upper] * decimal;
                i++;
                time += step;
            }
            return downsampled;
        }
};

#endif