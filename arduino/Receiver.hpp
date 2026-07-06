#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <Arduino.h>
#include <stddef.h>

#include "Demodulator.hpp"
#include "Microphone.hpp"
#include "Downsampler.hpp"
#include "Constants.hpp"
#include "WBASS.hpp"

#define STATES \
    STATE(INITIALIZE,"INITIALIZING")\
    STATE(PROBE,"SYMBOL DECISION THRESHOLD")\
    STATE(RECEIVE,"RECEIVING INCOMING MESSAGES...")

#define TRANSITIONS \
    TRANSITION(INITIALIZE,PROBE,"continue")\
    TRANSITION(PROBE,PROBE,"retry")\
    TRANSITION(PROBE,RECEIVE,"continue")\
    TRANSITION(RECEIVE,PROBE,"retry")\

unsigned long _T_LOOP_COUNTER;
#define TIGHT_LOOP(function,period) _T_LOOP_COUNTER = function(); while(function() - _T_LOOP_COUNTER < period)

#define AWAIT()\
    Serial.println("Press Y to continue");\
    while(true){\
        while(!Serial.available())delay(100);\
        char letter = Serial.read();\
        while(Serial.available()) Serial.read();\
        if(letter == 'Y') break;\
    }
    

class Receiver {
private:

    enum state_t {
#define STATE(name,text) name,
        STATES
#undef STATE
        _COUNT,
        _INVALID
    };

    struct transition_t {
        state_t source;
        state_t destination;
        const char* text;
    };

    using state_function_t = bool(*)();

#define STATE(name,text) inline static bool f_##name();
    STATES
#undef STATE

#define STATE(name,text) f_##name,
    static constexpr state_function_t FUNCTIONS[_COUNT] = {
        STATES
    };
#undef STATE

#define STATE(name,text) text,
    static constexpr const char* LABELS[_COUNT] = {
        STATES
    };
#undef STATE

#define TRANSITION(source,destination,text) \
        {source,destination,text},

    static constexpr transition_t SCREENS[] = {
        TRANSITIONS
    };

#undef TRANSITION
    
    //options
    static constexpr unsigned long PROBE_TIME = 10e3;
    static constexpr size_t FILTER_LENGTH = 10;
    
    //variables
    inline static state_t state = (state_t)0;
    inline static float decision_threshold;
    inline static Microphone<BUFFER_SIZE> * mic;
    inline static Demodulator<BUFFER_SIZE> * demod;
    inline static WBASS<FRAME_SIZE,complex_t> * wbass;
    inline static Downsampler<BUFFER_SIZE,FRAME_SIZE,complex_t> * downs;
    inline static Filter<FILTER_LENGTH,0,float,float,float> * filter;
    static constexpr size_t TRANSITION_COUNT = sizeof(SCREENS) / sizeof(SCREENS[0]);

public:

    static void run() {
        while(true) {

            Serial.println(LABELS[state]);

            size_t matches = 0;
            size_t options[TRANSITION_COUNT];

            for(size_t i = 0; i < TRANSITION_COUNT; i++) {
                const transition_t& transition = SCREENS[i];

                if(transition.source == state) {
                    options[matches] = i;
                    Serial.printf("\t-%u: %s\n", (unsigned)(matches), transition.text);
                    matches++;
                }
            }

            bool ready = false;

            while(true) {

                if(!ready)
                    ready = FUNCTIONS[state]();

                if(!Serial.available()) continue;
                int option = Serial.parseInt();
                while (Serial.available()) Serial.read();
                if(option < 0 || matches <= option) continue;
                size_t index = options[option];
                state = SCREENS[index].destination;
                break;
            }
        }
    }
};

inline bool Receiver::f_INITIALIZE(){
    mic = new Microphone<BUFFER_SIZE>(25,33,26);
    wbass = new WBASS<FRAME_SIZE,complex_t>();
    demod = new Demodulator<BUFFER_SIZE>();
    downs = new Downsampler<BUFFER_SIZE,FRAME_SIZE,complex_t>();

    float coeffs[FILTER_LENGTH];
    for(size_t i = 0; i < FILTER_LENGTH; i++)
        coeffs[i] = 1.0f / (float)FILTER_LENGTH;
    filter = new Filter<FILTER_LENGTH,0,float,float,float>(coeffs,nullptr);
    
    Serial.printf("Heap size     : %u\n", ESP.getHeapSize());
    Serial.printf("Free heap     : %u\n", ESP.getFreeHeap());
    Serial.printf("Min free heap : %u\n", ESP.getMinFreeHeap());
    return true;
}

inline bool Receiver::f_PROBE(){
    filter->clear();
    demod->clear();
    Serial.println("Input the symbol decision threshold: ");
    while(!Serial.available()) delay(100);
    decision_threshold = Serial.parseFloat();
    Serial.printf("Decision threshold for receiver hysteresis: %f\n", decision_threshold);
    return true;
}

inline bool Receiver::f_RECEIVE(){

    typedef enum{
        SETTING,
        RESETTING,
    }hysteresis_t;
    static hysteresis_t hysteresis = SETTING;

    const float * frame = mic->record();   
    const complex_t * demodulated = demod->execute(frame);
    const complex_t * dowsampled = downs->execute(demodulated);
    symbol_t symbol = wbass->execute(dowsampled);
    float energy = filter->execute(symbol.ratio);
    
    Serial.printf(">e:%f\n",energy);
    Serial.printf(">f:%f\n",symbol.frequency);
    Serial.printf(">r:%d\n",symbol.data);

    switch (hysteresis){
        case SETTING:{
            static bool done = false;
            static uint8_t data = 0;
            if(energy < decision_threshold) return false;
            Serial.printf(">s: %f\n",symbol.frequency);
            data |= symbol.data;
            if(done){
                Serial.printf("%c(",data);
                Serial.print(data,BIN);
                Serial.printf(")\n");
            };
            data <<= 4;
            done = !done;
            hysteresis = RESETTING;
        }break;

        case RESETTING:{
            if(decision_threshold <= energy) return false;
            hysteresis = SETTING;
        }break;
    }

    return false;
}

#endif