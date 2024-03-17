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
        float ampl = getEnvelope_ampl(&voice->op[op_n].env, (voice->life_t)) * voice->op[op_n].amplCoeff;
        if (ampl < 0)
            ESP_LOGE(TAG, "ampl OOR negative");
        if (ampl > 1)
            ESP_LOGE(TAG, "ampl OOR positive");
        //       output     = |            input           | * |                            Oscillator                                       | * |env|
        // voice->op[op_n].out = (*(voice->op[op_n].inptr) + 1) * sineLookupTable[(int)(voice->op[op_n].phase * (sineLookupTable_size / TWO_PI))] * ampl;
        voice->op[op_n].out = sineLookupTable[(int)((*(voice->op[op_n].inptr) + 1) * voice->op[op_n].phase * (sineLookupTable_size / TWO_PI))%sineLookupTable_size] * ampl;
    }

    voice->out = voice->op[N_OPERATORS - 1].out;
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
    voice->op[0].env.fase = ATT;
    voice->op[1].env.fase = ATT;

    voice->phase = 0;
    ESP_LOGI(TAG, "freq:%f", voice->freq);
}
void noteOff(struct Voice *voice)
{
    const char *TAG = "NoteOff";
    voice->op[0].env.Release_StartVal = getEnvelope_ampl(&voice->op[0].env, voice->life_t);
    voice->op[1].env.Release_StartVal = getEnvelope_ampl(&voice->op[0].env, voice->life_t);
    voice->life_t = 0;
    voice->op[0].env.fase = REL;
    voice->op[1].env.fase = REL;
}

float getEnvelope_ampl(struct Envelope *env, uint32_t time)
{
    const char *TAG = "getEnvelope_ampl";
    switch (env->fase)
    {
    case ATT:
        if (time >= env->Attack)
        {
            env->fase = DEC;
            time = 0;
            ESP_LOGI(TAG, "SETTING DEC");
        }

        return (uint32_t)time / (float)env->Attack;
        break;

    case DEC:

        if (time >= env->Decay + env->Attack)
        {
            ESP_LOGI(TAG, "setting SUS");
            env->Release_StartVal = env->Sustain;
            env->fase = SUS;
        }
        time -= env->Attack;
        return 1 - ((1 - env->Sustain) * ((uint32_t)time / (float)env->Decay));
        break;

    case SUS:
        return env->Sustain;
        break;
    case REL:
        env->Release_StartVal -= (1 / (float)env->Release);
        if (env->Release_StartVal <= 0)
        {
            ESP_LOGI(TAG, "setting OFF");
            env->fase = OFF;
            return 0;
        }
        return env->Release_StartVal;
        break;
    case OFF:
        return 0;
        break;
    }
    return 0;
}
