//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresPartialsMeanAmplitude.h"
#include <omp.h>

vector<string> FeaturesPartialsMeanAmplitude::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back(m_name);
    depandancies.push_back("PartialsMeanFrequency");
    return depandancies;
}

void FeaturesPartialsMeanAmplitude::analysis()
{
    
}

variant                     FeaturesPartialsMeanAmplitude::addition(PopulationPtr population)
{
    int                     nbInstr, i, j, l;
    matrixPtr               sFeat;
    matrixPtr               curFreq, curAmps, curEner;
    vector<SolutionPtr>     solutions = population->getSolutions();
    vector<matrixPtr>       partialMeanStructure (solutions.size());
    
    #pragma omp parallel for private(sFeat,curFreq,curAmps,curEner,nbInstr,j,l) shared(partialMeanStructure,solutions,population)
    for (i = 0; i < population->getNbSolutions(); i++)
    {
        if (solutions[i]->getIsComputed())
        {
            sFeat = solutions[i]->getSolutionFeature(m_name);
            #pragma omp critical
            partialMeanStructure[i] = (sFeat);
        }
        else
        {
            sFeat = matrixPtr(new matrix(1, maxFreq));
            nbInstr = solutions[i]->getNbInstruments();
            for (j = 0; j < nbInstr; j++)
            {
                if (solutions[i]->getIndividual(j)->getInstrument() == sSession->getKnowledge()->getNeutralID())
                    continue;
                curAmps = solutions[i]->getIndividual(j)->getFeature("PartialsMeanAmplitude");
                for (l = 0; l < curAmps->size(); l++)
                    sFeat->operator()(l) += curAmps->operator()(l);
            }
            #pragma omp critical
            {
                solutions[i]->setSolutionFeature("PartialsMeanAmplitude", sFeat);
                partialMeanStructure[i] = (sFeat);
            }
        }
    }
    population->setSolutions(solutions);
    return partialMeanStructure;
}

matrixPtr FeaturesPartialsMeanAmplitude::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int i;
    if (!targetFeatures.targetMeanSpec)
    {
        float  *target_amp  = targetFeatures.partials.partialsMeanAmplitude;
        float  *target_freq  = targetFeatures.partials.partialsMeanFrequency;
        matrixPtr targetSpec(new matrix(maxFreq, 1));
        for (i = 0; i < 25; i++)
        {
            if (target_freq[i] > 20 && target_freq[i] < 22000 && target_amp[i] > 0)
                targetSpec->operator()(round(90.0f + (24.0f * log2f(target_freq[i] / 220.0f)))) = target_amp[i];
        }
        //targetSpec->normalize();
        targetFeatures.targetMeanSpec = targetSpec;
    }
    matrixPtr criteria(new matrix((int)boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(), 1));
    
    #pragma omp parallel for shared(soundsetFeatures,criteria)
    for (i = 0; i < boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(); i++)
    {
        matrixPtr tmpSlice_S_B = boost::get<vector<matrixPtr> >(soundsetFeatures[m_name])[i];
        matrixPtr tmpSlice_T_B = targetFeatures.targetMeanSpec;
        float val;
        switch (0)
        {
            case 0:
                val = vectorPartials(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 1:
                val = vectorCorrelation(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 2:
                val = vectorKullbackLeibler(tmpSlice_T_B, tmpSlice_S_B);
                break;
            case 3:
                val = vectorDTW(tmpSlice_T_B, tmpSlice_S_B);
                break;
            default:
                val = vectorPartials(tmpSlice_T_B, tmpSlice_S_B);
        }
        #pragma omp critical
        criteria->operator()(i) = val;
    }
    return criteria;
}

void FeaturesPartialsMeanAmplitude::transpose(IndividualPtr individual)
{
    int         j;
    int         microtonality = individual->getMicrotonality();
    int         resolution = sSession->getProduction()->getResolution();
    matrixPtr   curMeanAmps;
    
    if (microtonality != 0 && resolution > 1)// && ((float)microtonality / (float)resolution) >= 0.5)
    {
        curMeanAmps = individual->getFeature(m_name);
        // Shift colums to microtonality times
        matrixPtr newCurMeanAmps(new matrix(maxFreq, 1));
//        int index = 0;
        for (j = 0; j < (maxFreq)-(int)(microtonality * (2.0f / (float)resolution)); j++)
            newCurMeanAmps->operator()(j + (int)(microtonality * (2.0f / (float)resolution))) = curMeanAmps->operator()(j);
        individual->setFeature(m_name, newCurMeanAmps);
    }
}

matrixPtr FeaturesPartialsMeanAmplitude::neutral()
{
    return matrixPtr(new matrix(maxFreq, 1));
}