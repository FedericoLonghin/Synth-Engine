#include "voice.h"
#include "sine_table.h"
void processVoice(struct Voice *voice)
{
    const char *TAG = "processVoice";
    // ESP_LOGI(TAG, " ");
    voice->phase += TWO_PI * (voice->freq / SAMPLE_RATE);
    if (voice->phase >= TWO_PI)
    {
        voice->phase = 0;
        // ESP_LOGI(TAG, "FASE\n");
    }
    voice->out = sineLookupTable[(int)(voice->phase * (sineLookupTable_size / TWO_PI))];
}