// general
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "sdkconfig.h"
#include "esp_task_wdt.h"

#include "i2s_manager.h"

#include "voice.h"
void audio_task();

void app_main(void)
{
  const char *TAG = "app_main";
  ESP_LOGI(TAG, "Started");

  printf("Synth Engine V-1.0\n");

  gpio_set_direction(5, GPIO_MODE_OUTPUT);
  gpio_set_level(5, 0);

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
  float onePtr = 1;
  const char *TAG = "audio_Task";
  struct Voice *v1 = malloc(1);
  v1->freq = 440;
  v1->phase = 0;
  v1->op[0].freqMolt = 1;
  v1->op[1].freqMolt = 2;
  v1->op[0].inptr = &onePtr;
  v1->op[1].inptr = &v1->op[0].out;

  while (1)
  {
    if (fillBufferREQ)
    {
      gpio_set_level(5, 1);
      for (int i = 0; i < outBuff_size; i++)
      {
        processVoice(v1);
        outBuffer_toFill[i] = (v1->out) * 0xFFF;
        // printf("%d\n", outBuffer_toFill[i]);
      }
      // ESP_LOGE(TAG, "buffer filled");
      bufferFilled = true;

      fillBufferREQ = false;
    }
    else
    {
      gpio_set_level(5, 0);
      // vTaskDelay(1);
    }
  }
  vTaskDelete(NULL);
}