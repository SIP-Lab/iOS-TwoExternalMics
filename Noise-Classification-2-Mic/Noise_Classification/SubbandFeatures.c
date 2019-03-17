//
//  SubbandFeatures.c
//  algorithm
//
//  Created by Abhishek Sehgal on 5/15/17.
//  Copyright © 2017 default. All rights reserved.
//

#include "SubbandFeatures.h"

// Change bands in power of 2
const int NBANDS = 8;
const int NHISTBINS = 10;


SubbandFeatures* initSubbandFeatures(int nFFT, int decisionBufferLength) {
    
    SubbandFeatures* newSubbandFeatures = (SubbandFeatures*) malloc(sizeof(SubbandFeatures));
    
    newSubbandFeatures->nBands          = NBANDS;
    newSubbandFeatures->nFFT            = nFFT;
    newSubbandFeatures->normalize       = 1.0/decisionBufferLength;
    newSubbandFeatures->subbandWidth    = nFFT/(2*NBANDS);
    newSubbandFeatures->histBins        = NHISTBINS;
    newSubbandFeatures->decisionBufferLength = decisionBufferLength;
    
    
    newSubbandFeatures->currentFrame    = (float complex*)malloc(nFFT * sizeof(float complex));
    newSubbandFeatures->previousFrame   = (float complex*)malloc(nFFT * sizeof(float complex));
    
    newSubbandFeatures->histCount       = (int*)calloc(sizeof(int), NHISTBINS);
    newSubbandFeatures->subbandPower    = (float*)calloc(sizeof(float), newSubbandFeatures->subbandWidth);
    
    newSubbandFeatures->currentSubbands         = (Transform2**)malloc(sizeof(Transform2*) * NBANDS);
    newSubbandFeatures->previousSubbands        = (Transform2**)malloc(sizeof(Transform2*) * NBANDS);
    newSubbandFeatures->periodicityTransform    = (Transform2**)malloc(sizeof(Transform2*) * NBANDS);
    
    for (size_t i = 0; i < NBANDS; i++) {
        newSubbandFeatures->currentSubbands[i]      = initTransform(newSubbandFeatures->subbandWidth, 1, 2* newSubbandFeatures->subbandWidth);
        newSubbandFeatures->previousSubbands[i]     = initTransform(newSubbandFeatures->subbandWidth, 1, 2* newSubbandFeatures->subbandWidth);
        newSubbandFeatures->periodicityTransform[i] = initTransform(2*newSubbandFeatures->subbandWidth, 1, 2* newSubbandFeatures->subbandWidth);
    }
    
    newSubbandFeatures->subbandFeatureList      = (float*)calloc(sizeof(float), 2 * NBANDS);
    newSubbandFeatures->bandPeriodicity         = newSubbandFeatures->subbandFeatureList;
    newSubbandFeatures->bandEntropy             = newSubbandFeatures->subbandFeatureList + NBANDS;
    
    newSubbandFeatures->bandEntropyBuffer       = (float**)malloc(sizeof(float*) * NBANDS);
    newSubbandFeatures->bandPeriodicityBuffer   = (float**)malloc(sizeof(float*) * NBANDS);
    
    for (size_t i = 0; i < NBANDS; i++) {
        
        newSubbandFeatures->bandPeriodicityBuffer[i] = (float*)calloc(sizeof(float), decisionBufferLength);
        newSubbandFeatures->bandEntropyBuffer[i]     = (float*)calloc(sizeof(float), decisionBufferLength);
        
    }
    
    return newSubbandFeatures;
    
}

