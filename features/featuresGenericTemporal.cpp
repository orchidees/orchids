//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresGenericTemporal.h"


vector<string> FeaturesGenericTemporal::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("EnergyEnvelope");
    depandancies.push_back("Loudness");
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesGenericTemporal::analysis()
{
    
}

variant                 FeaturesGenericTemporal::addition(PopulationPtr population)
{
    int                 nbTime, i, j;
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
            matrixPtr descriptor   = solutions[i]->getIndividualFeature(m_name);
            matrixPtr energymatrixPtr = solutions[i]->getIndividualFeature("Loudness");
            float *ener_sum = energymatrixPtr->getSum(2);
            descriptor->operator*(energymatrixPtr);
            float *sum = descriptor->getSum(2);
            matrixPtr sFeat(new matrix(1, descriptor->rows()));
            for (j = 0; j < descriptor->rows(); j++)
                sFeat->operator()(0, j) = sum[j] / ener_sum[j];
            for (j = 0; j < sFeat->size(); j++)
                if (isnan(sFeat->operator()(j)))
                    sFeat->operator()(j) = 0;
            sFeat->normalize();
            for (j = 0; j < nbTime; j++)
                sFeatures->operator()(i, j) = sFeat->operator()(0, j);
            solutions[i]->setSolutionFeature(m_name, sFeat);
            free(ener_sum);
            free(sum);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;
}

matrixPtr                  FeaturesGenericTemporal::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
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
        matrixPtr tmpSlice_S_B = matrixPtr(new matrix(128, 1));
        for (j = 0; j < 128; j++)
            tmpSlice_S_B->operator()(j) = soundsetMat->operator()(i, j);
        temporalCriteria->operator()(i) = vectorDTW(tmpSlice_S_B, targetFeat);
        //free(str1);
    }
    //free(U);
    //free(L);
    return temporalCriteria;
}

void FeaturesGenericTemporal::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesGenericTemporal::neutral()
{
    return matrixPtr(new matrix(1, 128));
}