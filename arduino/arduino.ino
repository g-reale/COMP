// #include "Constants.hpp"
// #include "Microphone.hpp"
// #include "WBASS.hpp"
// #include "Modulator.hpp"
#include "Receiver.hpp"


// WBASS<FRAME_SIZE> * wbass;
// float frame[FRAME_SIZE];

// float getSymbolFrequency(size_t index){
//     index = index * 16 + 8;
//     return RESOLUTION * index + RESOLUTION / 2.0f;
// }

// float fm;

// typedef enum state_t{
//     HELLO,
// };

// state_t state;


void setup() {
    Serial.begin(115200);
    
    // wbass = new WBASS<FRAME_SIZE>();
    
    // float frequency = symbolFrequency(2);
    // fm = frequency;
    // float dc = 2.0f;

    // cosine test signal
    // for(size_t i = 0; i < FRAME_SIZE; i++)
        // frame[i] = cosf(2.0f * PI * frequency * i / SAMPLE_RATE) + dc;

    // square wave test signal
    // for(size_t i = 0; i < FRAME_SIZE; i++)
    //     frame[i] = (fmodf((float)i, SAMPLE_RATE / frequency) < (SAMPLE_RATE / frequency / 2.0f) ? 1.0f : -1.0f) + dc;

    Serial.println("setup end");
}

void loop(){
    // const float * frame = mic->record();
    // for(size_t i = 0; i < FRAME_SIZE; i++)
    //     Serial.printf(">s:%f\n",frame[i]);
    Receiver::run();
}