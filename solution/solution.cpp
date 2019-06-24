//
//  solution.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "solution.h"

Solution::Solution(Session* sessObj, int size)
{
    sSession = sessObj;
    isComputed = false;
    isLocallyOptimized = false;
    individualsSet = vector<IndividualPtr>(size);
    individualsID = vector<int>(size);
}

Solution::Solution(const Solution &sol)
{
    sSession        = sol.sSession;
    isComputed      = sol.isComputed;
    isLocallyOptimized = sol.isLocallyOptimized;
    setIndividuals(sol.individualsSet);
    mixFeatures     = sol.mixFeatures;
    mixCriteria     = sol.mixCriteria;
}

void Solution::setIndividual(int id, IndividualPtr iValue, int idInst)
{
    individualsSet[id]  = iValue;
    individualsID[id]   = idInst;
    isComputed = false;
}

void Solution::setIndividuals(const vector<IndividualPtr>& indivSet)
{
    individualsSet.clear();
    individualsID.clear();
    //individualsSet.resize(indivSet.size(), Individual(0, 0));
    //for (i = 0; i < indivSet.size(); i++)
        //individualsSet.push_back(IndividualPtr(new Individual(0, 0)));
    //individualsID.reserve(indivSet.size());
    int i;
    for (i = 0; i < indivSet.size(); i++)
    {
        //individualsSet[i] = indivSet[i];
        individualsSet.push_back(indivSet[i]);
        individualsID.push_back(indivSet[i]->getInstrument());
    }
    isComputed = false;
}

void Solution::setSolutionFeature(string fName, matrixPtr fValue)
{
    mixFeatures[fName] = fValue;
}

void Solution::setFeatures(map<string, matrixPtr>& fSet)
{
    mixFeatures = fSet;
}

void Solution::setCriteria(matrixPtr cValue)
{
    mixCriteria = cValue;
}

void Solution::setComputed()
{
    isComputed = true;
}

void Solution::setLocallyOptimized()
{
    isLocallyOptimized = true;
}

bool Solution::getLocallyOptimized()
{
    return isLocallyOptimized;
}

void Solution::emptyIndividuals()
{
    individualsSet.clear();
    individualsID.clear();
    mixFeatures.clear();
    //mixCriteria->deallocate();
    isComputed = false;
}

int Solution::getNbInstruments()
{
    return (int)individualsSet.size();
}

IndividualPtr Solution::getIndividual(int id)
{
    return individualsSet[id];
}

vector<IndividualPtr> &Solution::getIndividuals()
{
    return individualsSet;
}

vector<int> &Solution::getIndividualsID()
{
    return individualsID;
}

matrixPtr Solution::getIndividualFeature(string fName)
{
    int i, j, k;
    matrixPtr mFeat;
    matrixPtr curFeat = individualsSet[0]->getFeature(fName);
    if (curFeat->rows() == 1)
    {
        mFeat = matrixPtr(new matrix(curFeat->size(), (int)individualsSet.size()));
        for (i = 0; i < individualsSet.size(); i++)
        {
            curFeat = individualsSet[i]->getFeature(fName);
            for (k = 0; k < mFeat->rows(); k++)
                mFeat->operator()(k, i) = curFeat->operator()(0, k);
        }
    }
    else
    {
        mFeat = matrixPtr(new matrix(curFeat->rows(), getNbInstruments() * curFeat->cols()));
        for (i = 0; i < individualsSet.size(); i++)
        {
            curFeat = individualsSet[i]->getFeature(fName);
            for (j = 0; j < curFeat->rows(); j++)
                for (k = 0; k < curFeat->cols(); k++)
                    mFeat->operator()(j, k + i * curFeat->cols()) = curFeat->operator()(j, k);
        }
    }
    return mFeat;
}

map<string, matrixPtr> &Solution::getSolutionFeatures()
{
    return mixFeatures;
}

matrixPtr Solution::getSolutionFeature(string fName)
{
    return mixFeatures.at(fName);
}

matrixPtr Solution::getSolutionCriteria()
{
    return mixCriteria;
}

bool Solution::getIsComputed()
{
    return isComputed;
}

void Solution::dumpSolution(string filename)
{
    boost::filesystem::path output(filename);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int i, j;
    /* Create the directory, read/write/access owner only. */
    map<string, matrixPtr>::iterator it;
    for (it = mixFeatures.begin(); it != mixFeatures.end(); ++it)
    {
        FILE *f = fopen((filename + "_" + it->first + ".txt").c_str(), "w");
        for (i = 0; i < it->second->rows(); i++)
        {
            for (j = 0; j < it->second->cols(); j++)
                fprintf(f, "%f ", it->second->operator()(i, j));
            fprintf(f, "\n");
        }
        fclose(f);
    }
    /*
    FILE *f = fopen((filename + ".criteria.txt").c_str(), "w");
    for (i = 0; i < mixCriteria->rows(); i++)
    {
        for (j = 0; j < mixCriteria->cols(); j++)
            fprintf(f, "%f ", mixCriteria->operator()(i, j));
        fprintf(f, "\n");
    }
    fclose(f);
    */

    for (i = 0; i < individualsSet.size(); i++)
        individualsSet[i]->dumpIndividual(filename + "/ins" + to_string(i));
}

