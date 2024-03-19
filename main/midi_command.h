#pragma once
#define MIDI_Note_On 144
#define MIDI_Note_Off 128
#define MIDI_Set_Env_Param 244
/*
MIDI_Set_Env_Param command structure
Byte: 0 244
      1 Op_num
      2 Env_param
      3
      4 float
      5 float
      6 float
      7 float
*/

typedef enum{
Attack,
Decay,
Sustain,
Release

}Env_param;