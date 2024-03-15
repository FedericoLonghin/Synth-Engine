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

struct command{
uint8_t cmd;
uint8_t val;
};
extern QueueHandle_t cmd_queue_handle;