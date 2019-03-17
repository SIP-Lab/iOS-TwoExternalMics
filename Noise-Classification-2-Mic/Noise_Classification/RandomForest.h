/*
 * RandomForest.h
 *
 *  Created on: Apr 17, 2015
 *      Author: Carissa
 */

#ifndef RANDOMFOREST_H_
#define RANDOMFOREST_H_
#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <math.h>

typedef struct RandomForests {
	int     nTrees;				 // Number of trees
	int     nClasses;            // Number of noise classes
	int     classDecision;       // Classifier output result
	int*  	treeVotes;           // class vote from each tree for debug
    float* scores;
} RandomForests;

/*!
 * Initializes the Random Forest Classifier
 * 
 * This function initializes the Random Forest Classifier and sets
 * the parameters for the different trees
 *
 * @return pointer to initialized Random Forest Classifier
 *
 */
RandomForests* initRandomForest();

/*!
 * Predicts the class based on the features provided
 *
 * This function accepts the feature vector and then classifies the vector.
 * The classification output is stored in the classDecision variable of
 * the random forest structure
 *
 * @param RandomForest pointer to initialized Random Forest Classifier
 * @param inputFeatureList The features based on which the classifier makes
 *                         a decision
 * 
 *
 */
void evalTrees(RandomForests* RandomForest, float* inputFeatureList);

const char* returnClassLabel(int classIndex);
void destroyRandomForest(RandomForests** rf);

#endif /* RANDOMFOREST_H_ */
