#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP


constexpr float SAMPLE_RATE = 30e3;
constexpr float CHANNEL_CENTER = SAMPLE_RATE/4;
constexpr float CHANNEL_LENGTH = 10e3;
constexpr float CHANNEL_START = CHANNEL_CENTER - CHANNEL_LENGTH/2.0f;
constexpr float CHANNEL_END = CHANNEL_CENTER + CHANNEL_LENGTH/2.0f;
constexpr float RESAMPLE_RATE = (SAMPLE_RATE/2.0f) / (CHANNEL_LENGTH);
constexpr size_t FRAME_SIZE = 2048;
constexpr size_t BUFFER_SIZE = ((size_t)(FRAME_SIZE * RESAMPLE_RATE + 1));
constexpr size_t ORDER = 16;
constexpr size_t ITERATIONS = [](){
    size_t aux = ORDER;
    size_t exponent = 0;
    while(aux){
        exponent++;
        aux >>= 1;
    }
    return exponent - 1; //may underflow
}();

// constexpr float RESOLUTION = CHANNEL_LENGTH / (float)(ORDER);
// float symbolFrequency(size_t index){return index * RESOLUTION + RESOLUTION/2.0f + CHANNEL_START;}

#endif
