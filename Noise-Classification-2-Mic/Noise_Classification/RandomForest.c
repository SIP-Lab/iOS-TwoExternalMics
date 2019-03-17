/*
 * RandomForest.c
 *
 *  Created on: Apr 17, 2015
 *      Author: Carissa
 */

#include "RandomForest.h"
#include "TrainData.h"

RandomForests* initRandomForest() {
	RandomForests* newRandomForest;

	newRandomForest = (RandomForests*)malloc(sizeof(RandomForests));
	newRandomForest->nTrees = nTrees;
	newRandomForest->nClasses = nClasses;
	newRandomForest->classDecision = -1;
	newRandomForest->treeVotes = (int*)malloc(nTrees*sizeof(int));
    newRandomForest->scores = (float*)malloc(nClasses*sizeof(float));


	return newRandomForest;
}

const char* returnClassLabel(int classIndex) {
    
    if (classIndex < 1) {
        return classLabels[0];
    }
    else{
        return classLabels[classIndex];
    }
}

void evalTrees(RandomForests* RandomForest, float* inputFeatureList) {
	RandomForests* rf = RandomForest;
	int i;
	int current_node;
	int cvar;
	//int tree_output[nTrees];
	int classVotes[nClasses];
	int max;
	//int mismatch_count=0;
    //float newNormalizedClassDecision;

	// Initialize vote counts to zero
	for(i=0;i<nClasses;i++)
	{
		classVotes[i]=0;
	}

    for(i = 0; i<rf->nTrees ;i++)
    {
        current_node = 0;
        while (childnode[i][current_node]!=0)
        {
            cvar = nodeCutVar[i][current_node];
            //if (inputFeatureList[i + (cvar-1)*M] < nodeCutValue[i][current_node])
            if( (inputFeatureList[cvar-1]) < nodeCutValue[i][current_node])
            	current_node = childnode[i][current_node]-1;
            else current_node = childnode[i][current_node];
        }
        rf->treeVotes[i] = nodelabel[i][current_node]; // for debug
        classVotes[(nodelabel[i][current_node])-1]++;
    }

    // Check which class received the most votes
    max=classVotes[0];
    rf->classDecision = 1;
    rf->scores[0] = (float)classVotes[0]/(float)nTrees;
    //rf->classDecisionCount[0]=classVotes[0];
    for(i=1;i<rf->nClasses;i++)
    {
        rf->scores[i] = (float)classVotes[i]/(float)nTrees;
    	//rf->classDecisionCount[i]=classVotes[i];
    	if(classVotes[i]>max)
    	{
    		max=classVotes[i];
    		rf->classDecision=i+1;
    	}
    }
    

//	// Multiply the class decision by 1/N
//	newNormalizedClassDecision = rf->Normalize * (float)rf->classDecision;
//	// Remove the oldest decision from the average
//	rf->floatAverageClassDecision -= rf->classDecisionBuffer[rf->oldestClassDecision];
//	// Add the newest decision to the average
//	rf->floatAverageClassDecision += newNormalizedClassDecision;
//	// Store the newest cbn to the periodicity buffer
//	rf->classDecisionBuffer[rf->oldestClassDecision]=newNormalizedClassDecision;
//
//	// Round the floating point average class to an integer class
//	rf->averageClassDecision = (int) (rf->floatAverageClassDecision + 0.5);
//	// Update the pointer in the decision buffer
//	if (rf->oldestClassDecision < rf->bufferLength-1)
//		rf->oldestClassDecision++;
//	else
//		rf->oldestClassDecision=0;
}

void destroyRandomForest(RandomForests** rf) {
	if((*rf)->treeVotes != NULL){
		free((*rf)->treeVotes);
		(*rf)->treeVotes = NULL;
	}
//	if((*rf)->classDecisionCount != NULL){
//		free((*rf)->classDecisionCount);
//		(*rf)->classDecisionCount = NULL;
//	}
	if(*rf != NULL){
		free(*rf);
		*rf=NULL;
	}
}
