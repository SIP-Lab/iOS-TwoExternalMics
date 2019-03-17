//
//  SubbandFeatures.h
//  algorithm
//
//  Created by Abhishek Sehgal on 5/15/17.
//  Copyright © 2017 default. All rights reserved.
//

#ifndef SubbandFeatures_h
#define SubbandFeatures_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "Transforms2.h"

typedef struct SubbandFeatures {
    
    int nBands;
    int nFFT;
    float normalize;
    int subbandWidth;
    int histBins;
    int decisionBufferLength;
    
    float complex* currentFrame;
    float complex* previousFrame;
    
    float* subbandPower;
    int*   histCount;
    
    Transform2** currentSubbands;
    Transform2** previousSubbands;
    Transform2** periodicityTransform;
    
    float*  bandPeriodicity;
    float** bandPeriodicityBuffer;
    float*  bandEntropy;
    float** bandEntropyBuffer;
    float*  subbandFeatureList;
    
    
} SubbandFeatures;

SubbandFeatures* initSubbandFeatures(int nFFT, int decisionBufferLength);
void computeSubbandFeatures(SubbandFeatures* sbf, Transform2* fft, int *isFirstFrame);
void destroySubbandFeatures(SubbandFeatures** sbf);

#endif /* SubbandFeatures_h */
