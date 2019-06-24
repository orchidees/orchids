//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresLoudness.h"


vector<string> FeaturesLoudness::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesLoudness::analysis()
{
    
}

variant FeaturesLoudness::addition(PopulationPtr population)
{
    int nbTime, i, j;
    try { nbTime = sSession->getTarget()->getFeature(m_name)->size(); }
    catch (const char* e) { throw e; }
    matrixPtr              sFeatures(new matrix(population->getNbSolutions(), nbTime));
    vector<SolutionPtr> solutions = population->getSolutions();
    
    for (i = 0; i < population->getNbSolutions(); i++)
    {
        if (solutions[i]->getIsComputed())
        {
            matrixPtr sFeat = solutions[i]->getSolutionFeature(m_name);
            for (j = 0; j < nbTime; j++)
                sFeatures->operator()(i, j) = sFeat->operator()(0, j);
        }
        else
        {
            matrixPtr   descriptor = solutions[i]->getIndividualFeature(m_name);
            float       *temporalmatrix = descriptor->getSum(2);
            for (j = 0; j < descriptor->rows(); j++)
                if (isnan(temporalmatrix[j]))
                    temporalmatrix[j] = 0;
            matrixPtr   m(new matrix(temporalmatrix, true, 1, descriptor->rows()));
            m->normalize();
            for (j = 0; j < nbTime; j++)
                sFeatures->operator()(i, j) = m->operator()(j);
            solutions[i]->setSolutionFeature(m_name, m);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;
}

matrixPtr FeaturesLoudness::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int     i, j;
    matrixPtr  targetFeat;
    try { targetFeat = sSession->getTarget()->getFeature(m_name); }
    catch (const char* e) { throw e; }
    matrixPtr soundsetMat = boost::get<matrixPtr>(soundsetFeatures[m_name]);
    /*
     float reach = floor(0.05 * 128);
     float *U    = (float*)malloc(targetFeat->size() * sizeof(float));
     float *L    = (float*)malloc(targetFeat->size() * sizeof(float));
     for (i = 0; i < targetFeat->size(); i++)
     {
     U[i] = _max(targetFeat->operator()(_max(i - reach, 1) - 1), targetFeat->operator()(_min(i + reach, targetFeat->size()) - 1));
     L[i] = _min(targetFeat->operator()(_max(i - reach, 1) - 1), targetFeat->operator()(_min(i + reach, targetFeat->size()) - 1));
     }
     */
    matrixPtr temporalCriteria(new matrix(soundsetMat->rows(), 1));
    
    for (i = 0; i < soundsetMat->rows(); i++)
    {
        matrixPtr tmpSlice_S_B = matrixPtr(new matrix(1, 128));
        for (j = 0; j < 128; j++)
            tmpSlice_S_B->operator()(j) = soundsetMat->operator()(i, j);
        /*
         float* str1 = soundsetMat->getRow(i);
         vector<float> v1, v2, vFinal;
         for (j = 0; j < soundsetMat->cols(); j++)
         {
         (str1[j] > U[j]) ? v1.push_back(1) : v1.push_back(0);
         (str1[j] < L[j]) ? v2.push_back(1) : v2.push_back(0);
         }
         for (j = 0; j < v1.size(); j++)
         {
         v1[j] = pow(v1[j] * (str1[j] - U[j]), 2);
         v2[j] = pow(v2[j] * (L[j] - str1[j]), 2);
         vFinal.push_back(sqrt(v1[j] + v2[j]));
         }
         temporalCriteria->operator()(i) = getMean(&vFinal[0], (int)vFinal.size());
         */
        temporalCriteria->operator()(i) = vectorDTW(tmpSlice_S_B, targetFeat);
        //free(str1);
    }
    //free(U);
    //free(L);
    return temporalCriteria;
}

void FeaturesLoudness::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesLoudness::neutral()
{
    return matrixPtr(new matrix(1, 128));
}