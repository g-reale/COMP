#ifndef FILTER_HPP
#define FILTER_HPP

#include <string.h>

template<size_t N, typename datatype>
class Circular {
    private:
        datatype memory[N];
        size_t index;
    public:
        Circular(){clear();};

        void push(datatype sample) {
            memory[index] = sample;
            index = (index + 1) % N;
        }

        inline datatype operator[](size_t i) const {
            return memory[(N + index - i - 1) % N];
        }

        void clear() {
            memset(memory, 0, sizeof(datatype) * N);
            index = 0;
        }
};

template<size_t N, size_t M, typename filtertype, typename sampletype, typename resulttype>
class Filter {
    private:
        filtertype fir[N];
        filtertype iir[M];
        Circular<N, sampletype> firmem;
        Circular<M, resulttype> iirmem;
    public:
        Filter() = default;

        Filter(const filtertype new_fir[N], const filtertype new_iir[M]) {
            if constexpr (N)
                memcpy(fir, new_fir, sizeof(filtertype) * N);
            if constexpr (M)
                memcpy(iir, new_iir, sizeof(filtertype) * M);
        }

        resulttype execute(sampletype sample) {
            
            resulttype result = 0.0f;

            if constexpr (N){
                firmem.push(sample);
                for(size_t i = 0; i < N; i++)
                    result += fir[i] * firmem[i];
            }
            
            if constexpr (M){
                for(size_t i = 0; i < M; i++)
                    result -= iir[i] * iirmem[i];
                iirmem.push(result);
            }
            
            return result;
        }

        template<size_t K>
        void execute(const sampletype samples[K], resulttype result[K], size_t decimation = 1){
            for(size_t i = 0; i < K; i+=decimation)
                result[i] = execute(samples[i]);
        }

        void clear() {
            firmem.clear();
            iirmem.clear();
        }
};

#endif