#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sdkconfig.h"

#include "config.h"
#include "operator.h"
struct Voice
{
    float out;
    float phase;
    float freq;
    uint32_t life_t;
    struct Operator op[N_OPERATORS];
};

void processVoice(struct Voice *voice);
void noteOn(struct Voice *voice, uint8_t note);
void noteOff(struct Voice *voice);
float getEnvelope_ampl(struct Envelope *env, uint32_t time);
