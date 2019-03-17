//
//  NoiseReduction.h
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 6/19/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#ifndef NoiseReduction_h
#define NoiseReduction_h

#include <stdio.h>
#include <complex.h>
#include "FIRFilter.h"
#include "NLMS.h"
#include "wienerAB.h"

typedef struct NoiseReduction {
    
    int stepSize;
    int decimatedStepSize;
    
    float* leftIn;
    float* rightIn;
    float* leftDS;
    float* rightDS;
    short* decimated;
    float* decimatedMono;
    float* interpolated;
    
    float* wienerInput;
    float* wienerOutput;
    float* output;
    
    FIR* firLeft;
    FIR* firRight;
    FIR* interpolation;
    
    NLMS* nlms;
    
} NoiseReduction;

NoiseReduction* initNoiseReduction(int stepSize);
void doNoiseReduction(NoiseReduction* _ptr, short* _in, short* _out, int audioOutput);

#endif /* NoiseReduction_h */
