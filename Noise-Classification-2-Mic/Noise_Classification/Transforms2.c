//
//  Transforms2.c
//  algorithm
//
//  Created by Abhishek Sehgal on 5/5/17.
//  Copyright © 2017 default. All rights reserved.
//

#include "Transforms2.h"
#define P_REF -93.9794

Transform2* initTransform(int windowSize, int framesPerSecond, int nFFT) {
    Transform2* newTransform = (Transform2*)malloc(sizeof(Transform2));
    
    newTransform->windowSize = windowSize;
    newTransform->framesPerSecond = framesPerSecond;
    
    
    int pow2Size = nFFT;
    //    while (pow2Size < windowSize) {
    //        pow2Size = pow2Size << 1;
    //    }
    newTransform->nFFT = nFFT;
    
    newTransform->input  = (float complex *)malloc(pow2Size*sizeof(float complex));
    newTransform->power  = (float*)malloc(pow2Size*sizeof(float));
    newTransform->sine   = (float*)malloc((pow2Size/2)*sizeof(float));
    newTransform->cosine = (float*)malloc((pow2Size/2)*sizeof(float));
    newTransform->dBpowerBuffer = (float*)calloc(framesPerSecond,
                                                 sizeof(float));
    newTransform->dbpower = 0;
    
    //precompute twiddle factors
    double arg;
    int i;
    for (i = 0; i < pow2Size/2; i++) {
        arg = (-2.0*M_PI*i)/pow2Size;
        newTransform->cosine[i] = cosf(arg);
        newTransform->sine[i]   = sinf(arg);
    }
    
    //create Hanning Window
    newTransform->window = (float*)malloc(pow2Size*sizeof(float));
    for (i = 0; i < windowSize; i++) {
        newTransform->window[i] = (1.0 - cosf(2.0*M_PI*(i+1)/(windowSize+1)))*0.5;
    }
    for (i=windowSize; i < pow2Size; i++) {
        newTransform->window[i] = 0;
    }
    
    return newTransform;
}

void FFT(Transform2* fft, float complex * input) {
    int i,j,k,L,m,n,o,p,q;
    float tempReal, tempImaginary, cos, sin,xt, yt, temp;
    k = fft->nFFT;
    fft->totalPower = 0;
    
    for (i = 0; i < k; i++) {
        fft->input[i] = 0.0 + 0.0*I;
    }
    
    for (i = 0; i < fft->windowSize; i++) {
        fft->input[i] = input[i] * fft->window[i];
    }
    
    j = 0;
    m = k/2;
    
    //bit reversal
    for (i = 1; i<(k-1);i++) {
        
        L = m;
        
        while(j>=L) {
            j = j-L;
            L = L/2;
        }
        
        j = j + L;
        
        if(i<j) {
            tempReal = crealf(fft->input[i]);
            tempImaginary = cimagf(fft->input[i]);
            
            fft->input[i] = fft->input[j];
            
            fft->input[j] = tempReal + tempImaginary * I;
        }
    }
    
    
    L = 0;
    m = 1;
    n = k/2;
    
    //computation
    for (i = k; i > 1; i = (i>>1)) {
        L = m;
        m = 2*m;
        o = 0;
        
        for (j = 0; j < L; j++) {
            cos = fft->cosine[o];
            sin = fft->sine[o];
            o = o+n;
            
            for (p = j; p < k; p = p + m) {
                q = p + L;
                
                xt = cos * crealf(fft->input[q]) - sin * cimagf(fft->input[q]);
                yt = sin * crealf(fft->input[q]) + cos * cimagf(fft->input[q]);
                
                fft->input[q] = (crealf(fft->input[p]) - xt) + I * (cimagf(fft->input[p]) - yt);
                fft->input[p] = (crealf(fft->input[p]) + xt) + I * (cimagf(fft->input[p]) + yt);
                
            }
        }
        n = n>>1;
    }
    
    for (i = 0; i < k; i++) {
        fft->power[i] = cabsf(fft->input[i]);
        fft->totalPower += fft->power[i]/fft->nFFT;
    }
    
    fft->dBSPL = 10 * log10f(fft->totalPower + 1e-6) - P_REF;
    temp = fft->dBSPL;
    fft->dbpower = fft->dbpower + (temp - fft->dBpowerBuffer[0])/fft->framesPerSecond;
    memmove(fft->dBpowerBuffer, fft->dBpowerBuffer + 1, sizeof(*fft->dBpowerBuffer)*(fft->framesPerSecond-1));
    fft->dBpowerBuffer[fft->framesPerSecond -1] = temp;
}

