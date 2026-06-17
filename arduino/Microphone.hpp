#ifndef MICROPHONE_HPP
#define MICROPHONE_HPP

#include <Arduino.h>
#include <driver/i2s.h>
#include "Constants.hpp"


template<size_t N>
class Microphone {
    private:
        int32_t raw[N];
        float frame[N];
    public:
        Microphone() = default;
        Microphone(uint8_t ws, uint8_t sd, uint8_t sck) {
            
            i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
                .sample_rate = (uint32_t)(SAMPLE_RATE),
                .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
                .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
                .communication_format = I2S_COMM_FORMAT_I2S,
                .intr_alloc_flags = 0,
                .dma_buf_count = 4,
                .dma_buf_len = 256,
                .use_apll = false,
                .tx_desc_auto_clear = false,
                .fixed_mclk = 0
              };
              

            i2s_pin_config_t pin_config = {
                .bck_io_num = sck,
                .ws_io_num = ws,
                .data_out_num = I2S_PIN_NO_CHANGE,
                .data_in_num = sd
            };

            esp_err_t err;
            err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
            Serial.printf("i2s_driver_install: %d\n", err);
            err = i2s_set_pin(I2S_NUM_0, &pin_config);
            Serial.printf("i2s_set_pin: %d\n", err);
            err = i2s_zero_dma_buffer(I2S_NUM_0);
            Serial.printf("i2s_zero_dma_buffer: %d\n", err);
        }

        const float * record() {
            size_t amount = 0;
            constexpr size_t bytes = N * sizeof(int32_t);
            
            while(amount < bytes){
                uint8_t* at = (uint8_t*)raw + amount;
                size_t remaining = bytes - amount;
                size_t got;
                i2s_read(I2S_NUM_0, at, remaining, &got, portMAX_DELAY);
                amount += got;
            }
            
            float energy = 0;
            for(size_t i = 0; i < N; i++){
                frame[i] = (raw[i] >> 8) / (float)(1 << 23);
                energy += frame[i] * frame[i];
            }
            
            energy = sqrt(energy/(float)N);
            for(size_t i = 0; i < N; i++)
                frame[i] /= energy;
        
            return frame;
        }
};

#endif