//
//  Transforms2.h
//  algorithm
//
//  Created by Abhishek Sehgal on 5/5/17.
//  Copyright © 2017 default. All rights reserved.
//

#ifndef Transforms2_h
#define Transforms2_h

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <string.h>


typedef struct Transform2 {
    
    int   nFFT;
    int   windowSize;
    float dBSPL;
    float dbpower;
    int   framesPerSecond;
    float totalPower;
    
    float complex *input;
    float *power;
    float *sine;
    float *cosine;
    float *window;
    float *dBpowerBuffer;
    
} Transform2;

Transform2* initTransform(int windowSize, int framesPerSecond, int nFFT);
void FFT(Transform2* fft, float complex * input);
void FFTwithoutWindow(Transform2* fft, float complex * input);
void IFFT(Transform2* fft);
void destroyTransform2(Transform2** transform);

#endif /* Transforms2_h */
