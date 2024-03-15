#include "voice.h"
#include "sine_table.h"
void processVoice(struct Voice *voice)
{
    const char *TAG = "processVoice";

    // Calcolating phase
    voice->phase += TWO_PI * (voice->freq / SAMPLE_RATE);
    if (voice->phase >= TWO_PI)
        voice->phase = 0;

    // Calcolating operators contribute
    for (int op_n = 0; op_n < N_OPERATORS; op_n++)
    {
        voice->op[op_n].phase += TWO_PI * ((voice->freq * voice->op[op_n].freqMolt) / SAMPLE_RATE);
        if (voice->op[op_n].phase >= TWO_PI)
            voice->op[op_n].phase = 0;

        voice->op[op_n].out = *(voice->op[op_n].inptr) * sineLookupTable[(int)(voice->op[op_n].phase * (sineLookupTable_size / TWO_PI))];
    }

    voice->out = voice->op[1].out;
}
void calcAlgoritmm()
{
    switch (1)
    {
    case 1:

        break;
    default:
        break;
    }
}

void noteOn(struct Voice *voice,uint8_t note)
{
    const char *TAG = "NoteOn";
    voice->freq = note * 100;
    voice->life_t = 0;
    voice->phase = 0;
    ESP_LOGI(TAG, "freq:%f", 2.0);
}