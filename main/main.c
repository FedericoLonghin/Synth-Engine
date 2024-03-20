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
  xTaskCreate(audio_task, "audioTask", 10000, NULL, 5, NULL);
  while (1)
  {
    vTaskDelay(100);
  }
  return;
}
void audio_task()
{
struct Voice *voices = malloc(N_VOICES * sizeof(struct Voice));
  // *voices = malloc(N_VOICES * sizeof(struct Voice));
  command *cmd = malloc(sizeof(command));
  float zeroPtr = 0;
  const char *TAG = "audio_Task";
  // struct Voice *vo1 = malloc(sizeof(struct Voice));
  // struct Voice *vo2 = malloc(sizeof(struct Voice));

  for (int vo_n = 0; vo_n < N_VOICES; vo_n++)
  {
    voices[vo_n].freq = 0;
    voices[vo_n].life_t = 0;
    voices[vo_n].op[0].freqMolt = 9;
    voices[vo_n].op[1].freqMolt = 1;
    voices[vo_n].op[0].inptr = &zeroPtr;
    voices[vo_n].op[1].inptr = &voices[vo_n].op[0].out;

    voices[vo_n].op[0].amplCoeff = 0.1;
    voices[vo_n].op[1].amplCoeff = 1;

    voices[vo_n].op[0].phase = voices[vo_n].op[1].phase = 0;
    voices[vo_n].op[0].env.fase = voices[vo_n].op[1].env.fase = ATT;

    voices[vo_n].op[0].env.Attack = 44100;
    voices[vo_n].op[0].env.Decay = 44100;
    voices[vo_n].op[0].env.Sustain = 0.2f;
    voices[vo_n].op[0].env.Release = 0;
    voices[vo_n].op[1].env.Attack = 0;
    voices[vo_n].op[1].env.Decay = 4410;
    voices[vo_n].op[1].env.Sustain = 0.3f;
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
      switch (cmd->cmd)
      {
      case MIDI_Note_On: // NoteOn
        voiceSelector++;
        if (voiceSelector >= N_VOICES)
          voiceSelector = 0;
        noteOn(&voices[voiceSelector], cmd->val_uint8);
        break;
      case MIDI_Note_Off: // NoteOff
        for (uint8_t vo_n = 0; vo_n < N_VOICES; vo_n++)
        {
          if (voices[vo_n].note == cmd->val_uint8)
          {
            noteOff(&voices[vo_n]);
          }
        }
        break;
      case MIDI_Set_Env_Param:
        ESP_LOGI(TAG, "MIDI_Set_Env_Param");
        for (int vo_n = 0; vo_n < N_VOICES; vo_n++)
        {
          switch (cmd->param2)
          {
          case Attack:
            ESP_LOGI(TAG, " -Setted Op:%d Attack to: %f", cmd->param1, cmd->val_float);
            voices[vo_n].op[cmd->param1].env.Attack = cmd->val_float;
            break;
          case Decay:
            ESP_LOGI(TAG, "Setted Op:%d Decay to: %f", cmd->param1, cmd->val_float);
            voices[vo_n].op[cmd->param1].env.Decay = cmd->val_float;
            break;
          case Sustain:
            ESP_LOGI(TAG, "Setted Op:%d Sustain to: %f", cmd->param1, cmd->val_float);
            voices[vo_n].op[cmd->param1].env.Sustain = cmd->val_float;
            break;
          case Release:
            ESP_LOGI(TAG, "Setted Op:%d Release to: %f", cmd->param1, cmd->val_float);
            voices[vo_n].op[cmd->param1].env.Release = cmd->val_float;
            break;
          }
        }
        break;
      default:
        ESP_LOGE(TAG, "Unknow command %d", cmd->cmd);
        break;
      }
    }

    if (fillBufferREQ)
    {
      gpio_set_level(5, 1);
      processVoices(voices,outBuff_size);

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