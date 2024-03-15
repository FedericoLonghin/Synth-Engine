#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"

#include "esp_check.h"
#include "sdkconfig.h"

void i2s_write_task(void *args);
void i2s_init();

#define outBuff_size 441
extern int16_t outBuff_A[outBuff_size];
extern int16_t outBuff_B[outBuff_size];
extern bool fillBufferREQ;
extern bool bufferFilled;
extern int16_t *outBuffer_toFill;