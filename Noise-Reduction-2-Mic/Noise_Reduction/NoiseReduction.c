//
//  NoiseReduction.c
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 6/19/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#include "NoiseReduction.h"

#define DECIMATION_FACTOR 3
#define EPS 1.0e-7
#define S2F 3.051757812500000e-05f
#define F2S 32768

const double amplitude = 0.75;

NoiseReduction* initNoiseReduction(int stepSize){
    
    NoiseReduction* inParam = (NoiseReduction*)malloc(sizeof(NoiseReduction));
    
    inParam->stepSize = stepSize;
    inParam->decimatedStepSize = stepSize/DECIMATION_FACTOR;
    
    inParam->leftIn     = (float*)calloc(sizeof(float), stepSize);
    inParam->rightIn    = (float*)calloc(sizeof(float), stepSize);
    
    inParam->leftDS = (float*)calloc(sizeof(float), stepSize);
    inParam->rightDS = (float*)calloc(sizeof(float), stepSize);
    inParam->interpolated = (float*)calloc(sizeof(float), stepSize);
    
    inParam->decimated = (short*)calloc(sizeof(short), 2*inParam->decimatedStepSize);
    inParam->decimatedMono = (float*)calloc(sizeof(float), inParam->decimatedStepSize);
    
    inParam->wienerInput = (float*)calloc(sizeof(float), inParam->decimatedStepSize);
    inParam->wienerOutput = (float*)calloc(sizeof(float), inParam->decimatedStepSize);
    
    inParam->output = (float*)calloc(sizeof(float), inParam->stepSize);
    
    inParam->firLeft = initFIR(stepSize);
    inParam->firRight = initFIR(stepSize);
    inParam->interpolation = initFIR(stepSize);
    
    inParam->nlms = initializeNLMS(inParam->decimatedStepSize,
                                   2*inParam->decimatedStepSize,
                                   64,
                                   0.1);
    wienerAB_init();
    return inParam;
}

void doNoiseReduction(NoiseReduction* _ptr, short* _in, short* _out, int audioOutput){
    
    NoiseReduction* inParam = _ptr;
    
    int i,j;
    
    // Deinterleave
    
    for (i = 0, j = 0; i < inParam->stepSize; i++, j += 2) {
        inParam->leftIn[i]  = _in[j]   * S2F;
        inParam->rightIn[i] = _in[j+1] * S2F;
    }
    
    // Downsample
    
    processFIRFilter(inParam->firLeft,  inParam->leftIn,  inParam->leftDS);
    processFIRFilter(inParam->firRight, inParam->rightIn, inParam->rightDS);
    
    // Decimate and InterLeave
    
    for (i = 0, j = 0; i < 2*inParam->decimatedStepSize; i += 2, j += 3) {
        inParam->decimated[i]   = F2S * inParam->leftDS[j];
        inParam->decimated[i+1] = F2S * inParam->rightDS[j];
        inParam->decimatedMono[(int)i/2] = 0.5*(inParam->leftDS[j] + inParam->rightDS[j]);
    }
    
    // Block-NLMS
    if (audioOutput == 0) {
        for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j += 3) {
            inParam->interpolated[j] = inParam->decimatedMono[i];
        }
    }
    
    else if (audioOutput == 1) {
        wienerAB(inParam->decimatedMono, -1, 16000, inParam->wienerOutput);
        // Interpolation
        
        for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j += 3) {
            inParam->output[j] = inParam->wienerOutput[i];
        }
        processFIRFilter(inParam->interpolation, inParam->output, inParam->interpolated);			
    }
    
    else if (audioOutput == 2) {
        processAudio(inParam->nlms, inParam->decimated, inParam->wienerInput);
        wienerAB(inParam->wienerInput, -1, 16000, inParam->wienerOutput);
        // Interpolation
        
        for (i = 0, j = 0; i < inParam->decimatedStepSize; i++, j += 3) {
            inParam->output[j] = inParam->wienerOutput[i];
        }
        
        processFIRFilter(inParam->interpolation, inParam->output, inParam->interpolated);
    }
    
    
    
    
    
    
    // Interleave
    
        for (i = 0, j = 0; i < inParam->stepSize; i++, j += 2) {
            _out[j] =   (short)(F2S * inParam->interpolated[i]);
            _out[j+1] = (short)(F2S * inParam->interpolated[i]);
        }
    
}
