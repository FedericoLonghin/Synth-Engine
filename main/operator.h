#pragma once
#include "env.h"

 struct Operator
{
    float in;
    float out;

    struct Envelope env;
    float env_level;
};