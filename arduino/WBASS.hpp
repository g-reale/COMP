#ifndef WBASS_HPP
#define WBASS_HPP

#include "Filter.hpp"
#include "Constants.hpp"
#include "filters/analytic.hpp"
#include "filters/lpf.hpp"
#include "filters/hpf.hpp"

typedef struct{
    float frequency = 0.0f;
    uint8_t data = 0;
    float ratio = 0.0f;
} symbol_t;

template<size_t N, typename sampletype>
class WBASS {
    private:
        size_t decimation;
        complex_t memory[3][N];
        Filter<LPF_FIR_SIZE,LPF_IIR_SIZE,float,complex_t, complex_t> lpf;
        Filter<HPF_FIR_SIZE,HPF_IIR_SIZE,float,complex_t, complex_t> hpf;
        
        float energy(const complex_t samples[N], size_t decimation){
            float energy = 0;
            for(size_t i = 0; i < N; i+= decimation)
                energy += samples[i].abs();
            return energy;
        }
        
        void shift(const complex_t samples[N], complex_t shifted[N], size_t decimation){
            constexpr complex_t frequency(0,-1.0f);
            complex_t phase(1,0);

            for(size_t i = 0; i < N; i+=decimation, phase *= frequency)
                shifted[i] = samples[i] * phase;
        }

    public:
        WBASS() : lpf(LPF_FIR_COEFFS,LPF_IIR_COEFFS), hpf(HPF_FIR_COEFFS,HPF_IIR_COEFFS){};
        
        symbol_t execute(const sampletype downsampled[N]){
            
            complex_t * whole = memory[0];
            complex_t * lower = memory[1];
            complex_t * upper = memory[2];

            symbol_t symbol;
            size_t decimation = 1;
            uint8_t low = 0;
            uint8_t up = (uint8_t)(ORDER);
            float lowf = CHANNEL_START;
            float upf = CHANNEL_END;
            float ub_energy;
            float lb_energy;

            memcpy(whole,downsampled,sizeof(sampletype) * N);
            
            for(size_t i = 0; i < ITERATIONS; i++, decimation <<= 1){
                
                lpf.execute<N>(whole,lower,decimation);
                hpf.execute<N>(whole,upper,decimation);
                lb_energy = energy(lower,decimation);
                ub_energy = energy(upper,decimation);

                if(lb_energy >= ub_energy){
                    complex_t * aux = whole;
                    whole = lower;
                    lower = aux;
                    up = (up+low) >> 1;
                    upf = (upf+lowf)/2.0f;
                }
                else{
                    shift(upper,whole,decimation);
                    low = (up+low) >> 1;
                    lowf = (upf+lowf)/2.0f;
                }
                
                hpf.clear();
                lpf.clear();
            }
            
            symbol.ratio = max(ub_energy,lb_energy);
            symbol.data = (low + up)>>1;
            symbol.frequency = (lowf + upf)/2.0f;
            return symbol;
        }
};

#endif