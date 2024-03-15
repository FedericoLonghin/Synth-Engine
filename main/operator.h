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
    float out;

    struct Envelope env;
    float env_level;
};
void processOperator(struct Operator *operator);
