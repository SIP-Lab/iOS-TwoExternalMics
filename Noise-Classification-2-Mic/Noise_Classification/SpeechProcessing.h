//
//  SpeechProcessing.h
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/12/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#ifndef SpeechProcessing_h
#define SpeechProcessing_h

#include <stdio.h>
#include <complex.h>
#include "Transforms2.h"
#include "SubbandFeatures.h"
#include "RandomForest.h"
#include "FIRFilter.h"

typedef struct Variables {
    
    int frameSize;
    int stepSize;
    int fs;
    int firstFrame;
    int frameCount;
    int decisionBufferLength;
    int class;
    
    FIR* fir;
    float complex   *input;
    Transform2      *fft;
    SubbandFeatures *sf;
    RandomForests   *rf;
    
} Variables;

long* initialize(int stepSize, int samplingFrequency, int decisionBufferLength);
void compute(long* memoryPointer, float* input, float* output, size_t channelNo);
void destroy(long* memoryPointer);
int returnElements(long* memoryPointer);
void copyArray(long *memoryPointer, float* array);
void getScores(long* memoryPointer, float* scores);
#endif /* SpeechProcessing_h */
