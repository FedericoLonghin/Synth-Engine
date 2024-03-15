// general
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sdkconfig.h"

#include "i2s_manager.h"

#include "voice.h"
void audio_task();

void app_main(void)
{
  const char *TAG = "app_main";
  ESP_LOGI(TAG, "Started");

  printf("Synth Engine V-1.0\n");

  i2s_init();
  xTaskCreate(audio_task, "audioTask", 10000, NULL, 2, NULL);
  while (1)
  {
    vTaskDelay(1);
  }
  return;
}

void audio_task()
{
  const char *TAG = "audio_Task";
  struct Voice *v = malloc(1);
  while (1)
  {

    if (fillBufferREQ)
    {
      for (int i = 0; i < outBuff_size; i++)
      {
        processVoice(v);
      }

      ESP_LOGE(TAG, "buffer filled");
      bufferFilled = true;
    }
    vTaskDelay(1);
  }
  vTaskDelete(NULL);
}