void FFTwithoutWindow(Transform2* fft, float complex * input){
    int i,j,k,L,m,n,o,p,q;
    float tempReal, tempImaginary, cos, sin,xt, yt, temp;
    k = fft->nFFT;
    fft->totalPower = 0;
    
    for (i = 0; i < k; i++) {
        fft->input[i] = 0.0 + 0.0*I;
    }
    
    for (i = 0; i < fft->windowSize; i++) {
        fft->input[i] = input[i];
    }
    
    j = 0;
    m = k/2;
    
    //bit reversal
    for (i = 1; i<(k-1);i++) {
        
        L = m;
        
        while(j>=L) {
            j = j-L;
            L = L/2;
        }
        
        j = j + L;
        
        if(i<j) {
            tempReal = crealf(fft->input[i]);
            tempImaginary = cimagf(fft->input[i]);
            
            fft->input[i] = fft->input[j];
            
            fft->input[j] = tempReal + tempImaginary * I;
        }
    }
    
    
    L = 0;
    m = 1;
    n = k/2;
    
    //computation
    for (i = k; i > 1; i = (i>>1)) {
        L = m;
        m = 2*m;
        o = 0;
        
        for (j = 0; j < L; j++) {
            cos = fft->cosine[o];
            sin = fft->sine[o];
            o = o+n;
            
            for (p = j; p < k; p = p + m) {
                q = p + L;
                
                xt = cos * crealf(fft->input[q]) - sin * cimagf(fft->input[q]);
                yt = sin * crealf(fft->input[q]) + cos * cimagf(fft->input[q]);
                
                fft->input[q] = (crealf(fft->input[p]) - xt) + I * (cimagf(fft->input[p]) - yt);
                fft->input[p] = (crealf(fft->input[p]) + xt) + I * (cimagf(fft->input[p]) + yt);
                
            }
        }
        n = n>>1;
    }
    
    for (i = 0; i < k; i++) {
        fft->power[i] = cabsf(fft->input[i]);
        fft->totalPower += fft->power[i]/fft->nFFT;
    }
    
    fft->dBSPL = 10 * log10f(fft->totalPower + 1e-6) - P_REF;
    temp = fft->dBSPL;
    fft->dbpower = fft->dbpower + (temp - fft->dBpowerBuffer[0])/fft->framesPerSecond;
    memmove(fft->dBpowerBuffer, fft->dBpowerBuffer + 1, sizeof(*fft->dBpowerBuffer)*(fft->framesPerSecond-1));
    fft->dBpowerBuffer[fft->framesPerSecond -1] = temp;
}

void IFFT(Transform2* fft){
    
    int i,j,k,L,m,n,o,p,q;
    float tempReal, tempImaginary, cos, sin, xt, yt;
    k = fft->nFFT;
    
    j = 0;
    m=k/2;
    
    //bit reversal
    for (i = 1; i<(k-1);i++) {
        
        L = m;
        
        while(j>=L) {
            j = j-L;
            L = L/2;
        }
        
        j = j + L;
        
        if(i<j) {
            tempReal = crealf(fft->input[i]);
            tempImaginary = cimagf(fft->input[i]);
            
            fft->input[i] = fft->input[j];
            
            fft->input[j] = tempReal + tempImaginary * I;
        }
    }
    
    L=0;
    m=1;
    n=k/2;
    
    //computation
    for (i = k; i > 1; i = (i>>1)) {
        L = m;
        m = 2*m;
        o = 0;
        
        for (j = 0; j < L; j++) {
            cos = fft->cosine[o];
            sin = -fft->sine[o];
            o = o+n;
            
            for (p = j; p < k; p = p + m) {
                q = p + L;
                
                xt = cos * crealf(fft->input[q]) - sin * cimagf(fft->input[q]);
                yt = sin * crealf(fft->input[q]) + cos * cimagf(fft->input[q]);
                
                fft->input[q] = (crealf(fft->input[p]) - xt) + I * (cimagf(fft->input[p]) - yt);
                fft->input[p] = (crealf(fft->input[p]) + xt) + I * (cimagf(fft->input[p]) + yt);
                
            }
        }
        n = n>>1;
    }
    
    for (i=0; i<k; i++) {
        fft->input[i] /= k;
    }
}


void destroyTransform2(Transform2** transform){
    if (*transform != NULL) {
        if((*transform)->cosine != NULL){
            free((*transform)->cosine);
            (*transform)->cosine = NULL;
        }
        if((*transform)->sine != NULL){
            free((*transform)->sine);
            (*transform)->sine = NULL;
        }
        if ((*transform)->input != NULL) {
            free((*transform)->input);
            (*transform)->input = NULL;
        }
        if((*transform)->power != NULL){
            free((*transform)->power);
            (*transform)->power = NULL;
        }
        if((*transform)->dBpowerBuffer != NULL){
            free((*transform)->dBpowerBuffer);
            (*transform)->dBpowerBuffer = NULL;
        }
        if((*transform)->window != NULL){
            free((*transform)->window);
            (*transform)->window = NULL;
        }
        free(*transform);
        *transform = NULL;
    }
}
