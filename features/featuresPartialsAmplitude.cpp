//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresPartialsAmplitude.h"
#include <omp.h>

vector<string> FeaturesPartialsAmplitude::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("PartialsFrequency");
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesPartialsAmplitude::analysis()
{
    
}

variant FeaturesPartialsAmplitude::addition(PopulationPtr population)
{
    vector<SolutionPtr> solutions = population->getSolutions();
    vector<matrixPtr>   partialStructure (solutions.size());
    matrixPtr           solSpec, curFreq, curAmps, curEner;
    int                 nbInstr, i;
    int                 j, k, l, curOn;
    
    #pragma omp parallel for private(solSpec,curFreq,curAmps,curEner,nbInstr,j,k,l,curOn) shared(partialStructure,solutions,population)
    for (i = 0; i < solutions.size(); i++)
    {
        if (solutions[i]->getIsComputed())
        {
            solSpec = solutions[i]->getSolutionFeature(m_name);
            #pragma omp critical
            partialStructure[i] = solSpec;
        }
        else
        {
            solSpec = matrixPtr(new matrix(maxFreq, 64));
            nbInstr = solutions[i]->getNbInstruments();
            for (j = 0; j < nbInstr; j++)
            {
                if (solutions[i]->getIndividual(j)->getInstrument() == sSession->getKnowledge()->getNeutralID())
                    continue;
                curOn = floor(solutions[i]->getIndividual(j)->getOnset() / 2);
                curAmps = solutions[i]->getIndividual(j)->getFeature("PartialsAmplitude");
                for (l = 0; l < maxFreq; l++)
                {
                    for (k = curOn; k < 64; k++)
                        solSpec->operator()(l, k) += curAmps->operator()(l, k);
                }
            }
            #pragma omp critical
            {
                solutions[i]->setSolutionFeature("PartialsAmplitude", solSpec);
                partialStructure[i] = solSpec;
            }
        }
    }
    population->setSolutions(solutions);
    return partialStructure;
}

matrixPtr FeaturesPartialsAmplitude::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int i, j;
    if (!targetFeatures.targetSpec)
    {
        matrixPtr target_amp   = targetFeatures.partials.partialsAmplitude;
        matrixPtr target_freq  = targetFeatures.partials.partialsFrequency;
        /*
        for (i = 0; i < target_freq->rows(); i++)
            for (j = 0; j < target_freq->cols(); j++)
                if (target_freq->operator()(i, j) < 0)
                    target_freq->operator()(i, j) = 0;
         */
        matrixPtr targetSpec(new matrix(maxFreq, target_freq->cols()));
        for (i = 0; i < target_freq->rows(); i++)
            for (j = 0; j < target_freq->cols(); j++)
                if (target_amp->operator()(i, j) > 0 && target_freq->operator()(i, j) > 20 && target_freq->operator()(i, j) < 22000)
                    targetSpec->operator()(round(90.0f + (24.0f * log2f(target_freq->operator()(i, j) / 220.0f))), j) += target_amp->operator()(i, j);
        //targetSpec->normalize();
        targetFeatures.targetSpec = targetSpec;
    }
    matrixPtr criteria(new matrix((int)boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(), 1));
    
    #pragma omp parallel for shared(soundsetFeatures,criteria)
    for (i = 0; i < boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(); i++)
    {
        matrixPtr tmpSlice_S_B = boost::get<vector<matrixPtr> >(soundsetFeatures[m_name])[i];
        matrixPtr tmpSlice_T_B = targetFeatures.targetSpec;
        float val;
        switch (0)
        {
            case 0:
                val = matrixPartials(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 1:
                val = matrixCorrelation(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 2:
                val = matrixKullbackLeibler(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 3:
                val = matrixDTW(tmpSlice_T_B, tmpSlice_S_B);
                break;
            default:
                val = matrixPartials(tmpSlice_T_B, tmpSlice_S_B);
        }
        #pragma omp critical
        criteria->operator()(i) = val;
    }
    return criteria;
}

void FeaturesPartialsAmplitude::transpose(IndividualPtr individual)
{
    int                 j, k;
    int                 microtonality = individual->getMicrotonality();
    int                 resolution = sSession->getProduction()->getResolution();
    matrixPtr           curAmps, newCurAmps;
    
    if (microtonality != 0 && resolution > 1)// && ((float)microtonality / (float)resolution) >= 0.5)
    {
        curAmps = individual->getFeature(m_name);
        // Shift colums to microtonality times
        newCurAmps = matrixPtr(new matrix(curAmps->rows(), curAmps->cols()));
        for (j = 0; j < curAmps->rows() - (int)(microtonality * (2.0f / (float)resolution)); j++)
            for (k = 0; k < curAmps->cols(); k++)
                newCurAmps->operator()(j + (int)(microtonality * (2.0f / (float)resolution)), k) = curAmps->operator()(j, k);
        individual->setFeature(m_name, newCurAmps);
    }
}

matrixPtr FeaturesPartialsAmplitude::neutral()
{
    return matrixPtr(new matrix(maxFreq, 64));
}