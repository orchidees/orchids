//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresSpectralSpreadMean.h"


vector<string>          FeaturesSpectralSpreadMean::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("EnergyEnvelopeMean");
    depandancies.push_back("SpectralCentroidMean");
    depandancies.push_back(m_name);
    return depandancies;
}

void                    FeaturesSpectralSpreadMean::analysis()
{
    
}

variant                 FeaturesSpectralSpreadMean::addition(PopulationPtr population)
{
    matrixPtr           sFeatures(new matrix(population->getNbSolutions(), 1));
    vector<SolutionPtr> solutions = population->getSolutions();
    float               curDesc, curCentro, curEner, totDesc, totEner;
    int                 nbIns, i, j;
    
    for (i = 0; i < population->getNbSolutions(); i++)
    {
        if (solutions[i]->getIsComputed())
            sFeatures->operator()(i, 0) = solutions[i]->getSolutionFeature(m_name)->operator()(0, 0);
        else
        {
            nbIns = solutions[i]->getNbInstruments();
            totDesc = 0; totEner = 0;
            for (j = 0; j < nbIns; j++)
            {
                curDesc = solutions[i]->getIndividual(j)->getFeature("SpectralCentroidMean")->operator()(0);
                curEner = solutions[i]->getIndividual(j)->getFeature("EnergyEnvelopeMean")->operator()(0);
                totDesc += curDesc * curEner;
                totEner += curEner;
            }
            curCentro = totDesc / totEner;
            for (j = 0, curDesc = 0, totDesc = 0; j < nbIns; j++)
            {
                curDesc = solutions[i]->getIndividual(j)->getFeature(m_name)->operator()(0);
                curEner = solutions[i]->getIndividual(j)->getFeature("EnergyEnvelopeMean")->operator()(0);
                curDesc = pow(curDesc, 2) + pow(curCentro, 2);
                if (curEner > 0)
                    totDesc += (curDesc / curEner);
            }
            matrixPtr sFeat(new matrix(1, 1));
            if (totEner)
                sFeat->operator()(0) = sqrt((totDesc / totEner) - pow(curCentro, 2));
            else
                sFeat->operator()(0) = 0;
            sFeatures->operator()(i, 0) = sFeat->operator()(0);
            solutions[i]->setSolutionFeature(m_name, sFeat);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;
}

matrixPtr                  FeaturesSpectralSpreadMean::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int                     i;
    matrixPtr               sc_vector_T;
    
    try { sc_vector_T = sSession->getTarget()->getFeature(m_name); }
    catch (const char* e) { throw e; }
    matrixPtr fDistance(new matrix(boost::get<matrixPtr>(soundsetFeatures[m_name])->rows(), 1));
    for (i = 0; i < fDistance->rows(); i++)
        fDistance->operator()(i, 0) = abs(boost::get<matrixPtr>(soundsetFeatures[m_name])->operator()(i, 0) - sc_vector_T->operator()(0));
    return fDistance;
}

void                    FeaturesSpectralSpreadMean::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesSpectralSpreadMean::neutral()
{
    return matrixPtr(new matrix(1, 1));
}