//
//  NLMS.h
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 6/5/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

#ifndef NLMS_h
#define NLMS_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EPS 1.0e-7
#define FAC 3.051757812500000e-05f


typedef struct NLMS{
    int stepSize;
    int frameSize;
    int filLen;
    float u;
    
    float* topMicBuffer;
    float* botMicBuffer;
    float* y_prev;
    float* y_curr;
    float* w;
    float* e;
} NLMS;

NLMS* initializeNLMS(int stepSize, int frameSize, int filtLen, float mu);
void processAudio(NLMS* memoryPointer, short* _in, float* _out);

#endif /* NLMS_h */