void computeSubbandFeatures(SubbandFeatures* sbf, Transform2* fft, int *isFirstFrame){
    
    float histMin, histMax, max, temp, currentSum, previousSum, histWidth, histSum;
    
    memcpy(sbf->currentFrame, fft->input, sizeof(float complex) * fft->nFFT);
    if (*isFirstFrame == 1) {
        memcpy(sbf->previousFrame, fft->input, sizeof(float complex) * fft->nFFT);
        (*isFirstFrame) = 0;
    }
    
    // Calculating Band Periodicity
    for (size_t i = 0; i < NBANDS; i++) {
        
        max = 0;
        histMin = 1;
        histMax = 0;
        histSum = 0;
        currentSum = 0;
        previousSum = 0;
        
        for (size_t n = 0; n < sbf->subbandWidth; n++) {
            sbf->subbandPower[n] = cabsf(sbf->currentFrame[n + (i * sbf->subbandWidth)]);
            histMax = (histMax > sbf->subbandPower[n] ? histMax : sbf->subbandPower[n]);
            histMin = (histMin < sbf->subbandPower[n] ? histMin : sbf->subbandPower[n]);
            
            currentSum += powf(sbf->subbandPower[n],2);
            previousSum += powf(cabsf(sbf->previousFrame[n + (i * sbf->subbandWidth)]),2);
        }
        
        // Calculating Band Periodicity
        
        FFTwithoutWindow(sbf->currentSubbands[i],  sbf->currentFrame + (i * sbf->subbandWidth));
        FFTwithoutWindow(sbf->previousSubbands[i], sbf->previousFrame + (i * sbf->subbandWidth));
        
        for (size_t n = 0; n < sbf->periodicityTransform[i]->nFFT; n++) {
            sbf->periodicityTransform[i]->input[n] = sbf->currentSubbands[i]->input[n] * conjf(sbf->previousSubbands[i]->input[n]);
        }
        
        IFFT(sbf->periodicityTransform[i]);
        
        for (size_t n = 0; n < sbf->periodicityTransform[i]->nFFT; n++) {
            temp = cabsf(sbf->periodicityTransform[i]->input[n]);
            max = (max > temp ? max : temp);
        }
        max /= (sqrtf(currentSum * previousSum) + 1e-10);
        
        sbf->bandPeriodicity[i] = sbf->bandPeriodicity[i] + (max - sbf->bandPeriodicityBuffer[i][0])*sbf->normalize;
        memcpy(sbf->bandPeriodicityBuffer[i], sbf->bandPeriodicityBuffer[i] + 1,
               sizeof(sbf->bandPeriodicityBuffer[i][0]) * (sbf->decisionBufferLength - 1));
        sbf->bandPeriodicityBuffer[i][sbf->decisionBufferLength - 1] = max;
        
        
        // Calculating Band Entropy
        
        histWidth = (histMax - histMin)/NHISTBINS;
        memset(sbf->histCount, 0, NHISTBINS * sizeof(sbf->histCount[0]));
        
        for (size_t n = 0; n < sbf->subbandWidth; n++) {
            size_t k;
            for (k = 0; k < NHISTBINS && sbf->subbandPower[n] >= (histMin + k*histWidth); k++);
            ++sbf->histCount[k-1];
        }
        
        for (size_t n = 0; n < NHISTBINS; n++) {
            histSum += sbf->histCount[n] * logf(sbf->histCount[n] + 1e-10);
        }
        histSum /= sbf->subbandWidth;
        
        sbf->bandEntropy[i] = sbf->bandEntropy[i] + (histSum - sbf->bandEntropyBuffer[i][0])*sbf->normalize;
        memcpy(sbf->bandEntropyBuffer[i], sbf->bandEntropyBuffer[i] + 1,
               sizeof(sbf->bandEntropyBuffer[i][0]) * sbf->decisionBufferLength - 1);
        sbf->bandEntropyBuffer[i][sbf->decisionBufferLength - 1] = histSum;
    }
    
  
    memcpy(sbf->previousFrame, sbf->currentFrame, sizeof(float complex) * fft->nFFT);
    
}

void destroySubbandFeatures(SubbandFeatures** sbf) {
    
    if (*sbf != NULL) {
        
        if((*sbf)->currentFrame != NULL){
            free((*sbf)->currentFrame);
            (*sbf)->currentFrame = NULL;
        }
        
        if((*sbf)->previousFrame != NULL){
            free((*sbf)->previousFrame);
            (*sbf)->previousFrame = NULL;
        }
                
        if((*sbf)->subbandPower != NULL){
            free((*sbf)->subbandPower);
            (*sbf)->subbandPower = NULL;
        }
        
        if((*sbf)->histCount != NULL){
            free((*sbf)->histCount);
            (*sbf)->histCount = NULL;
        }
        
        if ((*sbf)->currentSubbands != NULL) {
            for (size_t i =0; i < NBANDS; i++) {
                if ((*sbf)->currentSubbands[i] != NULL){
                    free((*sbf)->currentSubbands[i]);
                    (*sbf)->currentSubbands[i] = NULL;
                }
            }
            
            free((*sbf)->currentSubbands);
            (*sbf)->currentSubbands = NULL;
        }
        
        if ((*sbf)->previousSubbands != NULL) {
            for (size_t i =0; i < NBANDS; i++) {
                if ((*sbf)->previousSubbands[i] != NULL){
                    free((*sbf)->previousSubbands[i]);
                    (*sbf)->previousSubbands[i] = NULL;
                }
            }
            
            free((*sbf)->previousSubbands);
            (*sbf)->previousSubbands = NULL;
        }
        
        if ((*sbf)->periodicityTransform != NULL) {
            for (size_t i =0; i < NBANDS; i++) {
                if ((*sbf)->periodicityTransform[i] != NULL){
                    free((*sbf)->periodicityTransform[i]);
                    (*sbf)->periodicityTransform[i] = NULL;
                }
            }
            
            free((*sbf)->periodicityTransform);
            (*sbf)->periodicityTransform = NULL;
        }
        
        if ((*sbf)->bandPeriodicityBuffer != NULL) {
            for (size_t i =0; i < NBANDS; i++) {
                if ((*sbf)->bandPeriodicityBuffer[i] != NULL){
                    free((*sbf)->bandPeriodicityBuffer[i]);
                    (*sbf)->bandPeriodicityBuffer[i] = NULL;
                }
            }
            
            free((*sbf)->bandPeriodicityBuffer);
            (*sbf)->bandPeriodicityBuffer = NULL;
        }
        
        if ((*sbf)->bandEntropyBuffer != NULL) {
            for (size_t i =0; i < NBANDS; i++) {
                if ((*sbf)->bandEntropyBuffer[i] != NULL){
                    free((*sbf)->bandEntropyBuffer[i]);
                    (*sbf)->bandEntropyBuffer[i] = NULL;
                }
            }
            
            free((*sbf)->bandEntropyBuffer);
            (*sbf)->bandEntropyBuffer = NULL;
        }
        
        if ((*sbf)->subbandFeatureList != NULL) {
            free((*sbf)->subbandFeatureList);
            (*sbf)->subbandFeatureList = NULL;
            (*sbf)->bandPeriodicity = NULL;
            (*sbf)->bandEntropy = NULL;
        }
        
        free((*sbf));
        (*sbf) = NULL;
        
    }
    
}
