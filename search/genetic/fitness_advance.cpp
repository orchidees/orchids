//
//  fitness_advance.cpp
//  Orchids
//
//  Created by Philippe Esling on 01/11/2014.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "fitness_advance.h"
#include <math.h>

float       fitness_advance(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights)
{
    float   *meanCrit, res;
    int     i;
    
    meanCrit = newPopCriteria->getMean(1);
    for (res = 0, i = 0; i < idealCriteria.size(); i++)
        res += weights[i] * ((meanCrit[i] - idealCriteria[i]) / nadirCriteria[i]);
    res /= (float)idealCriteria.size();
    free(meanCrit);
    return ((1 - res) < 0) ? 0 : (1 - res);
}

vector<float>           compute_mutation_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights)
{
    vector<float>       probas;
    vector<float>       newBest;
    float               *meanCrit, res;
    int                 i, sol;
    
    probas = vector<float>(newPopCriteria->rows());
    meanCrit = newPopCriteria->getMean(1);
    newBest = newPopCriteria->Min(1);
    for (sol = 0; sol < newPopCriteria->rows(); sol++)
    {
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += (meanCrit[i] - newPopCriteria->operator()(sol, i));
        if (res < 0) { probas[sol] = 0.05; continue; }
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += ((((idealCriteria[i] - meanCrit[i]) / nadirCriteria[i])) /
                    (((idealCriteria[i] - newPopCriteria->operator()(sol, i)) / nadirCriteria[i])));
        res *= 0.05 / (float)idealCriteria.size();
        probas[sol] = MAX(MIN(res, 0.25), 0.05);
    }
    free(meanCrit);
    return probas;
}

vector<float>           compute_cross_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights)
{
    vector<float>       probas;
    vector<float>       newBest;
    float               *meanCrit, res;
    int                 i, sol;
    
    probas = vector<float>(newPopCriteria->rows());
    meanCrit = newPopCriteria->getMean(1);
    newBest = newPopCriteria->Min(1);
    for (sol = 0; sol < (newPopCriteria->rows() - 1); sol += 2)
    {
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += (meanCrit[i] - min(newPopCriteria->operator()(sol, i), newPopCriteria->operator()(sol + 1, i)));
        if (res < 0) { probas[sol] = 1; probas[sol + 1] = 1; continue; }
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += ((((min(newPopCriteria->operator()(sol, i), newPopCriteria->operator()(sol + 1, i)) - idealCriteria[i]) / nadirCriteria[i])) /
                    (((meanCrit[i] - idealCriteria[i]) / nadirCriteria[i])));
        res /= (float)idealCriteria.size();
        res = MIN(res * 1.25, 1);
        probas[sol] = res;
        probas[sol + 1] = res;
    }
    free(meanCrit);
    return probas;
}

/*
 
 vector<float>           compute_mutation_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights)
 {
 vector<float>       probas;
 vector<float>       newBest;
 float               *meanCrit, res;
 int                 i, sol;
 
 probas = vector<float>(newPopCriteria->rows());
 meanCrit = newPopCriteria->getMean(1);
 newBest = newPopCriteria->Min(1);
 for (sol = 0; sol < newPopCriteria->rows(); sol++)
 {
 for (res = 0, i = 0; i < idealCriteria.size(); i++)
 res += weights[i] * (meanCrit[i] - newPopCriteria->operator()(sol, i));
 if (res < 0) { probas[sol] = 0.05; continue; }
 for (res = 0, i = 0; i < idealCriteria.size(); i++)
 res += ((weights[i] * ((idealCriteria[i] - meanCrit[i]) / nadirCriteria[i])) /
 (weights[i] * ((idealCriteria[i] - newPopCriteria->operator()(sol, i)) / nadirCriteria[i])));
 res *= 0.05 / (float)idealCriteria.size();
 probas[sol] = MAX(MIN(res, 0.25), 0.05);
 }
 free(meanCrit);
 return probas;
 }
 
vector<float>           compute_cross_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights)
{
    vector<float>       probas;
    vector<float>       newBest;
    float               *meanCrit, res;
    int                 i, sol;
    
    probas = vector<float>(newPopCriteria->rows());
    meanCrit = newPopCriteria->getMean(1);
    newBest = newPopCriteria->Min(1);
    for (sol = 0; sol < (newPopCriteria->rows() - 1); sol += 2)
    {
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += weights[i] * (meanCrit[i] - min(newPopCriteria->operator()(sol, i), newPopCriteria->operator()(sol + 1, i)));
        if (res < 0) { probas[sol] = 1; probas[sol + 1] = 1; continue; }
        for (res = 0, i = 0; i < idealCriteria.size(); i++)
            res += ((weights[i] * ((min(newPopCriteria->operator()(sol, i), newPopCriteria->operator()(sol + 1, i)) - idealCriteria[i]) / nadirCriteria[i])) /
                    (weights[i] * ((meanCrit[i] - idealCriteria[i]) / nadirCriteria[i])));
        res /= (float)idealCriteria.size();
        probas[sol] = res;
        probas[sol + 1] = res;
    }
    free(meanCrit);
    return probas;
}
*/