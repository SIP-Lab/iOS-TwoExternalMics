//
//  SpeechProcessing.c
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/12/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#include "SpeechProcessing.h"

#define DECIMATION_FACTOR 3

static int* smoothingBuffer;
static int smoothingBufferLength;

long* initialize(int stepSize, int samplingFrequency, int decisionBufferLength) {
    
    Variables* inParam = (Variables*)malloc(sizeof(Variables));
    
    inParam->stepSize               = (int)floor((float)stepSize/(float)DECIMATION_FACTOR);
    inParam->frameSize              = 2 * inParam->stepSize;
    inParam->fs                     = samplingFrequency;
    inParam->firstFrame             = 1;
    inParam->frameCount             = 0;
    inParam->decisionBufferLength   = decisionBufferLength;
    inParam->class                  = -1;
    
    inParam->fir                    = initFIR(stepSize);
    
    int pow2size = 0x1;
    
    while (pow2size < inParam->frameSize) {
        pow2size = pow2size << 1;
    }
    
    inParam->input                  = (float complex *)calloc(sizeof(float complex), inParam->frameSize);
    inParam->fft                    = initTransform(inParam->frameSize, (int)(samplingFrequency/stepSize), pow2size);
    inParam->sf                     = initSubbandFeatures(inParam->fft->nFFT, decisionBufferLength);
    inParam->rf                     = initRandomForest();
    
    smoothingBufferLength   = 5;
    smoothingBuffer         = calloc(sizeof(int), smoothingBufferLength);
    
    return (long*)inParam;
}

void compute(long *memoryPointer, float *input, float* output,  size_t channelNo){
    
    Variables* inParam = (Variables*)memoryPointer;
    
    processFIRFilter(inParam->fir, input, output);
    
    for (size_t n = 0; n < inParam->stepSize; n++) {
        inParam->input[n] = inParam->input[n + inParam->stepSize];
        inParam->input[n + inParam->stepSize] = output[3*n];
    }
    
    // Compute the FFT of the incoming frame
    FFT(inParam->fft, inParam->input);
    
    // Calculate the subband features of the frame
    computeSubbandFeatures(inParam->sf, inParam->fft, &inParam->firstFrame);

    if (inParam->frameCount > inParam->decisionBufferLength) {
        evalTrees(inParam->rf, inParam->sf->subbandFeatureList);
        if (channelNo) {
            printf("%f, %f, %f\n",inParam->rf->scores[0],
                   inParam->rf->scores[1],
                   inParam->rf->scores[2]);
        }
        inParam->frameCount = 0;
        
        inParam->class = inParam->rf->classDecision;
        
        
        //Smoothing buffer: The decision is averaged over
        // a duration of 5 * Decision Buffer Length * step size
        int i, class1 = 0, class2 = 0, class3 = 0;
        
        for (i = smoothingBufferLength-1; i > 0 ; i--) {
            smoothingBuffer[i] = smoothingBuffer[i-1];
        }
        
        smoothingBuffer[0] = inParam->rf->classDecision;
        
        for (i = 0; i < smoothingBufferLength; i++) {
            switch (smoothingBuffer[i]) {
                case 1:
                    class1++;
                    break;
                case 2:
                    class2++;
                    break;
                case 3:
                    class3++;
                    break;
                default:
                    break;
            }
        }
        
        if (class1 > class2) {
            if (class1 > class3) {
                inParam->class = 1;
            }
            else {
                inParam->class = 3;
            }
        }
        else {
            if (class2 > class3) {
                inParam->class = 2;
            }
            else {
                inParam->class = 3;
            }
        }
    }
    else{
        inParam->frameCount++;
    }
//
//
//    memcpy(output, input, sizeof(float) * inParam->stepSize);

}



int returnElements(long* memoryPointer){
    Variables* inParam = (Variables*)memoryPointer;
    return inParam->rf->nClasses;
}

void getScores(long* memoryPointer, float* scores){
    Variables* inParam = (Variables*)memoryPointer;
    
    for(int i = 0; i < inParam->rf->nClasses; i++){
        scores[i] = inParam->rf->scores[i];
    }
}

void copyArray(long *memoryPointer, float* array){
    Variables* inParam = (Variables*)memoryPointer;
    memcpy(array, inParam->fir->inputBuffer + inParam->fir->N, inParam->fir->N * sizeof(float));
}



void destroy(long* memoryPointer) {
    
    Variables* inParam = (Variables*)memoryPointer;
    
    if (memoryPointer != NULL) {
        
        destroyFIR(&(inParam->fir));
        destroyTransform2(&(inParam->fft));
        destroySubbandFeatures(&(inParam->sf));
        destroyRandomForest(&(inParam->rf));
        
        if (inParam->input != NULL) {
            free(inParam->input);
            inParam->input = NULL;
        }
        
        free(inParam);
        inParam = NULL;
    }
    
}
