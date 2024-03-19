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
#include "env.h"

struct Operator
{
    float in;
    float *inptr;
    float out;

    float freqMolt;
    float phase;
    struct Envelope env;
    // float env_level;
    float amplCoeff;
};
void processOperator(struct Operator *operator);
