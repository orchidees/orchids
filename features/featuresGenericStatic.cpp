//
//  features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//


#include "featuresGenericStatic.h"

vector<string>          FeaturesGenericStatic::depandancy()
{
    vector<string> depandancies;
    depandancies.push_back("EnergyEnvelopeMean");
    depandancies.push_back(m_name);
    return depandancies;
}

void                    FeaturesGenericStatic::analysis()
{
    
}

variant                 FeaturesGenericStatic::addition(PopulationPtr population)
{
    int                 i, j;
    matrixPtr           sFeatures(new matrix(population->getNbSolutions(), 1));
    vector<SolutionPtr> solutions = population->getSolutions();
    float               curDesc, curEner, totDesc, totEner;
    int                 nbIns;
    
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
                curDesc = solutions[i]->getIndividual(j)->getFeature(m_name)->operator()(0);
                curEner = solutions[i]->getIndividual(j)->getFeature("EnergyEnvelopeMean")->operator()(0);
                totDesc += curDesc * curEner;
                totEner += curEner;
            }
            matrixPtr sFeat(new matrix(1, 1));
            if (totEner)
                sFeat->operator()(0) = totDesc / totEner;
            else
                sFeat->operator()(0) = 0;
            sFeatures->operator()(i, 0) = sFeat->operator()(0);
            solutions[i]->setSolutionFeature(m_name, sFeat);
        }
    }
    population->setSolutions(solutions);
    return sFeatures;
}

matrixPtr                  FeaturesGenericStatic::compare(features& soundsetFeatures, sqlStruct& targetFeatures)
{
    int                 i;
    matrixPtr              sc_vector_T;
    try { sc_vector_T = sSession->getTarget()->getFeature(m_name); }
    catch (const char* e) { throw e; }
    matrixPtr fDistance(new matrix(boost::get<matrixPtr>(soundsetFeatures[m_name])->rows(), 1));
    for (i = 0; i < fDistance->rows(); i++)
        fDistance->operator()(i, 0) = abs(boost::get<matrixPtr>(soundsetFeatures[m_name])->operator()(i, 0) - sc_vector_T->operator()(0));
    return fDistance;
}

void FeaturesGenericStatic::transpose(IndividualPtr individual)
{
    
}

matrixPtr FeaturesGenericStatic::neutral()
{
    return matrixPtr(new matrix(1, 1));
}