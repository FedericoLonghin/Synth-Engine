#include "voice.h"
#include "sine_table.h"
void processVoices(struct Voice *voice, uint16_t samples)
{
    float ampl[N_VOICES][N_OPERATORS] = {0};
    uint16_t lev = 0;
    for (int sa_n = 0; sa_n < samples; sa_n++)
    {
        lev = 0;
        for (int vo_n = 0; vo_n < N_VOICES; vo_n++)
        {
            uint8_t voice_env_off_num = 0;
            if (0 || voice[vo_n].isActive)
            {
                const char *TAG = "processVoice";
                // Calcolating operators contribute
                for (int op_n = 0; op_n < N_OPERATORS; op_n++)
                {

                    // printf("%d\n", vo_n);
                    // Calcolating phase
                    voice[vo_n].op[op_n].phase += TWO_PI * ((voice[vo_n].freq * voice[vo_n].op[op_n].freqMolt) / SAMPLE_RATE);
                    if (voice[vo_n].op[op_n].phase >= TWO_PI)
                        voice[vo_n].op[op_n].phase -= TWO_PI;

                    // Calcolating envelope ampl
                    if (sa_n % 10 == 0)
                    {
                        ampl[vo_n][op_n] = getEnvelope_ampl(&voice[vo_n].op[op_n].env, (voice[vo_n].life_t)) * voice[vo_n].op[op_n].amplCoeff;
                        if (voice[vo_n].op[op_n].env.fase == OFF)
                            voice_env_off_num++;
                        // if (ampl[vo_n][op_n] < 0)
                        //     ESP_LOGE(TAG, "ampl OOR negative");
                        // if (ampl[vo_n][op_n] > 1)
                        //     ESP_LOGE(TAG, "ampl OOR positive");
                    }

                    //       output     = |            input           | * |                            Oscillator                                       | * |env|
                    // voice[vo_n].op[op_n].out = (*(voice[vo_n].op[op_n].inptr) + 1) * sineLookupTable[(int)(voice[vo_n].op[op_n].phase * (sineLookupTable_size / TWO_PI))] * ampl;
                    voice[vo_n].op[op_n].out = sineLookupTable[(int)(((*(voice[vo_n].op[op_n].inptr))*50) + voice[vo_n].op[op_n].phase * (sineLookupTable_size / TWO_PI)) % sineLookupTable_size] * ampl[vo_n][op_n];
                }

                voice[vo_n].out = voice[vo_n].op[N_OPERATORS - 1].out;
                // Updating note life
                voice[vo_n].life_t++;

                lev += (voice[vo_n].out) * 0xFA0;
                if (voice_env_off_num == N_OPERATORS)
                {
                    voice[vo_n].isActive = false;
                    ESP_LOGI(TAG, "END VOICE n%d", vo_n);
                }
            }
        }
        outBuffer_toFill[sa_n] = lev;
        // printf("%d\n", outBuffer_toFill[sa_n]);
    }
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
    ESP_LOGI(TAG, "note: %d", note);
    voice->isActive = true;

    voice->note = note;
    voice->freq = 440 * pow(2, ((note - 81) / 12.0f));
    voice->life_t = 0;
    for (int op_n = 0; op_n < N_OPERATORS; op_n++)
    {
        voice->op[op_n].phase = 0;
        voice->op[op_n].env.fase = ATT;
    }

    ESP_LOGI(TAG, "freq:%f", voice->freq);
}
void noteOff(struct Voice *voice)
{
    const char *TAG = "NoteOff";
    // voice->isActive = false;
    for (int op_n = 0; op_n < N_OPERATORS; op_n++)
    {
        voice->op[op_n].env.Release_StartVal = getEnvelope_ampl(&voice->op[op_n].env, voice->life_t);
        voice->op[op_n].env.fase = REL;
    }
    voice->life_t = 0;
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
            // ESP_LOGI(TAG, "SETTING DEC");
            return 1;
        }

        return (uint32_t)time / (float)env->Attack;
        break;

    case DEC:

        if (time >= env->Decay + env->Attack)
        {
            // ESP_LOGI(TAG, "setting SUS");
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
        env->Release_StartVal -= (10 / (float)env->Release);
        if (env->Release_StartVal <= 0)
        {
            // ESP_LOGI(TAG, "setting OFF");
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
