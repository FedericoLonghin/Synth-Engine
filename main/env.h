#pragma once
typedef enum
{
    ATT,
    DECa,
    SUS,
    REL,
    OFF
} EnvelopeFase;

struct Envelope
{
    uint32_t Attack;
    uint32_t Decay;
    float Sustain;
    uint32_t Release;
    float Release_StartVal;
    EnvelopeFase fase;
};
