#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "config.h"

void command_reciver(void *args);

typedef struct{
uint8_t cmd;
uint8_t val_uint8;
uint8_t param1;
uint8_t param2;
uint8_t param3;
float val_float
}command;
extern QueueHandle_t cmd_queue_handle;