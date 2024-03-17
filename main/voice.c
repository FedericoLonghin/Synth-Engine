#include "voice.h"
#include "sine_table.h"
void processVoice(struct Voice *voice)
{
    const char *TAG = "processVoice";
    if (voice->life_t == 0)
    {
        ESP_LOGI(TAG, "life=0");
    }
    // Calcolating operators contribute
    for (int op_n = 0; op_n < N_OPERATORS; op_n++)
    {
        // Calcolating phase
        voice->op[op_n].phase += TWO_PI * ((voice->freq * voice->op[op_n].freqMolt) / SAMPLE_RATE);
        if (voice->op[op_n].phase >= TWO_PI)
            voice->op[op_n].phase = 0;

        // Calcolating envelope ampl
        float ampl = getEnvelope_ampl(&voice->op[op_n].env, &(voice->life_t));

        //       output     = |            input           | * |                            Oscillator                                       | * |env|
        voice->op[op_n].out = (*(voice->op[op_n].inptr) + 1) * sineLookupTable[(int)(voice->op[op_n].phase * (sineLookupTable_size / TWO_PI))] * ampl;
    }

    voice->out = voice->op[N_OPERATORS-1].out;
    // Updating note life
    voice->life_t++;
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

void noteOn(struct Voice *voice, uint8_t note)
{
    const char *TAG = "NoteOn";
    voice->freq = note * 100;
    voice->life_t = 0;
    voice->op[0].env.fase =  ATT;
    voice->op[1].env.fase =  ATT;

    voice->phase = 0;
    ESP_LOGI(TAG, "freq:%f", voice->freq);
}

float getEnvelope_ampl(struct Envelope *env, uint32_t *time)
{
    const char *TAG = "getEnvelope_ampl";
    switch (env->fase)
    {
    case ATT:
        if (*time >= env->Attack)
        {
            env->fase = DECa;
            *time = 0;
            ESP_LOGI(TAG, "SETTING DEC");
        }

        return (uint32_t)*time / (float)env->Attack;
        break;

    case DECa:

        if (*time >= env->Decay)
        {
            ESP_LOGI(TAG, "setting SUS");
            env->fase = SUS;
            *time = 0;
            env->Release_StartVal = env->Sustain;
        }
        // return 1;
        return 1 - ((1 - env->Sustain) * ((uint32_t)*time / (float)env->Decay));
        break;

    case SUS:

        return env->Sustain;
        break;
    case REL:
        return 1 - (1 - env->Release_StartVal) * ((uint32_t)*time / (float)env->Release);
        break;
    case OFF:
        return 0;
        break;
    }
    return 0;
}
