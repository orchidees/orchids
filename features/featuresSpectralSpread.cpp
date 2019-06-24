//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresSpectralSpread.h"


vector<string> FeaturesSpectralSpread::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("Loudness");
    depandancies.push_back("SpectralCentroid");
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesSpectralSpread::analysis()
{
    
}

variant FeaturesSpectralSpread::addition(PopulationPtr population)
{
    int nbTime, i, j, k;
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
            matrixPtr centroMatrix = solutions[i]->getIndividualFeature("SpectralCentroid");
            float *ener_sum = energyMatrix->getSum(2);
            centroMatrix->operator*(energyMatrix);
            descriptor->operator^(2);
            float *sum = centroMatrix->getSum(2);
            for (j = 0; j < descriptor->rows(); j++)
            {
                sum[j] = sum[j] / ener_sum[j];
                for (k = 0; k < descriptor->cols(); k++)
                    descriptor->operator()(j, k) = descriptor->operator()(j, k) + pow(sum[j], 2);
            }
            descriptor->operator*(energyMatrix);
            float *m2sum = descriptor->getSum(2);
            matrixPtr sFeat(new matrix(1, descriptor->rows()));
            for (j = 0; j < sFeat->size(); j++)
            {
                sFeat->operator()(j) = sqrt((m2sum[j] / ener_sum[j]) - pow(sum[j], 2));
                if (isnan(sFeat->operator()(j)))
                    sFeat->operator()(j) = 0;
            }
            sFeat->normalize();
            for (j = 0; j < nbTime; j++)
                sFeatures->operator()(i, j) = sFeat->operator()(0, j);
            solutions[i]->setSolutionFeature(m_name, sFeat);
            free(ener_sum);
            free(m2sum);
            free(sum);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;

}

matrixPtr FeaturesSpectralSpread::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
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
        //tmpSlice_S_B->print();
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

void FeaturesSpectralSpread::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesSpectralSpread::neutral()
{
    return matrixPtr(new matrix(1, 128));
}