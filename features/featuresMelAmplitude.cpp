//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresMelAmplitude.h"

#define NB_MEL_BANDS    70

vector<string> FeaturesMelAmplitude::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back(m_name);
    return depandancies;
}

void FeaturesMelAmplitude::analysis()
{
    
}

variant FeaturesMelAmplitude::addition(PopulationPtr population)
{
    vector<SolutionPtr>     solutions = population->getSolutions();
    vector<matrixPtr>       MelStructure(solutions.size());
    matrixPtr               curMel, solSpec;
    int                     nbInstr, i, j, k, l, curOn;
    
    #pragma omp parallel for private(curMel,solSpec,nbInstr,j,k,l,curOn) shared(MelStructure,solutions,population)
    for (i = 0; i < solutions.size(); i++)
    {
        if (solutions[i]->getIsComputed())
        {
            solSpec = solutions[i]->getSolutionFeature(m_name);
            #pragma omp critical
            MelStructure[i] = (solSpec);
        }
        else
        {
            solSpec = matrixPtr(new matrix(NB_MEL_BANDS, 64));
            nbInstr = solutions[i]->getNbInstruments();
            for (j = 0; j < nbInstr; j++)
            {
                if (solutions[i]->getIndividual(j)->getInstrument() == sSession->getKnowledge()->getNeutralID())
                    continue;
                curOn = floor(solutions[i]->getIndividual(j)->getOnset() / 2);
                curMel = solutions[i]->getIndividual(j)->getFeature(m_name);
                for (k = 0; k < NB_MEL_BANDS; k++)
                    for (l = curOn; l < 64; l++)
                        solSpec->operator()(k, l) += curMel->operator()(k, l);
            }
            #pragma omp critical
            {
                solutions[i]->setSolutionFeature(m_name, solSpec);
                MelStructure[i] = (solSpec);
            }
        }
    }
    population->setSolutions(solutions);
    return MelStructure;
}

matrixPtr FeaturesMelAmplitude::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int         i;
    matrixPtr   criteria(new matrix((int)boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(), 1));
    
    #pragma omp parallel for shared(soundsetFeatures,criteria)
    for (i = 0; i < boost::get<vector<matrixPtr> >(soundsetFeatures[m_name]).size(); i++)
    {
        matrixPtr tmpSlice_S_B = boost::get<vector<matrixPtr> >(soundsetFeatures[m_name])[i];;
        matrixPtr tmpSlice_T_B = targetFeatures.melbands.melAmplitude;
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

void FeaturesMelAmplitude::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesMelAmplitude::neutral()
{
    return matrixPtr(new matrix(NB_MEL_BANDS, 64));
}