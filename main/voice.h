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
    uint8_t life_t;
    struct Operator op[N_OPERATORS];
};

void processVoice(struct Voice *voice);