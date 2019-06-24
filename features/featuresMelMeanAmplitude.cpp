//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "featuresMelMeanAmplitude.h"

#define NB_MEL_BANDS    70

vector<string> FeaturesMelMeanAmplitude::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesMelMeanAmplitude::analysis()
{
    
}

variant                     FeaturesMelMeanAmplitude::addition(PopulationPtr population)
{
    int                     nbInstr, i, j, l;
    matrixPtr               curMeanMel;
    matrixPtr               solSpec;
    vector<SolutionPtr>     solutions = population->getSolutions();
    vector<matrixPtr>       MelMeanStructure(solutions.size());
    
    #pragma omp parallel for private(curMeanMel,solSpec,nbInstr,j,l) shared(MelMeanStructure,solutions,population)
    for (i = 0; i < population->getNbSolutions(); i++)
    {
        if (solutions[i]->getIsComputed())
        {
            solSpec = solutions[i]->getSolutionFeature(m_name);
            #pragma omp critical
            MelMeanStructure[i] = (solSpec);
        }
        else
        {
            solSpec = matrixPtr(new matrix(NB_MEL_BANDS, 1));
            nbInstr = solutions[i]->getNbInstruments();
            for (j = 0; j < nbInstr; j++)
            {
                if (solutions[i]->getIndividual(j)->getInstrument() == sSession->getKnowledge()->getNeutralID())
                    continue;
                curMeanMel = solutions[i]->getIndividual(j)->getFeature(m_name);
                for (l = 0; l < curMeanMel->size(); l++)
                    solSpec->operator()(l) += curMeanMel->operator()(l);
            }
            #pragma omp critical
            {
                solutions[i]->setSolutionFeature(m_name, solSpec);
                MelMeanStructure[i] = (solSpec);
            }
        }
    }
    population->setSolutions(solutions);
    return MelMeanStructure;
}

matrixPtr FeaturesMelMeanAmplitude::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int         i;
    matrixPtr   criteria(new matrix((int)boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(), 1));
    
    #pragma omp parallel for shared(soundsetFeatures,criteria)
    for (i = 0; i < boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(); i++)
    {
        matrixPtr tmpSlice_S_B = boost::get<vector<matrixPtr> >(soundsetFeatures[m_name])[i];
        matrixPtr tmpSlice_T_B = targetFeatures.melbands.melMeanAmp;
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

void FeaturesMelMeanAmplitude::transpose(IndividualPtr individual)
{

}

matrixPtr FeaturesMelMeanAmplitude::neutral()
{
    return matrixPtr(new matrix(NB_MEL_BANDS, 1));
}