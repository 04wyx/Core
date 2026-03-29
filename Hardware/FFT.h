#ifndef _FFT_H_
#define _FFT_H_

#include "main.h"
#include "tim.h"
#include "math.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#include "Config.h"

extern float FFT_in[ NPT*2 ];
extern float FFT_out[ NPT/2 ];
extern float ADC_Float[ NPT ];
extern uint16_t ADC_Value[ NPT ];

int find_peak(void);
void FFT_test(void);
float Find_Wave_Amp(int index, int step);
int wave_set(int index);

#endif
