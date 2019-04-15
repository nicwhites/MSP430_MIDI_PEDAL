#ifndef rotaryencoder_H_
#define rotaryencoder_H_

#include <msp430.h>
#include "WS2812Driver.h"

#define ENCODER_A BIT1 //rotary encoder pin A
#define ENCODER_B BIT3 //rotary encoder pin B


//pin C (usually in the middle) connect on ground

void encoderInit();
void stepCCW();
void stepCW();


#endif // ENC_ROT_H_
