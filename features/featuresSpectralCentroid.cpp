//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresSpectralCentroid.h"


vector<string> FeaturesSpectralCentroid::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("Loudness");
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesSpectralCentroid::analysis()
{
    
}

variant FeaturesSpectralCentroid::addition(PopulationPtr population)
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
            matrixPtr descriptor   = solutions[i]->getIndividualFeature(m_name);
            matrixPtr energyMatrix = solutions[i]->getIndividualFeature("Loudness");
            float *ener_sum = energyMatrix->getSum(2);
            descriptor->operator*(energyMatrix);
            float *sum = descriptor->getSum(2);
            matrixPtr sFeat(new matrix(1, descriptor->rows()));
            for (j = 0; j < descriptor->rows(); j++)
                sFeat->operator()(j) = sum[j] / ener_sum[j];
            for (j = 0; j < sFeat->size(); j++)
                if (isnan(sFeat->operator()(j)))
                    sFeat->operator()(j) = 0;
            sFeat->normalize();
            for (j = 0; j < nbTime; j++)
                sFeatures->operator()(i, j) = sFeat->operator()(j);
            solutions[i]->setSolutionFeature(m_name, sFeat);
            free(ener_sum);
            free(sum);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;
}

matrixPtr FeaturesSpectralCentroid::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int         i, j;
    matrixPtr   targetFeat;
    try { targetFeat = sSession->getTarget()->getFeature(m_name); }
    catch (const char* e) { throw e; }
    matrixPtr   soundsetMat = boost::get<matrixPtr>(soundsetFeatures[m_name]);
    matrixPtr temporalCriteria(new matrix(soundsetMat->rows(), 1));
    
    #pragma omp parallel for shared(soundsetMat, temporalCriteria)
    for (i = 0; i < soundsetMat->rows(); i++)
    {
        matrixPtr tmpSlice_S_B = matrixPtr(new matrix(128, 1));
        for (j = 0; j < 128; j++)
            tmpSlice_S_B->operator()(j) = soundsetMat->operator()(i, j);
        #pragma omp critical
        temporalCriteria->operator()(i) = vectorDTW(tmpSlice_S_B, targetFeat);
        //free(str1);
    }
    return temporalCriteria;
}

void FeaturesSpectralCentroid::transpose(IndividualPtr individual)
{
    int                 resolution      = sSession->getProduction()->getResolution();
    int                 microtonality   = individual->getMicrotonality();
    float               transpo;
    if (microtonality != 0)
    {
        transpo = 100.0f * (float)microtonality / (float)resolution;
        transpo = pow(2, transpo / 1200);
        individual->getFeature(m_name)->operator*(transpo);
    }
}

matrixPtr FeaturesSpectralCentroid::neutral()
{
    return matrixPtr(new matrix(1, 128));
}