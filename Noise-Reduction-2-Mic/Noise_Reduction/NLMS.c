//
//  NLMS.c
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 6/5/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#include "NLMS.h"

float checkOutputRange(float input){
    float output;
    if (input > 1.0) {
        output = 1.0;
    }
    else if(input < -1.0){
        output = -1.0;
    }
    else {
        output = input;
    }
    
    return output;
}

NLMS* initializeNLMS(int stepSize, int frameSize, int filtLen, float mu) {
    NLMS *mainParam = (NLMS*) malloc(sizeof(NLMS));
    int i;
    
    mainParam->u = mu;
    mainParam->stepSize = stepSize;
    mainParam->frameSize = frameSize;
    mainParam->filLen = 64;
    
    mainParam->topMicBuffer = (float*) calloc(frameSize + mainParam->filLen - 1,sizeof(float));
    mainParam->botMicBuffer = (float*) calloc(frameSize + mainParam->filLen - 1,sizeof(float));
    mainParam->w = (float*) malloc(mainParam->filLen * sizeof(float));
    for (i = 0; i < mainParam->filLen; i++) {
        mainParam->w[i] = 0.5;
    }
    mainParam->y_curr = (float*) calloc(frameSize, sizeof(float));
    mainParam->y_prev = (float*) calloc(frameSize, sizeof(float));
    mainParam->e = (float*) calloc(stepSize, sizeof(float));
    
    return mainParam;
}

void processAudio(NLMS* memoryPointer, short* _in, float* _out){
    
    NLMS *mainParam = memoryPointer;
    
    int i, j, stepSize, frameSize, filLen;
    float tempVar, tempEng;
    stepSize = mainParam->stepSize;
    frameSize = mainParam->frameSize;
    filLen = mainParam->filLen;
    
    for (i = 0; i < filLen - 1; i++) {
        mainParam->topMicBuffer[i] = mainParam->topMicBuffer[i + stepSize];
        mainParam->botMicBuffer[i] = mainParam->botMicBuffer[i + stepSize];
    }
    
    for (i = filLen - 1, j = 0; i < filLen + stepSize - 1; i++, j += 2) {
        mainParam->topMicBuffer[i] = mainParam->topMicBuffer[i + stepSize];
        mainParam->topMicBuffer[i + stepSize] = _in[j]*FAC;
        mainParam->botMicBuffer[i] = mainParam->botMicBuffer[i + stepSize];
        mainParam->botMicBuffer[i + stepSize] = _in[j+1]*FAC;
    }
    
    /* Pre Processing */
    
    tempEng = 0;
    for (i = 0; i < frameSize; i++) {
        mainParam->y_prev[i] = mainParam->y_curr[i];
        tempVar = 0;
        for (j = 0; j < filLen; j++) {
            tempVar += mainParam->w[j] * mainParam->botMicBuffer[i + j];
            tempEng += mainParam->botMicBuffer[i + j]* mainParam->botMicBuffer[i + j];
        }
        mainParam->y_curr[i] = tempVar;
    }
    
    for (i = 0; i < stepSize; i++) {
        mainParam->e[i] = mainParam->topMicBuffer[i + filLen - 1]- 0.5 * (mainParam->y_prev[i + stepSize] + mainParam->y_curr[i]);
    }
    
    for (i = 0; i < filLen; i++) {
        tempVar = 0;
        for (j = 0; j < stepSize; j++) {
            tempVar += mainParam->botMicBuffer[j + i] * mainParam->e[j];
        }
        mainParam->w[i] += mainParam->u * tempVar / (tempEng + EPS);
    }
    
    for (i = 0; i < stepSize; i++) {
        _out[i] = checkOutputRange(mainParam->e[i]);
    }
    
}


