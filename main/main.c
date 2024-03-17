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
#include "midi_command.h"

#include "voice.h"

#define MONO_VOICE 0
void audio_task();
int voiceSelector = 0;
void app_main(void)
{
  const char *TAG = "app_main";
  ESP_LOGI(TAG, "Started");

  printf("Synth Engine V-1.0\n");

  gpio_set_direction(5, GPIO_MODE_OUTPUT);
  gpio_set_level(5, 0);

  i2s_init();
  xTaskCreate(command_reciver, "command_reciver", 2048, NULL, configMAX_PRIORITIES, NULL);
  xTaskCreate(audio_task, "audioTask", 10000, NULL, 2, NULL);
  while (1)
  {
    vTaskDelay(100);
  }
  return;
}

void audio_task()
{
  struct Voice *voices = malloc(N_VOICES * sizeof(struct Voice));
  struct command *cmd = malloc(sizeof(struct command));
  float zeroPtr = 0;
  const char *TAG = "audio_Task";
  struct Voice *vo1 = malloc(sizeof(struct Voice));
  struct Voice *vo2 = malloc(sizeof(struct Voice));

  // voices[0] = vo1;
  // voices[1] = vo2;
  for (int vo_n = 0; vo_n < N_VOICES; vo_n++)
  {
    voices[vo_n].freq = 0;
    voices[vo_n].life_t = 0;
    voices[vo_n].op[0].freqMolt = 2;
    voices[vo_n].op[1].freqMolt = 1;
    voices[vo_n].op[0].inptr = &zeroPtr;
    voices[vo_n].op[1].inptr = &voices[0].op[0].out;

    voices[vo_n].op[0].amplCoeff = 0;
    voices[vo_n].op[1].amplCoeff = 1;

    voices[vo_n].op[0].phase = voices[vo_n].op[1].phase = 0;
    voices[vo_n].op[0].env.fase = voices[vo_n].op[1].env.fase = ATT;

    voices[vo_n].op[0].env.Attack = 44100;
    voices[vo_n].op[0].env.Decay = 4410;
    voices[vo_n].op[0].env.Sustain = 1.0f;
    voices[vo_n].op[0].env.Release = 4410;
    voices[vo_n].op[1].env.Attack = 410;
    voices[vo_n].op[1].env.Decay = 4410;
    voices[vo_n].op[1].env.Sustain = 0.2f;
    voices[vo_n].op[1].env.Release = 4410;
  }

  while (cmd_queue_handle == 0)
  {
    vTaskDelay(1); // wait for queue to be created
  }

  while (1)
  {
    // Commands reciver
    if (xQueueReceive(cmd_queue_handle, (cmd), 0))
    {
      ESP_LOGI(TAG, "CMD in queue");
      printf("cmd add: %d\n", (uint8_t)cmd->cmd);
      switch (cmd->cmd)
      {
      case MIDI_Note_On: // NoteOn
// ESP_LOGI(TAG, "Case N");
#if MONO_VOICE
        noteOn(&voices[0], cmd->val);
#else
        voiceSelector++;
        if (voiceSelector >= N_VOICES)
          voiceSelector = 0;
        noteOn(&voices[voiceSelector], cmd->val);
#endif
        break;
      case MIDI_Note_Off: // NoteOff
                          // ESP_LOGI(TAG, "Case O");
#if MONO_VOICE
        noteOff(&voices[0]);
#else
        for (uint8_t vo_n = 0; vo_n < N_VOICES; vo_n++)
        {
          if (voices[vo_n].note == cmd->val)
          {

            noteOff(&voices[vo_n]);
          }
        }
#endif
        break;
      default:
        ESP_LOGE(TAG, "Unknow command %d", cmd->cmd);
        break;
      }
    }

    if (fillBufferREQ)
    {
      // gpio_set_level(5, 1);
      uint16_t lev = 0;
      for (int i = 0; i < outBuff_size; i++)
      {
        lev = 0;
        for (int i = 0; i < N_VOICES; i++)
        {

          processVoice(&voices[i]);
          lev += (voices[i].out) * 0xFFF;
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
      // gpio_set_level(5, 0);
      // vTaskDelay(1);
    }
  }
  vTaskDelete(NULL);
}