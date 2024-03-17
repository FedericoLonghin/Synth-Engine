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
#include "comunication_manager.h"

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
  xTaskCreate(command_reciver, "command_reciver", 2048, NULL, 2, NULL);
  xTaskCreate(audio_task, "audioTask", 10000, NULL, 2, NULL);
  while (1)
  {
    vTaskDelay(1);
  }
  return;
}

void audio_task()
{
  struct Voice *voices[1];
  struct command *cmd = malloc(sizeof(struct command));
  float zeroPtr = 0;
  const char *TAG = "audio_Task";
  struct Voice *vo1 = malloc(sizeof(struct Voice));
  struct Voice *vo2 = malloc(sizeof(struct Voice));

  voices[0] = vo1;
  voices[0]->freq = 440;
  voices[0]->life_t = 0;
  voices[0]->op[0].freqMolt = 1;
  voices[0]->op[1].freqMolt = 2;
  voices[0]->op[0].inptr = &zeroPtr;
  voices[0]->op[1].inptr = &voices[0]->op[0].out;

  voices[0]->op[0].phase = voices[0]->op[1].phase = 0;
  voices[0]->op[0].env.Attack = voices[0]->op[1].env.Attack = 44100;
  voices[0]->op[0].env.Decay = voices[0]->op[1].env.Decay = 44100;
  voices[0]->op[0].env.Sustain = voices[0]->op[1].env.Sustain = 0.8f;
  voices[0]->op[0].env.Release = voices[0]->op[1].env.Release = 44100;
  voices[0]->op[0].env.fase = voices[0]->op[1].env.fase = ATT;

  // voices[0]->op[1].inptr = &zeroPtr;

  while (cmd_queue_handle == 0)
  {
    vTaskDelay(1); // wait for queue to be created
  }

  while (1)
  {
    if (xQueueReceive(cmd_queue_handle, (cmd), 0))
    {
      ESP_LOGI(TAG, "CMD in queue");
      printf("cmd add: %d\n", (uint8_t)cmd->cmd);
      switch (cmd->cmd)
      {
      case 'N': // NoteOn
        ESP_LOGI(TAG, "Case N");
        // uint8_t voice_n = 0;
        // voices[0]->freq = 880;
        noteOn(voices[0], cmd->val);

        break;
      default:
        break;
      }
    }

    if (fillBufferREQ)
    {
      gpio_set_level(5, 1);
      uint16_t lev = 0;
      for (int i = 0; i < outBuff_size; i++)
      {
        lev = 0;
        for (int i = 0; i < N_VOICES; i++)
        {

          processVoice(voices[i]);
          lev += (voices[i]->out) * 0xFFF;
        }
        outBuffer_toFill[i] = lev;
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