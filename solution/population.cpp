//
//  population.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "population.h"
#include "Orchids.h"
//#include "../search/genetic/eval_population.h"

Population::Population(Session* sessObj)
{
    sSession = sessObj;
}

void Population::setSession(Session* sessObj)
{
    sSession = sessObj;
}

void Population::setSolution(SolutionPtr sol)
{
    solutionSet.push_back(sol);
}

void Population::setSolutions(vector<SolutionPtr>& solSet)
{
    solutionSet.clear();
    for (int i = 0; i < solSet.size(); i++)
        solutionSet.push_back(solSet[i]);
}

void Population::setSolutionsIDs(matrixIPtr solIDs)
{
    //solutionIDs->deallocate();
    solutionIDs = solIDs;
}

void Population::setCriteria(matrixPtr critSet)
{
    //criteriaSet->deallocate();
    criteriaSet = critSet;
}

void Population::setFeature(string featName, matrixPtr featValue)
{
    featuresSet[featName] = featValue;
}

void Population::setFeatures(features& fSet)
{
    featuresSet.clear();
    featuresSet = fSet;
}

int Population::getNbSolutions()
{
    return (int)solutionSet.size();
}

vector<SolutionPtr> &Population::getSolutions()
{
    return solutionSet;
}

matrixPtr Population::getCriteria()
{
    return criteriaSet;
}

features &Population::getFeatures()
{
    return featuresSet;
}

matrixIPtr Population::getSolutionsIDs()
{
    return solutionIDs;
}

SolutionPtr Population::getSolution(int id)
{
    return solutionSet[id];
}

void Population::createNeutralPop()
{
    int                     nbIns  = (int)sSession->getProduction()->getInstruments().size();
    IndividualPtr           ntrInd = sSession->getProduction()->getVariableIndividuals()[sSession->getProduction()->getVariableIndividuals().size() - 1];
    SolutionPtr             sol(new Solution(sSession, nbIns));
    
    vector<IndividualPtr>   individuals;
    for (int i = 0; i < nbIns; i++)
        individuals.push_back(ntrInd);
    sol->setIndividuals(individuals);
    solutionSet.push_back(sol);
    criteriaSet = matrixPtr(new matrix(1, 1));
    solutionIDs = matrixIPtr(new matrixI(1, 1));
    solutionIDs->operator()(0, 0) = ntrInd->getInstrument();
}

PopulationPtr Population::extractPopulation(vector<int> &loc)
{
    int i, j, k;
    PopulationPtr newPop(new Population(sSession));
    
    for (i = 0; i < loc.size(); i++)
        newPop->solutionSet.push_back(SolutionPtr(new Solution(*solutionSet[loc[i]])));
    newPop->setSolutionsIDs(solutionIDs->reorder2(loc));
    newPop->setCriteria(criteriaSet->reorder2(loc));
    features fSet;
    features::iterator it;
    for (it = featuresSet.begin(); it != featuresSet.end(); ++it)
    {
        matrixPtr toExtract = boost::get<matrixPtr>(it->second);
        matrixPtr m(new matrix((int)loc.size(), toExtract->cols()));
        for (j = 0; j < m->rows(); j++)
            for (k = 0; k < m->cols(); k++)
                m->operator()(j, k) = toExtract->operator()(loc[j], k);
        fSet[it->first] = m;
    }
    newPop->setFeatures(fSet);
    return newPop;
}

void Population::combinePopulations(PopulationPtr newPop)
{
    int i;
    
    /*
    for (i = 0; i < newPop->solutionSet.size(); i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < newPop->solutionSet[i]->getNbInstruments(); j++)
            printf("%d ", newPop->solutionSet[i]->getIndividual(j)->getInstrument());
     printf("\n");
        solutionSet.push_back(SolutionPtr(new Solution(*newPop->solutionSet[i])));
         }
         */
    solutionSet.insert(solutionSet.end(), newPop->solutionSet.begin(), newPop->solutionSet.end());
    solutionIDs->mergeArray(newPop->getSolutionsIDs());
    criteriaSet->mergeArray(newPop->getCriteria());
    
    vector<FeaturesPtr> optFeatures = sSession->getFeatures();
    for (i = 0; i < optFeatures.size(); i++)
    {
        string curFeatName = optFeatures[i]->getFeatureName();
        // Test if key exists
        if (featuresSet.find(curFeatName) != featuresSet.end())
            boost::get<matrixPtr>(featuresSet[curFeatName])->mergeArray(boost::get<matrixPtr>(newPop->featuresSet[curFeatName]));
    }
}

void Population::generateRandomPopulation(int popsize, float popSparse)
{
    int i, j;
    vector<pair<string, vector<int> > > variableDomains;
    
    try {
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Get neutral element
    int ntrElem = sSession->getKnowledge()->getNeutralID();
    // Define sparsity level
    float sparsity = popSparse;
    
    // Get size of each variable domain
    matrixIPtr  dSize(new matrixI(popsize, (int)variableDomains.size()));
    vector<int> maxSize;
    for (i = 0; i < variableDomains.size(); i++)
        // Decrement dsize of 1 to account for neutral element
        maxSize.push_back((int)variableDomains[i].second.size() - 1);

    if (getMax(&maxSize[0], (int)maxSize.size()) == 0)
        throw "Population::generateRandomPopulation::ImpossibleOperation, Search space is empty !";
    
    // Draw random indices within dsize range
    vector<float> random_float = gen_random_float_vector<float>(0, 1, dSize->size());
    for (i = 0; i < dSize->rows(); i++)
        for (j = 0; j < dSize->cols(); j++)
            dSize->operator()(i, j) = ceil(random_float[dSize->cols() * i + j] * maxSize[j]);

    //for (i = 0; i < dSize.size(); i++)
        //if (dSize(i) == 0)
            //dSize(i) = 1;
    random_float.clear();

    // Replace indices by actual variable values
    for (i = 0; i < variableDomains.size(); i++)
        for (j = 0; j < dSize->rows(); j++)
            dSize->operator()(j, i) = variableDomains[i].second[dSize->operator()(j, i)];

    // Introduce neutral elements in population
    int nElements = dSize->size();
    vector<float> I = gen_random_float_vector<float>(0, nElements, round(nElements * sparsity));
    for (i = 0; i < I.size(); i++)
        dSize->c_data()[(int)I[i]] = ntrElem;

    solutionIDs = dSize;
}

void Population::generateConstrainedPopulation(int popsize, vector<matrixIPtr> &proposals, vector<int>& onsets)
{
    int i, j, k;
    vector<pair<string, vector<int> > > variableDomains;
    try {
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Get neutral element
    int ntrElem = sSession->getKnowledge()->getNeutralID();
    // Preparing production for onsets and features
    vector<vector<int> >    filteredOnsets      = sSession->getProduction()->getFilteredOnsets();
    vector<vector<int> >    filteredDurations   = sSession->getProduction()->getFilteredDurations();
    vector<vector<int> >    variableResolution  = sSession->getProduction()->getVariableResolution();
    vector<int>             variableTable       = sSession->getProduction()->getVariableTable();
    vector<int>             maxOnsets           = sSession->getProduction()->getMaxOnsetValues();
    vector<int>             maxDurations        = sSession->getProduction()->getMaxDurationValues();
    int                     resolution          = sSession->getProduction()->getResolution();
    vector<FeaturesPtr>     features            = sSession->getFeatures();
    // Define constrained level
    float constrained = 0.4;
    // Define sparsity level
    float sparsity = 0.4;

    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    
    // Get size of each variable domain
    matrixIPtr  dSize(new matrixI(popsize, (int)variableDomains.size()));
    vector<int> maxSize;
    for (i = 0; i < variableDomains.size(); i++)
        // Decrement dsize of 1 to account for neutral element
        maxSize.push_back((int)variableDomains[i].second.size() - 1);
    
    if (getMax(&maxSize[0], (int)maxSize.size()) == 0)
        throw "Population::generateConstrainedPopulation::ImpossibleOperation, Search space is empty !";
    
    // Draw random indices within dsize range
    vector<float> random_float = gen_random_float_vector<float>(0, 1, dSize->size());
    for (i = 0; i < dSize->rows(); i++)
        for (j = 0; j < dSize->cols(); j++)
            dSize->operator()(i, j) = ceil(random_float[dSize->cols() * i + j] * maxSize[j]);
    //for (i = 0; i < dSize.size(); i++)
    //if (dSize(i) == 0)
    //dSize(i) = 1;
    random_float.clear();
    
    // Replace indices by actual variable values
    matrixIPtr tmpOnsets(new matrixI(popsize, (int)variableDomains.size()));
    for (i = 0; i < variableDomains.size(); i++)
    {
        for (j = 0; j < dSize->rows(); j++)
            dSize->operator()(j, i) = variableDomains[i].second[dSize->operator()(j, i)];
        // Now introduce our constrained
        int nElems = dSize->rows();
        if (proposals[i]->size() != 0)
        {
            matrixIPtr consDomain = proposals[i];
            vector<float> cEL = gen_random_float_vector<float>(0, nElems, round(nElems * constrained));
            for (j = 0; j < cEL.size(); j++)
                cEL[j] = floor(cEL[j]);
            vector<float> cChoice = gen_random_float_vector<float>(0, consDomain->cols(), round(nElems * constrained));
            for (j = 0; j < cChoice.size(); j++)
                cChoice[j] = floor(cChoice[j]);
            for (j = 0; j < cEL.size(); j++)
            {
                dSize->operator()(cEL[j], i)        = consDomain->operator()(0, cChoice[j]);
                //tmpOnsets->operator()(cEL[j], i)    = consDomain->operator()(1, cChoice[j]);
                tmpOnsets->operator()(cEL[j], i)    = -1;
            }
        }
    }
    
    // Introduce neutral elements in population
    int nElements = dSize->size();
    vector<float> I = gen_random_float_vector<float>(0, nElements, round(nElements * sparsity));
    for (i = 0; i < I.size(); i++)
        dSize->c_data()[(int)I[i]] = ntrElem;
    
    solutionIDs = dSize;
    vector<IndividualPtr>   uInds;
    vector<int>             idSet;
    sSession->getProduction()->updateIndividualFeatures(solutionIDs, uInds, idSet);
    
    if (solutionSet.empty())
    {
        vector<SolutionPtr> tmpSolutionSet;
        for (i = 0; i < solutionIDs->rows(); i++)
            tmpSolutionSet.push_back(SolutionPtr(new Solution(sSession, (int)variableDomains.size())));
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < solutionIDs->cols(); j++)
            {
                int tmpOnset, index = 0;
                if (!sSession->getProduction()->isStatic())
                {
                    for (k = 0; k < variableTable.size(); k++)
                        if (variableTable[k] == solutionIDs->operator()(i, j))
                        {
                            index = k;
                            break;
                        }
                    if (tmpOnsets->operator()(i, j) == -1)
                    {
                        vector<int> onProp(onsets.begin(), onsets.end());
                        onProp.insert(onProp.end(), filteredOnsets[index].begin(), filteredOnsets[index].end());
                        if (onProp.empty())
                            onProp.push_back(floor(gen() * maxOnsets[index]));
                        tmpOnset = onProp[floor(gen() * onProp.size())];
                    }
                    else
                        tmpOnset = tmpOnsets->operator()(i, j);
                }
                else
                    tmpOnset = 0;
                
                IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset));
                int pos = 0;
                for (k = 0; k < idSet.size(); k++)
                    if (idSet[k] == solutionIDs->operator()(i, j))
                    {
                        pos = k;
                        break;
                    }
                map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                newIndiv->setFeatures(f);
                // Set Individual microtonality
                for (k = 0; k < variableTable.size(); k++)
                    if (variableTable[k] == solutionIDs->operator()(i, j))
                    {
                        index = k;
                        break;
                    }
                vector<int> curMicro = variableResolution[index];
                newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                newIndiv->computeFinalFeatures();
                // Apply tranposition to population features
                for (k = 0; k < features.size(); k++)
                    features[k]->transpose(newIndiv);
                tmpSolutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
            }
        }
        setSolutions(tmpSolutionSet);
    }
    else
    {
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < variableDomains.size(); j++)
            {
                IndividualPtr curIndiv = solutionSet[i]->getIndividual(j);
                if (!curIndiv->getIsComputed())
                {
                    int tmpOnset, index = 0;
                    if (!sSession->getProduction()->isStatic())
                    {
                        for (k = 0; k < variableTable.size(); k++)
                            if (variableTable[k] == solutionIDs->operator()(i, j))
                            {
                                index = k;
                                break;
                            }
                        vector<int> onProp = filteredOnsets[index];
                        if (onProp.empty())
                            onProp.push_back(floor(gen() * maxOnsets[index]));
                        tmpOnset = onProp[floor(gen() * onProp.size())];
                    }
                    else
                        tmpOnset = 0;
                    
                    IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset));
                    int pos = 0;
                    for (k = 0; k < idSet.size(); k++)
                        if (idSet[k] == solutionIDs->operator()(i, j))
                        {
                            pos = k;
                            break;
                        }
                    map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                    newIndiv->setFeatures(f);
                    // Set Individual microtonality if adaptiveOptimization didn't do it
                    if (curIndiv->getMicrotonality() == -1)
                    {
                        // Set Individual microtonality
                        for (k = 0; k < variableTable.size(); k++)
                            if (variableTable[k] == solutionIDs->operator()(i, j))
                            {
                                index = k;
                                break;
                            }
                        vector<int> curMicro = variableResolution[index];
                        newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                    }
                    else
                        newIndiv->setMicrotonality(curIndiv->getMicrotonality());
                    newIndiv->computeFinalFeatures();
                    // Apply tranposition to population features
                    for (k = 0; k < features.size(); k++)
                        features[k]->transpose(newIndiv);
                    solutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
                }
            }
        }
    }
}


void Population::generateConstrainedPopulation(int popsize, vector<matrixIPtr> &proposals, vector<int>& onsets, vector<int>& durations, float popCons, float popSparse)
{
    int i, j, k;
    vector<pair<string, vector<int> > > variableDomains;
    try {
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Get neutral element
    int ntrElem = sSession->getKnowledge()->getNeutralID();
    // Preparing production for onsets and features
    vector<vector<int> >    filteredOnsets      = sSession->getProduction()->getFilteredOnsets();
    vector<vector<int> >    filteredDurations   = sSession->getProduction()->getFilteredDurations();
    vector<vector<int> >    variableResolution  = sSession->getProduction()->getVariableResolution();
    vector<int>             variableTable       = sSession->getProduction()->getVariableTable();
    vector<int>             maxOnsets           = sSession->getProduction()->getMaxOnsetValues();
    vector<int>             maxDurations        = sSession->getProduction()->getMaxDurationValues();
    int                     resolution          = sSession->getProduction()->getResolution();
    vector<FeaturesPtr>     features            = sSession->getFeatures();
    // Define constrained level
    float constrained = popCons;
    // Define sparsity level
    float sparsity = popSparse;
    
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    
    // Get size of each variable domain
    matrixIPtr  dSize(new matrixI(popsize, (int)variableDomains.size()));
    vector<int> maxSize;
    for (i = 0; i < variableDomains.size(); i++)
        // Decrement dsize of 1 to account for neutral element
        maxSize.push_back((int)variableDomains[i].second.size() - 1);
    
    if (getMax(&maxSize[0], (int)maxSize.size()) == 0)
        throw "Population::generateConstrainedPopulation::ImpossibleOperation, Search space is empty !";
    
    // Draw random indices within dsize range
    vector<float> random_float = gen_random_float_vector<float>(0, 1, dSize->size());
    for (i = 0; i < dSize->rows(); i++)
        for (j = 0; j < dSize->cols(); j++)
            dSize->operator()(i, j) = ceil(random_float[dSize->cols() * i + j] * maxSize[j]);
    //for (i = 0; i < dSize.size(); i++)
    //if (dSize(i) == 0)
    //dSize(i) = 1;
    random_float.clear();
    
    // Replace indices by actual variable values
    matrixIPtr tmpOnsets(new matrixI(popsize, (int)variableDomains.size()));
    matrixIPtr tmpDurations(new matrixI(popsize, (int)variableDomains.size()));
    for (i = 0; i < variableDomains.size(); i++)
    {
        for (j = 0; j < dSize->rows(); j++)
            dSize->operator()(j, i) = variableDomains[i].second[dSize->operator()(j, i)];
        // Now introduce our constrained
        int nElems = dSize->rows();
        if (proposals[i]->size() != 0)
        {
            matrixIPtr consDomain = proposals[i];
            vector<float> cEL = gen_random_float_vector<float>(0, nElems, round(nElems * constrained));
            for (j = 0; j < cEL.size(); j++)
                cEL[j] = floor(cEL[j]);
            vector<float> cChoice = gen_random_float_vector<float>(0, consDomain->cols(), round(nElems * constrained));
            for (j = 0; j < cChoice.size(); j++)
                cChoice[j] = floor(cChoice[j]);
            for (j = 0; j < cEL.size(); j++)
            {
                dSize->operator()(cEL[j], i)        = consDomain->operator()(0, cChoice[j]);
                //tmpOnsets->operator()(cEL[j], i)    = consDomain->operator()(1, cChoice[j]);
                //tmpDurations->operator()(cEL[j], i) = consDomain->operator()(2, cChoice[j]);
                tmpOnsets->operator()(cEL[j], i)    = -1;
                tmpDurations->operator()(cEL[j], i)    = -1;
            }
        }
    }
    
    // Introduce neutral elements in population
    int nElements = dSize->size();
    vector<float> I = gen_random_float_vector<float>(0, nElements, round(nElements * sparsity));
    for (i = 0; i < I.size(); i++)
        dSize->c_data()[(int)I[i]] = ntrElem;
    
    solutionIDs = dSize;
    vector<IndividualPtr>   uInds;
    vector<int>             idSet;
    sSession->getProduction()->updateIndividualFeatures(solutionIDs, uInds, idSet);
    
    if (solutionSet.empty())
    {
        vector<SolutionPtr> tmpSolutionSet;
        for (i = 0; i < solutionIDs->rows(); i++)
            tmpSolutionSet.push_back(SolutionPtr(new Solution(sSession, (int)variableDomains.size())));
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < solutionIDs->cols(); j++)
            {
                int tmpOnset = 0, tmpDuration = 0, index = 0;
                if (!sSession->getProduction()->isStatic())
                {
                    for (k = 0; k < variableTable.size(); k++)
                        if (variableTable[k] == solutionIDs->operator()(i, j))
                        {
                            index = k;
                            break;
                        }
                    if (tmpOnsets->operator()(i, j) == -1)
                    {
                        vector<int> onProp;//(onsets.begin(), onsets.end());
                        vector<int> durProp;//(durations.begin(), durations.end());
                        onProp.insert(onProp.end(), filteredOnsets[index].begin(), filteredOnsets[index].end());
                        durProp.insert(durProp.end(), filteredDurations[index].begin(), filteredDurations[index].end());
                        if (onProp.empty())
                            onProp.push_back(floor(gen() * maxOnsets[index]));
                        if (durProp.empty())
                            durProp.push_back(floor(gen() * maxDurations[index]));
                        tmpOnset = onProp[floor(gen() * onProp.size())];
                        tmpDuration = durProp[floor(gen() * durProp.size())];
                    }
                    else
                    {
                        tmpOnset = tmpOnsets->operator()(i, j);
                        tmpDuration = tmpDurations->operator()(i, j);
                    }
                }
                else
                {
                    tmpOnset = 0;
                    tmpDuration = 0;
                }
                IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset, tmpDuration));
                int pos = 0;
                for (k = 0; k < idSet.size(); k++)
                    if (idSet[k] == solutionIDs->operator()(i, j))
                    {
                        pos = k;
                        break;
                    }
                map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                newIndiv->setFeatures(f);
                // Set Individual microtonality
                for (k = 0; k < variableTable.size(); k++)
                    if (variableTable[k] == solutionIDs->operator()(i, j))
                    {
                        index = k;
                        break;
                    }
                vector<int> curMicro = variableResolution[index];
                newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                newIndiv->computeFinalFeatures();
                // Apply tranposition to population features
                for (k = 0; k < features.size(); k++)
                    features[k]->transpose(newIndiv);
                tmpSolutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
            }
        }
        setSolutions(tmpSolutionSet);
    }
    else
    {
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < variableDomains.size(); j++)
            {
                IndividualPtr curIndiv = solutionSet[i]->getIndividual(j);
                if (!curIndiv->getIsComputed())
                {
                    int tmpOnset = 0, tmpDuration, index = 0;
                    if (!sSession->getProduction()->isStatic())
                    {
                        for (k = 0; k < variableTable.size(); k++)
                            if (variableTable[k] == solutionIDs->operator()(i, j))
                            {
                                index = k;
                                break;
                            }
                        vector<int> onProp = filteredOnsets[index];
                        vector<int> durProp = filteredDurations[index];
                        if (onProp.empty())
                            onProp.push_back(floor(gen() * maxOnsets[index]));
                        if (durProp.empty())
                            durProp.push_back(floor(gen() * maxDurations[index]));
                        tmpOnset = onProp[floor(gen() * onProp.size())];
                        tmpDuration = durProp[floor(gen() * durProp.size())];
                    }
                    else
                    {
                        tmpOnset = 0;
                        tmpDuration = 0;
                    }
                    IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset, tmpDuration));
                    int pos = 0;
                    for (k = 0; k < idSet.size(); k++)
                        if (idSet[k] == solutionIDs->operator()(i, j))
                        {
                            pos = k;
                            break;
                        }
                    map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                    newIndiv->setFeatures(f);
                    // Set Individual microtonality if adaptiveOptimization didn't do it
                    if (curIndiv->getMicrotonality() == -1)
                    {
                        for (k = 0; k < variableTable.size(); k++)
                            if (variableTable[k] == solutionIDs->operator()(i, j))
                            {
                                index = k;
                                break;
                            }
                        vector<int> curMicro = variableResolution[index];
                        newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                    }
                    else
                        newIndiv->setMicrotonality(curIndiv->getMicrotonality());
                    newIndiv->computeFinalFeatures();
                    // Apply tranposition to population features
                    for (k = 0; k < features.size(); k++)
                        features[k]->transpose(newIndiv);
                    solutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
                }
            }
        }
    }
}

void                                    Population::updatePopulationFeatures()
{
    int                                 i, j, idx;
    int                                 k, tabSize;
    vector<int>                         variableTable;
    vector<pair<string, vector<int> > > variableDomains;
    vector<vector<int> >                variableResolution;
    vector<vector<int> >                filteredOnsets;
    vector<vector<int> >                filteredDurations;
    try {
        variableTable   = sSession->getProduction()->getVariableTable();
        variableDomains = sSession->getProduction()->getVariableDomains();
        variableResolution = sSession->getProduction()->getVariableResolution();
        filteredOnsets  = sSession->getProduction()->getFilteredOnsets();
        filteredDurations  = sSession->getProduction()->getFilteredDurations();
    } catch (const char* e) {
        throw e;
    }
    tabSize = (int)variableTable.size();
    vector<IndividualPtr>   uInds;
    vector<int>             idSet;
    int                     resolution  = sSession->getProduction()->getResolution();
    vector<FeaturesPtr>     features    = sSession->getFeatures();
    sSession->getProduction()->updateIndividualFeatures(solutionIDs, uInds, idSet);
    
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    
    if (solutionSet.empty())
    {
        vector<SolutionPtr> tmpSolutionSet;
        for (i = 0; i < solutionIDs->rows(); i++)
            tmpSolutionSet.push_back(SolutionPtr(new Solution(sSession, (int)variableDomains.size())));
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < variableDomains.size(); j++)
            {
                int tmpOnset, tmpDuration;
                int index;
                if (!sSession->getProduction()->isStatic())
                {
                    for (k = 0; k < tabSize; k++)
                        if (variableTable[k] == solutionIDs->operator()(i, j))
                            { index = k; break; }
                    
                    if (k == tabSize)
                    {
                        tmpOnset = 0;
                        tmpDuration = 0;
                    }
                    else
                    {
                        vector<int> onsetVals, durationVals;
                        onsetVals = filteredOnsets[index];
                        durationVals = filteredDurations[index];
                        if (!onsetVals.empty())
                        {
                            tmpOnset = onsetVals[floor(gen() * onsetVals.size())];
                            tmpDuration = durationVals[floor(gen() * durationVals.size())];
                        }
                        else
                        {
                            tmpOnset = 0;
                            tmpDuration = 0;
                        }
                    }
                }
                else
                {
                    tmpOnset = 0;
                    tmpDuration = 0;
                }
                
                IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset, tmpDuration));
                int pos = 0;
                for (k = 0; k < idSet.size(); k++)
                    if (idSet[k] == solutionIDs->operator()(i, j))
                    {
                        pos = k;
                        break;
                    }
                map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                newIndiv->setFeatures(f);
                // Set Individual microtonality
                for (k = 0; k < variableTable.size(); k++)
                    if (variableTable[k] == solutionIDs->operator()(i, j))
                    {
                        idx = k;
                        break;
                    }
                vector<int> curMicro = variableResolution[idx];
                newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                newIndiv->computeFinalFeatures();
                // Apply tranposition to population features
                for (k = 0; k < features.size(); k++)
                    features[k]->transpose(newIndiv);
                tmpSolutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
            }
        }
        setSolutions(tmpSolutionSet);
    }
    else
    {
        for (i = 0; i < solutionIDs->rows(); i++)
        {
            for (j = 0; j < variableDomains.size(); j++)
            {
                IndividualPtr curIndiv = solutionSet[i]->getIndividual(j);
                if (!curIndiv->getIsComputed())
                {
                    int tmpOnset, tmpDuration;
                    int index;
                    if (!sSession->getProduction()->isStatic())
                    {
                        for (k = 0; k < tabSize; k++)
                        if (variableTable[k] == solutionIDs->operator()(i, j))
                            { index = k; break; }
                        if (curIndiv->getOnset() == -1)
                        {
                            if (k == tabSize)
                            {
                                tmpOnset = 0;
                                tmpDuration = 0;
                            }
                            else
                            {
                                vector<int> onsetVals, durationVals;
                                onsetVals = filteredOnsets[index];
                                durationVals = filteredDurations[index];
                                if (!onsetVals.empty())
                                {
                                    tmpOnset = onsetVals[floor(gen() * onsetVals.size())];
                                    tmpDuration = durationVals[floor(gen() * onsetVals.size())];
                                }
                                else
                                {
                                    tmpOnset = 0;
                                    tmpDuration = 0;
                                }
                            }
                        }
                        else
                        {
                            tmpOnset = curIndiv->getOnset();
                            tmpDuration = curIndiv->getDuration();
                        }
                    }
                    else
                    {
                        tmpOnset = 0;
                        tmpDuration = 0;
                    }
                    
                    IndividualPtr newIndiv = IndividualPtr(new Individual(solutionIDs->operator()(i, j), tmpOnset, tmpDuration));
                    int pos = 0;
                    for (k = 0; k < idSet.size(); k++)
                        if (idSet[k] == solutionIDs->operator()(i, j))
                        {
                            pos = k;
                            break;
                        }
                    map<string, matrixPtr> f = uInds[pos]->getOriginalFeatures();
                    newIndiv->setFeatures(f);
                    // Set Individual microtonality if adaptiveOptimization didn't do it
                    if (curIndiv->getMicrotonality() == -1)
                    {
                        
                        for (k = 0; k < variableTable.size(); k++)
                            if (variableTable[k] == solutionIDs->operator()(i, j))
                            {
                                idx = k;
                                break;
                            }
                        vector<int> curMicro = variableResolution[idx];
                        newIndiv->setMicrotonality(curMicro[floor(gen() * curMicro.size())]);
                    }
                    else
                        newIndiv->setMicrotonality(curIndiv->getMicrotonality());
                    newIndiv->computeFinalFeatures();
                    // Apply tranposition to population features
                    for (k = 0; k < features.size(); k++)
                        features[k]->transpose(newIndiv);
                    solutionSet[i]->setIndividual(j, newIndiv, solutionIDs->operator()(i, j));
                }
            }
        }
    }
}

void                    Population::localOptimization(sqlStruct& tFeatures)
{
    int                 i, j, k, sol, col;
    vector<SolutionPtr> newSolutions;
    matrixPtr              crit;
    
    // Get neutral element
    int ntrElem = sSession->getKnowledge()->getNeutralID();
    // Get number of instrument
    int nbIns = getSolution(0)->getNbInstruments();
    // Get features to optimize
    vector<FeaturesPtr> optFeatures = sSession->getFeatures();
    // Our vector of top instruments
    vector<vector<IndividualPtr> > topInstruments = vector<vector<IndividualPtr> >();
    // The complete individuals table
    vector<IndividualPtr> varIndiv = sSession->getProduction()->getVariableIndividuals();
    for (k = 0; k < nbIns; k++)
        topInstruments.push_back(vector<IndividualPtr>());
    for (i = 0; i < solutionSet.size(); i++)
    {
        PopulationPtr newPop = PopulationPtr(new Population(sSession));
        vector<int> keptInstruments;
        boost::shared_ptr<Solution> currentSol = solutionSet[i];
        newSolutions.push_back(SolutionPtr(new Solution(*currentSol)));
        if (currentSol->getLocallyOptimized())
            continue;
        // We create a new population
        newPop->solutionSet.push_back(currentSol);
        for (j = 0; j < nbIns; j++)
            if (currentSol->getIndividual(j)->getInstrument() != ntrElem)
            {
                keptInstruments.push_back(j);
                SolutionPtr newSol = SolutionPtr(new Solution(*currentSol));
                newSol->setIndividual(j, varIndiv[varIndiv.size() - 1], ntrElem);
                newPop->solutionSet.push_back(newSol);
            }
        matrixIPtr sIDs = matrixIPtr(new matrixI((int)newPop->solutionSet.size(), nbIns));
        for (j = 0; j < newPop->solutionSet.size(); j++)
            for (k = 0; k < nbIns; k++)
                sIDs->operator()(j, k) = newPop->solutionSet[j]->getIndividual(k)->getInstrument();
        newPop->setSolutionsIDs(sIDs);
        crit = eval_population(newPop, tFeatures, optFeatures);
        float *meanCrit = crit->getMean(2);
        float *diffCrit = (float *)calloc(newPop->getNbSolutions() - 1, sizeof(float));
        for (j = 1; j < newPop->getNbSolutions(); j++)
        {
            diffCrit[j - 1] = meanCrit[0] - meanCrit[j];
            if (diffCrit[j - 1] > 0)
            {
                SolutionPtr newSol = SolutionPtr(new Solution(*newPop->getSolution(j)));
                newSolutions.push_back(newSol);
            }
            if (diffCrit[j - 1] < 0 && currentSol->getIndividual(keptInstruments[j - 1])->getInstrument() != ntrElem)
                topInstruments[keptInstruments[j - 1]].push_back(currentSol->getIndividual(keptInstruments[j - 1]));
        }
        free(meanCrit);
        free(diffCrit);
        keptInstruments.clear();
        currentSol->setLocallyOptimized();
    }
    // Max number possible
    int maxNbSolutions = 1;
    for (k = 0; k < nbIns; k++)
    {
        if (topInstruments[k].size() == 0)
            topInstruments[k].push_back(varIndiv[varIndiv.size() - 1]);
        maxNbSolutions *= topInstruments[k].size();
    }
    if (maxNbSolutions < 0)
        maxNbSolutions = 100;
    maxNbSolutions = min(100, maxNbSolutions);
    // Now we can try to generate solutions based on topInstruments
    for (i = 0; i < maxNbSolutions; i++)
    {
        SolutionPtr newSol = SolutionPtr(new Solution(sSession, nbIns));
        vector<IndividualPtr> tmpSolSet1;
        for (k = 0; k < nbIns; k++)
        {
            IndividualPtr cur = topInstruments[k][(int)(rand() % topInstruments[k].size())];
            tmpSolSet1.push_back(cur);
        }
        newSol->setIndividuals(tmpSolSet1);
        newSolutions.push_back(newSol);
    }
    matrixIPtr newSolutionIDs = matrixIPtr(new matrixI((int)newSolutions.size(), nbIns));
    for (sol = 0; sol < newSolutionIDs->rows(); sol++)
        for (col = 0; col < newSolutionIDs->cols(); col++)
            newSolutionIDs->operator()(sol, col) = newSolutions[sol]->getIndividual(col)->getInstrument();
    setSolutionsIDs(newSolutionIDs);
    setSolutions(newSolutions);
}

/**
 *
 * @brief           Perform an harmonic fix on the existing population
 *
 *
 *
 *
PopulationPtr       Population::harmonicFix(sqlStruct& targetFeatures, int nbGen, int nbKept)
{
    int             i, j, k, r, curId, inst, h, so, sol, col, rF;
    PopulationPtr   t = PopulationPtr(new Population(sSession));
    // Retrieve the domains in which instruments can be chosen
    // Retrieve the variable table of indexes
    vector<int>                         variableTable;
    vector<pair<string, vector<int> > >   variableDomains;
    try {
        variableTable   = sSession->getProduction()->getVariableTable();
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Retrieve the list of optimization features
    vector<FeaturesPtr>         optFeatures     = sSession->getFeatures();
    int                         resolution      = sSession->getProduction()->getResolution();
    map<string, vector<int> >    onMap           = sSession->getProduction()->getVariableOnsetsDomains();
    map<string, vector<int> >    durMap          = sSession->getProduction()->getVariableDurationsDomains();
    vector<string>              instrus         = sSession->getProduction()->getInstruments();
    vector<SolutionPtr>         newSolutions;
    matrixIPtr                  newSolutionIDs;

    for (i = 0; i < optFeatures.size(); i++)
    {
        string curFName     = optFeatures[i]->getFeatureName();
        if (curFName == "PartialsMeanAmplitude")
        {
            printf("Harmonic fix ...\n");
            matrixPtr curFeature   = targetFeatures.targetMeanSpec;
            // We will ameliorate every solution based on partials
            for (k = 0; k < solutionSet.size(); k++)
            {
                // First we retrieve the partials of the solution
                matrixPtr solFeature = solutionSet[k]->getSolutionFeature(curFName);
                // And keep the maxVals and maxIDs
                vector<float> maxVals;
                for (r = 0; r < solFeature->cols(); r++)
                    maxVals.push_back(curFeature->operator()(0, r) - solFeature->operator()(0, r));
                vector<float>   solVals = vector<float>(solFeature->cols());
                vector<int>     solIds = vector<int>(solFeature->cols());
                vector<float>   solMidi = vector<float>(1);
                mySort(maxVals, solFeature->cols(), solVals, solIds);
                for (curId = (int)solIds.size() - 1; curId > (int)solIds.size() - (nbGen + 1); curId--)
                {
                    solMidi[0] = ((int)(solIds[curId] / 2) + 12);
                    int microTonality = floor((solIds[curId] % 2) * (resolution / 2));
                    if (solMidi[0] > 125)
                        continue;
                    vector<string> curNote = midi2mtnotes(solMidi);
                    for (inst = 0; inst < solutionIDs->cols(); inst++)
                    {
                        string key = instrus[inst] + "-" + curNote[0];
                        vector<int> proposals = onMap[key];
                        vector<int> propoDurs = durMap[key];
                        if (!proposals.empty())
                        {
                            vector<float> randomizer = gen_random_float_vector<float>(0, proposals.size(), nbGen);
                            for (h = 0; h < randomizer.size(); h++)
                            {
                                int index = floor(randomizer[h]);
                                SolutionPtr newSol = SolutionPtr(new Solution(sSession, solutionIDs->cols()));
                                IndividualPtr newIndiv = IndividualPtr(new Individual(proposals[index], -1));
                                newIndiv->setMicrotonality(microTonality);
                                for (so = 0; so < solutionIDs->cols(); so++)
                                    newSol->setIndividual(so, solutionSet[k]->getIndividual(so), solutionSet[k]->getIndividual(so)->getInstrument());
                                newSol->setIndividual(inst, newIndiv, proposals[index]);
                                newSolutions.push_back(newSol);
                            }
                        }
                    }
                }
            }
        }
    }
    newSolutionIDs = matrixIPtr(new matrixI((int)newSolutions.size(), solutionIDs->cols()));
    for (sol = 0; sol < newSolutionIDs->rows(); sol++)
        for (col = 0; col < newSolutionIDs->cols(); col++)
            newSolutionIDs->operator()(sol, col) = newSolutions[sol]->getIndividual(col)->getInstrument();
    t->setSolutions(newSolutions);
    t->setSolutionsIDs(newSolutionIDs);
    if (t->getNbSolutions() > nbKept)
    {
        vector<int> randomIDs;
        vector<float> randomIDs_f = gen_random_float_vector<float>(0, t->getNbSolutions(), nbKept);
        for (rF = 0; rF < randomIDs_f.size(); rF++)
            randomIDs.push_back(floor(randomIDs_f[rF]));
        vector<SolutionPtr> newRandomSolutions;
        for (i = 0; i < randomIDs.size(); i++)
            newRandomSolutions.push_back(t->solutionSet[randomIDs[i]]);
        t->setSolutionsIDs(t->solutionIDs->reorder2(randomIDs));
        t->setSolutions(newRandomSolutions);
    }
    return t;
}
*/

PopulationPtr       Population::harmonicFix(sqlStruct& targetFeatures, int nbGen, int nbKept)
{
    int             i, j, k, r, inst, h, so, sol, col, rF, maxI;
    float           maxV;
    
    PopulationPtr   t = PopulationPtr(new Population(sSession));
    // Retrieve the domains in which instruments can be chosen
    // Retrieve the variable table of indexes
    vector<int>                         variableTable;
    vector<pair<string, vector<int> > > variableDomains;
    try {
        variableTable   = sSession->getProduction()->getVariableTable();
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Retrieve the list of optimization features
    vector<FeaturesPtr>         optFeatures     = sSession->getFeatures();
    int                         resolution      = sSession->getProduction()->getResolution();
    map<string, vector<int> >   idMap           = sSession->getProduction()->getVariableOnsetsIDs();
    map<string, vector<int> >   onMap           = sSession->getProduction()->getVariableOnsetsDomains();
    map<string, vector<int> >   durMap          = sSession->getProduction()->getVariableDurationsDomains();
    vector<string>              instrus         = sSession->getProduction()->getInstruments();
    vector<SolutionPtr>         newSolutions;
    matrixIPtr                  newSolutionIDs;
    
    for (i = 0; i < optFeatures.size(); i++)
    {
        string curFName     = optFeatures[i]->getFeatureName();
        if (curFName == "PartialsMeanAmplitude")
        {
            printf("Harmonic fix ...\n");
            matrixPtr curFeature   = targetFeatures.targetMeanSpec;
            // We will ameliorate every solution based on partials
            for (k = 0; k < solutionSet.size(); k++)
            {
                // First we retrieve the partials of the solution
                matrixPtr solFeature = solutionSet[k]->getSolutionFeature(curFName);
                // And keep the maxVals and maxIDs
                matrixPtr maxVals = matrixPtr(new matrix(1, solFeature->cols()));
                for (maxV = 0, r = 0; r < solFeature->cols(); r++)
                {
                    maxVals->operator()(r) = MAX(curFeature->operator()(0, r) - solFeature->operator()(0, r), 0);
                    if (maxV < maxVals->operator()(r)) { maxV = maxVals->operator()(r); maxI = r; }
                }
                vector<float> solMidi = vector<float>(1);
                solMidi[0] = ((int)(maxI / 2) + 12);
                int microTonality = (maxI % resolution);
                if (solMidi[0] > 125)
                    continue;
                vector<string> curNote = midi2mtnotes(solMidi);
                for (inst = 0; inst < solutionIDs->cols(); inst++)
                {
                    string key = instrus[inst] + "-" + curNote[0];
                    vector<int> proposals = onMap[key];
                    vector<int> proposIDs = idMap[key];
                    vector<int> propoDurs = durMap[key];
                    if (!proposals.empty())
                    {
                        vector<float> dVals(MIN(proposals.size(), 10));
                        int minIdx = 0, maxIdx = (int)(proposals.size());
                        if (proposals.size() > 10) {minIdx = (rand() % (proposals.size() - 10)); maxIdx = minIdx + 10;}
                        #pragma omp parallel for shared(proposals, dVals)
                        for (j = minIdx; j < maxIdx; j++)
                        {
                            IndividualPtr tmpInd = sSession->getProduction()->getVariableIndividuals()[proposIDs[j]];
                            if (tmpInd == NULL || !tmpInd->getIsFilled()) { dVals[j - minIdx] = (((float)(rand() % 100) + 100.0f) / 100.0f); continue; }
                            matrixPtr featVect = tmpInd->getOriginalFeature("PartialsMeanAmplitude");
                            dVals[j - minIdx] = (vectorPartials(maxVals, featVect));
                        }
                        vector<float>   solVals = vector<float>(dVals.size());
                        vector<int>     solIds = vector<int>(dVals.size());
                        mySort(dVals, (int)dVals.size(), solVals, solIds);
                        for (h = 0; h < dVals.size() && h < nbGen; h++)
                        {
                            int index = proposals[solIds[h] + minIdx];
                            SolutionPtr newSol = SolutionPtr(new Solution(sSession, solutionIDs->cols()));
                            IndividualPtr newIndiv = IndividualPtr(new Individual(index, -1));
                            newIndiv->setMicrotonality(microTonality);
                            for (so = 0; so < solutionIDs->cols(); so++)
                                newSol->setIndividual(so, solutionSet[k]->getIndividual(so), solutionSet[k]->getIndividual(so)->getInstrument());
                            newSol->setIndividual(inst, newIndiv, index);
                            newSolutions.push_back(newSol);
                        }
                    }
                }
            }
            newSolutionIDs = matrixIPtr(new matrixI((int)newSolutions.size(), solutionIDs->cols()));
            for (sol = 0; sol < newSolutionIDs->rows(); sol++)
                for (col = 0; col < newSolutionIDs->cols(); col++)
                    newSolutionIDs->operator()(sol, col) = newSolutions[sol]->getIndividual(col)->getInstrument();
            t->setSolutions(newSolutions);
            t->setSolutionsIDs(newSolutionIDs);
            if (t->getNbSolutions() > nbKept)
            {
                vector<int> randomIDs;
                vector<float> randomIDs_f = gen_random_float_vector<float>(0, t->getNbSolutions(), nbKept);
                for (rF = 0; rF < randomIDs_f.size(); rF++)
                    randomIDs.push_back(floor(randomIDs_f[rF]));
                vector<SolutionPtr> newRandomSolutions;
                for (i = 0; i < nbKept; i++)
                    newRandomSolutions.push_back(t->solutionSet[randomIDs[i]]);
                t->setSolutionsIDs(t->solutionIDs->reorder2(randomIDs));
                t->setSolutions(newRandomSolutions);
            }
        }
    }
    return t;
}

PopulationPtr Population::adaptiveOptimization(sqlStruct& targetFeatures, bool TSIndex, map<string, indexTreePtr>& indexes, int nbGen, int nbKept)
{
    int i, j, k, r, curId, inst, h, sol, col, rF, maxO, maxI, maxD;
    float maxV;
    
    PopulationPtr t = PopulationPtr(new Population(sSession));
    // Retrieve the domains in which instruments can be chosen
    // Retrieve the variable table of indexes
    vector<int>                         variableTable;
    vector<pair<string, vector<int> > >   variableDomains;
    try {
        variableTable   = sSession->getProduction()->getVariableTable();
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Retrieve the list of optimization features
    vector<FeaturesPtr> optFeatures     = sSession->getFeatures();
    float               targetDuration  = targetFeatures.duration;
    int                 resolution      = sSession->getProduction()->getResolution();
    map<string, vector<int> > idMap      = sSession->getProduction()->getVariableOnsetsIDs();
    map<string, vector<int> > onMap      = sSession->getProduction()->getVariableOnsetsDomains();
    map<string, vector<int> > durMap     = sSession->getProduction()->getVariableDurationsDomains();
    vector<SolutionPtr> newSolutions;
    matrixIPtr          newSolutionIDs;
    int                 c, nbIns = solutionIDs->cols();
    
    printf("Adaptive optimization ...\n");
    for (i = 0; i < optFeatures.size(); i++)
    {
        printf("  - %s\n", optFeatures[i]->getFeatureName().c_str());
        string      curFName = optFeatures[i]->getFeatureName();
        matrixPtr   curFeature;
        if (sSession->getIsMultiTarget())
            curFeature = sSession->getTarget()->getFeature(targetFeatures, curFName);
        else
            curFeature = sSession->getTarget()->getFeature(curFName);
        if (curFName == "PartialsAmplitude")
        {
            curFeature   = targetFeatures.targetSpec;
            // We will ameliorate every solution based on partials
            for (k = 0; k < solutionSet.size(); k++)
            {
                // First we retrieve the partials of the solution
                matrixPtr solFeature = solutionSet[k]->getSolutionFeature(curFName);
                // And keep the maxVals and maxIDs
                matrixPtr maxVals = matrixPtr(new matrix(solFeature->rows(), solFeature->cols()));
                for (maxV = 0, r = 0; r < solFeature->rows(); r++)
                    for (c = 0; c < solFeature->cols(); c++)
                    {
                        maxVals->operator()(r, c) = MAX(curFeature->operator()(r, c) - solFeature->operator()(r, c), 0);
                        if (maxV < maxVals->operator()(r, c)) { maxV = maxVals->operator()(r, c); maxI = r; maxO = c;}
                    }
                vector<float> solMidi = vector<float>(1);
                solMidi[0] = ((int)(maxI / 2) + 12);
                int microTonality = (maxI % resolution);
                if (solMidi[0] > 125)
                    continue;
                vector<string> curNote = midi2mtnotes(solMidi);
                for (j = maxO, maxD = 0; j > 0 && (maxVals->operator()(maxI, j) || (maxVals->operator()(maxI, j-1))); j--)
                    maxO--;
                for (j = maxO, maxD = 0; j < solFeature->cols() - 1 && (maxVals->operator()(maxI, j) || (maxVals->operator()(maxI, j+1))); j++)
                    maxD++;
                matrixPtr curShift = matrixPtr(new matrix(curFeature->rows(), MIN(curFeature->cols() - maxO, 4)));
                if (maxD < 2)
                    maxD = 0;
                for (r = 0; r < curFeature->rows(); r++)
                    for (j = maxO; j < (maxO + curShift->cols()); j++)
                        curShift->operator()(r, j - maxO) = curFeature->operator()(r, j);
                /*
                matrixPtr curShift = matrixPtr(new matrix(curFeature->rows(), 1));
                for (r = 0; r < curFeature->rows(); r++)
                 curShift->operator()(r, 0) = curFeature->operator()(r, maxO);
                 */
                for (inst = 0; inst < solutionIDs->cols(); inst++)
                {
                    string key = sSession->getProduction()->getInstruments()[inst] + "-" + curNote[0];
                    vector<int> proposals = onMap[key];
                    vector<int> proposIDs = idMap[key];
                    vector<int> propoDurs = durMap[key];
                    if (!proposals.empty())
                    {
                        vector<float> dVals = vector<float>(MIN(proposals.size(), 10));
                        int minIdx = 0, maxIdx = (int)(proposals.size());
                        if (proposals.size() > 10) {minIdx = (rand() % (proposals.size() - 10)); maxIdx = minIdx + 10;}
                        #pragma omp parallel for shared(proposIDs, dVals)
                        for (j = minIdx; j < maxIdx; j++)
                        {
                            IndividualPtr tmpInd = sSession->getProduction()->getVariableIndividuals()[proposIDs[j]];
                            if (tmpInd == NULL || !tmpInd->getIsFilled()) { dVals[j - minIdx] = (((float)(rand() % 100) + 100.0f) / 100.0f); continue; }
                            matrixPtr featVect = tmpInd->getOriginalFeature("PartialsAmplitude");
                            dVals[j - minIdx] = (matrixPartials(curShift, featVect));
                        }
                        SolutionPtr newSol;
                        vector<float>   solVals = vector<float>(dVals.size());
                        vector<int>     solIds = vector<int>(dVals.size());
                        mySort(dVals, (int)dVals.size(), solVals, solIds);
                        for (h = 0; h < dVals.size() && h < nbGen; h++)
                        {
                            int index = proposals[solIds[h] + minIdx];
                            newSol = SolutionPtr(new Solution(*solutionSet[k]));
                            IndividualPtr newIndiv = IndividualPtr(new Individual(index, maxO * 2, maxD * 2));
                            //printf("Added : %d - %d\n", newIndiv->getOnset(), newIndiv->getDuration());
                            newIndiv->setMicrotonality(microTonality);
                            newSol->setIndividual(inst, newIndiv, index);
                            newSolutions.push_back(newSol);
                        }
                    }
                }
            }
        }
        if (curFeature->size() > 1 && !(curFeature->cols() > 1) && (curFName != "PartialsMeanAmplitude") && (curFName != "MelMeanAmplitude") && TSIndex)
        {
            indexTreePtr indexDesc;
            matrixPtr curFeat = boost::get<matrixPtr>(featuresSet[curFName]);
            if (TSIndex)
                //indexDesc = constructIndexesFromDB(sSession->getKnowledge(), curFName);
                indexDesc = indexes[curFName];
            printf("Performing segmentation on %s\n", optFeatures[i]->getFeatureName().c_str());
            for (k = 0; k < solutionSet.size(); k++)
            {
                matrixPtr       tmpFeature = matrixPtr(new matrix(curFeature));
                vector<float>   curFeatRow = curFeat->getRowVec(k);
                for (j = 0; j < curFeatRow.size(); j++)
                    tmpFeature->operator()(j) -= curFeatRow[j];
                int             resample_size = tmpFeature->size() / 2;
                float*          resampleModifCurve = new float[resample_size];
                resampleData(tmpFeature->c_data(), tmpFeature->size(), resampleModifCurve, resample_size, 4);
                matrixPtr       modifCurve(new matrix(resampleModifCurve, true, 1, resample_size));
                
                vector<vector<float> >  resultSegments;
                vector<int>            tmpOn, durations;
                tsMultiLevelSegment(modifCurve, 8, resultSegments, tmpOn, durations);
                for (j = 0; j < resultSegments.size(); j++)
                {
                    //printf("Performing segmentation on %s\t%d / %zd\n", optFeatures[i]->getFeatureName().c_str(), j + 1, resultSegments.size());
                    float           sDuration = targetDuration * (resultSegments[j].size() / 64);
                    if (sDuration < 0.5)
                        continue;
                    if (TSIndex)
                    {
                        vector<int>     results;
                        indexNodePtr    resNode;
                        indexDesc->approximateQueryConstrained(resultSegments[j], variableTable, 5, sDuration, results, resNode);
                        for (inst = 0; inst < solutionIDs->cols(); inst++)
                        {
                            vector<int> tmpProposals, index;
                            index = isMember(results, variableDomains[inst].second);
                            reorder(variableDomains[inst].second, index, tmpProposals);
                            if (!tmpProposals.empty())
                            {
                                for (h = 0; h < tmpProposals.size(); h++)
                                {
                                    SolutionPtr newSol = SolutionPtr(new Solution(sSession, solutionIDs->cols()));
                                    IndividualPtr newIndiv = IndividualPtr(new Individual(tmpProposals[h], tmpOn[j] * 2));
                                    newSol->setIndividuals(solutionSet[k]->getIndividuals());
                                    newSol->setIndividual(inst, newIndiv, tmpProposals[h]);
                                    newSolutions.push_back(newSol);
                                    newSolutionIDs->quickmerge(&newSol->getIndividualsID()[0]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    newSolutionIDs = matrixIPtr(new matrixI((int)newSolutions.size(), solutionIDs->cols()));
    for (sol = 0; sol < newSolutionIDs->rows(); sol++)
        for (col = 0; col < newSolutionIDs->cols(); col++)
            newSolutionIDs->operator()(sol, col) = newSolutions[sol]->getIndividual(col)->getInstrument();
    t->setSolutions(newSolutions);
    t->setSolutionsIDs(newSolutionIDs);
    if (t->getNbSolutions() > nbKept)
    {
        vector<int> randomIDs;
        vector<float> randomIDs_f = gen_random_float_vector<float>(0, t->getNbSolutions(), nbKept);
        for (rF = 0; rF < randomIDs_f.size(); rF++)
            randomIDs.push_back(floor(randomIDs_f[rF]));
        //t = t->extractPopulation(randomIDs);
        // ExtractPop marche po car criteriaSet et featuresSet sont vides!
        vector<SolutionPtr> newRandomSolutions;
        for (i = 0; i < nbKept; i++)
            newRandomSolutions.push_back(t->solutionSet[randomIDs[i]]);
        t->setSolutionsIDs(t->solutionIDs->reorder2(randomIDs));
        t->setSolutions(newRandomSolutions);
    }
    return t;
}

/*
PopulationPtr Population::adaptiveOptimization(sqlStruct& targetFeatures, bool TSIndex, map<string, indexTreePtr>& indexes)
{
    int i, j, k, r, curId, inst, h, sol, col, rF;
    PopulationPtr t = PopulationPtr(new Population(sSession));
    // Retrieve the domains in which instruments can be chosen
    // Retrieve the variable table of indexes
    vector<int>                         variableTable;
    vector<pair<string, vector<int> > >   variableDomains;
    try {
        variableTable   = sSession->getProduction()->getVariableTable();
        variableDomains = sSession->getProduction()->getVariableDomains();
    } catch (const char* e) {
        throw e;
    }
    // Retrieve the list of optimization features
    vector<FeaturesPtr> optFeatures     = sSession->getFeatures();
    float               targetDuration  = targetFeatures.duration;
    int                 resolution      = sSession->getProduction()->getResolution();
    map<string, vector<int> > onMap      = sSession->getProduction()->getVariableOnsetsDomains();
    map<string, vector<int> > durMap     = sSession->getProduction()->getVariableDurationsDomains();
    vector<SolutionPtr> newSolutions;
    matrixIPtr          newSolutionIDs;
    int                 c, nbIns = solutionIDs->cols();
    
    printf("Adaptive optimization ...\n");
    for (i = 0; i < optFeatures.size(); i++)
    {
        printf("  - %s\n", optFeatures[i]->getFeatureName().c_str());
        string      curFName = optFeatures[i]->getFeatureName();
        matrixPtr   curFeature;
        if (sSession->getIsMultiTarget())
            curFeature = sSession->getTarget()->getFeature(targetFeatures, curFName);
        else
            curFeature = sSession->getTarget()->getFeature(curFName);
        if (curFName == "PartialsAmplitude")
        {
            curFeature   = targetFeatures.targetSpec;
            // We will ameliorate every solution based on partials
            for (k = 0; k < solutionSet.size(); k++)
            {
                // First we retrieve the partials of the solution
                matrixPtr solFeature = solutionSet[k]->getSolutionFeature(curFName);
                // And keep the maxVals and maxIDs
                vector<int> maxIDs = vector<int>(solFeature->cols() * solFeature->rows());
                vector<float> maxVals = vector<float>(solFeature->cols() * solFeature->rows());
                vector<float> maxOnsets = vector<float>(solFeature->cols() * solFeature->rows());
                vector<float> maxDurations = vector<float>(solFeature->cols() * solFeature->rows());
                for (r = 0; r < solFeature->rows(); r++)
                    for (c = 0; c < solFeature->cols(); c++)
                    {
                        curId = (r * solFeature->cols()) + c;
                        maxVals[curId] = curFeature->operator()(r, c) - solFeature->operator()(r, c);
                        maxIDs[curId] = r;
                        maxOnsets[curId] = c;
                    }
                vector<float>   solVals = vector<float>(solFeature->cols() * solFeature->rows());
                vector<int>     solIds = vector<int>(solFeature->cols() * solFeature->rows());
                vector<float>   solMidi = vector<float>(1);
                mySort(maxVals, solFeature->cols() * solFeature->rows(), solVals, solIds);
                for (j = 10, curId = (int)solIds.size() - 1; curId > (int)solIds.size() - 2; curId--, j -= 2)
                {
                    solMidi[0] = ((int)(maxIDs[solIds[curId]] / 2) + 12);
                    int microTonality = floor((maxIDs[solIds[curId]] % 2) * (resolution / 2));
                    if (solMidi[0] > 127)
                        continue;
                    vector<string> curNote = midi2mtnotes(solMidi);
                    for (inst = 0; inst < nbIns; inst++)
                    {
                        string key = sSession->getProduction()->getInstruments()[inst] + "-" + curNote[0];
                        vector<int> proposals = onMap[key];
                        vector<int> propoDurs = durMap[key];
                        if (!proposals.empty())
                        {
                            vector<float> randomizer = gen_random_float_vector<float>(0, proposals.size(), j);
                            for (h = 0; h < randomizer.size(); h++)
                            {
                                int index = floor(randomizer[h]);
                                SolutionPtr newSol = SolutionPtr(new Solution(*solutionSet[k]));
                                IndividualPtr newIndiv = IndividualPtr(new Individual(proposals[index], maxOnsets[solIds[curId]], propoDurs[index]));
                                newIndiv->setMicrotonality(microTonality);
                                //for (so = 0; so < solutionIDs->cols(); so++)
                                    //newSol->setIndividual(so, solutionSet[k]->getIndividual(so), solutionSet[k]->getIndividual(so)->getInstrument());
                                newSol->setIndividual(inst, newIndiv, proposals[index]);
                                newSolutions.push_back(newSol);
                            }
                        }
                    }
                }
            }
        }
        if (curFeature->size() > 1 && !(curFeature->cols() > 1) && (curFName != "PartialsMeanAmplitude") && (curFName != "MelMeanAmplitude"))
        {
            indexTreePtr indexDesc;
            matrixPtr curFeat = boost::get<matrixPtr>(featuresSet[curFName]);
            if (TSIndex)
                //indexDesc = constructIndexesFromDB(sSession->getKnowledge(), curFName);
                indexDesc = indexes[curFName];
            printf("Performing segmentation on %s\n", optFeatures[i]->getFeatureName().c_str());
            for (k = 0; k < solutionSet.size(); k++)
            {
                matrixPtr       tmpFeature = matrixPtr(new matrix(curFeature));
                vector<float>   curFeatRow = curFeat->getRowVec(k);
                for (j = 0; j < curFeatRow.size(); j++)
                    tmpFeature->operator()(j) -= curFeatRow[j];
                int             resample_size = tmpFeature->size() / 2;
                float*          resampleModifCurve = new float[resample_size];
                resampleData(tmpFeature->c_data(), tmpFeature->size(), resampleModifCurve, resample_size, 4);
                matrixPtr       modifCurve(new matrix(resampleModifCurve, true, 1, resample_size));
                
                vector<vector<float> >  resultSegments;
                vector<int>            tmpOn, durations;
                tsMultiLevelSegment(modifCurve, 8, resultSegments, tmpOn, durations);
                for (j = 0; j < resultSegments.size(); j++)
                {
                    //printf("Performing segmentation on %s\t%d / %zd\n", optFeatures[i]->getFeatureName().c_str(), j + 1, resultSegments.size());
                    float           sDuration = targetDuration * (resultSegments[j].size() / 64);
                    if (sDuration < 0.5)
                        continue;
                    if (TSIndex)
                    {
                        vector<int>     results;
                        indexNodePtr    resNode;
                        indexDesc->approximateQueryConstrained(resultSegments[j], variableTable, 5, sDuration, results, resNode);
                        for (inst = 0; inst < solutionIDs->cols(); inst++)
                        {
                            vector<int> tmpProposals, index;
                            index = isMember(results, variableDomains[inst].second);
                            reorder(variableDomains[inst].second, index, tmpProposals);
                            if (!tmpProposals.empty())
                            {
                                for (h = 0; h < tmpProposals.size(); h++)
                                {
                                    SolutionPtr newSol = SolutionPtr(new Solution(sSession, solutionIDs->cols()));
                                    IndividualPtr newIndiv = IndividualPtr(new Individual(tmpProposals[h], tmpOn[j] * 2));
                                    newSol->setIndividuals(solutionSet[k]->getIndividuals());
                                    newSol->setIndividual(inst, newIndiv, tmpProposals[h]);
                                    newSolutions.push_back(newSol);
                                    newSolutionIDs->quickmerge(&newSol->getIndividualsID()[0]);
                                }
                            }
                        }
                    }
                    else
                    {
                        float curValue = 0.0;
                        for (seg = 0; seg < resultSegments[j].size(); seg++)
                            curValue += resultSegments[j][seg];
                        curValue /= (float)resultSegments[j].size();
                        pair<float, float> vals = pair<float, float>(curValue - (curValue * 0.05), curValue + (curValue * 0.05));
                        vector<int> varTable = sSession->getProduction()->getVariableTable();
                        vector<int> soundIDs;
                        vector<Query> queries;
                        for (vT = 0; vT < varTable.size(); vT++)
                            soundIDs.push_back(varTable[vT]);
                        printf("Querying %s with %f\n", optFeatures[i]->getFeatureName().c_str(), curValue);
                        queries.push_back(Query("BETWEEN", optFeatures[i]->getFeatureName() + "Mean", vals));
                        sSession->getKnowledge()->getBDBConnector()->getSoundsQuery(queries, soundIDs);
                        for (inst = 0; inst < variableDomains.size(); inst++)
                        {
                            vector<int> index = isMember(soundIDs, variableDomains[inst].second);
                            vector<int> tmpProposals;
                            reorder(variableDomains[inst].second, index, tmpProposals);
                            if (!tmpProposals.empty())
                            {
                                for (h = 0; h < tmpProposals.size(); h++)
                                {
                                    SolutionPtr newSol = SolutionPtr(new Solution(sSession, solutionIDs->cols()));
                                    IndividualPtr newIndiv = IndividualPtr(new Individual(tmpProposals[h], tmpOn[j] * 2));
                                    newSol->setIndividuals(solutionSet[k]->getIndividuals());
                                    newSol->setIndividual(inst, newIndiv, tmpProposals[h]);
                                    newSolutions.push_back(newSol);
                                    newSolutionIDs->quickmerge(&newSol->getIndividualsID()[0]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    newSolutionIDs = matrixIPtr(new matrixI((int)newSolutions.size(), solutionIDs->cols()));
    for (sol = 0; sol < newSolutionIDs->rows(); sol++)
        for (col = 0; col < newSolutionIDs->cols(); col++)
            newSolutionIDs->operator()(sol, col) = newSolutions[sol]->getIndividual(col)->getInstrument();
    t->setSolutions(newSolutions);
    t->setSolutionsIDs(newSolutionIDs);
    if (t->getNbSolutions() > 200)
    {
        vector<int> randomIDs;
        vector<float> randomIDs_f = gen_random_float_vector<float>(0, t->getNbSolutions(), 200);
        for (rF = 0; rF < randomIDs_f.size(); rF++)
            randomIDs.push_back(floor(randomIDs_f[rF]));
        //t = t->extractPopulation(randomIDs);
        // ExtractPop marche po car criteriaSet et featuresSet sont vides!
        vector<SolutionPtr> newRandomSolutions;
        for (i = 0; i < randomIDs.size(); i++)
            newRandomSolutions.push_back(t->solutionSet[randomIDs[i]]);
        t->setSolutionsIDs(t->solutionIDs->reorder2(randomIDs));
        t->setSolutions(newRandomSolutions);
    }
    return t;
}
*/

void                Population::selectiveMutation(float *weights)
{
    int             i, j, curID, mutaID, curIns, curOn, curDur;
    int             nbIns = solutionSet[0]->getNbInstruments();
    int             nbSol = (int)solutionSet.size();
    int             nbDesc = criteriaSet->cols();
    int             nbPart = floor((float)nbSol / (float)nbIns);
    vector<float>   solVals = vector<float>(nbIns);
    vector<int>     solIds = vector<int>(nbIns);
    vector<float>   tmpVals = vector<float>(nbIns);
    vector<int>     ids = vector<int>(nbIns);
    
    // Generate a random population (same size as population_in)
    PopulationPtr randomPop = PopulationPtr(new Population(sSession));
    randomPop->generateRandomPopulation((int)solutionSet.size(), 0.3);
    // First rank solutions based on the fitness and current weights
    vector<float> fitness = vector<float>(nbSol);
    for (i = 0; i < nbSol; i++)
        for (fitness[i] = 0, j = 0; j < nbDesc; j++)
            fitness[i] += criteriaSet->operator()(i, j) * weights[j];
    mySort(fitness, nbSol, solVals, solIds);
    // Now partition the ranks into N parts and selectively mutate
    for (i = 0; i < nbIns; i++)
    {
        for (j = 0; j < nbPart; j++)
        {
            curID = solIds[(i * nbPart) + j];
            // We rank in the current solution the energy features
            vector<float> energies = solutionSet[curID]->getIndividualFeature("LoudnessMean")->getRowVec(0);
            mySort(energies, (int)energies.size(), tmpVals, ids);
            mutaID = ids[i];
            curIns = randomPop->solutionIDs->operator()(curID, mutaID);
            curOn = -1;
            curDur = -1;
            // We replace the instrument that lies in the same loudness-specific partition of the system
            IndividualPtr ind = IndividualPtr(new Individual(curIns, curOn, curDur));
            solutionSet[curID]->setIndividual(mutaID, ind, randomPop->solutionIDs->operator()(curID, mutaID));
            solutionIDs->operator()(curID, mutaID) = randomPop->solutionIDs->operator()(curID, mutaID);
        }
    }
}

void Population::mutation()
{
    int i;
    // Generate a random population (same size as population_in)
    PopulationPtr randomPop = PopulationPtr(new Population(sSession));
    randomPop->generateRandomPopulation((int)solutionSet.size(), 0.3);
    
    // Randomly insert elements of the randomPop matrix
    vector<float> mutaID = gen_random_float_vector<float>(0, solutionSet[0]->getNbInstruments(), (int)solutionSet.size());
    for (i = 0; i < mutaID.size(); i++)
    {
        mutaID[i] = floor(mutaID[i]);
        IndividualPtr ind = IndividualPtr(new Individual(randomPop->solutionIDs->operator()(i, mutaID[i]), -1, -1));
        solutionSet[i]->setIndividual(mutaID[i], ind, randomPop->solutionIDs->operator()(i, mutaID[i]));
        solutionIDs->operator()(i, mutaID[i]) = randomPop->solutionIDs->operator()(i, mutaID[i]);
    }
}

void Population::mutationProbas(vector<float> &probas)
{
    int mutaID, i, j;
    // Generate a random population (same size as population_in)
    PopulationPtr randomPop = PopulationPtr(new Population(sSession));
    randomPop->generateRandomPopulation((int)solutionSet.size(), 0.3);
    
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    
    // Randomly insert elements of the randomPop matrix
    for (i = 0; i < solutionSet.size(); i++)
    {
        // Number of instruments to mutate in the current solution
        int curNbIns = round(probas[i] * solutionSet[0]->getNbInstruments());
        for (j = 0; j < curNbIns; j++)
        {
            mutaID = (int)(gen() * solutionSet[0]->getNbInstruments());
            IndividualPtr ind = IndividualPtr(new Individual(randomPop->solutionIDs->operator()(i, mutaID), -1, -1));
            solutionSet[i]->setIndividual(mutaID, ind, randomPop->solutionIDs->operator()(i, mutaID));
            solutionIDs->operator()(i, mutaID) = randomPop->solutionIDs->operator()(i, mutaID);
        }
    }
}

void            Population::onePointCrossover()
{
    int         nbSol = (int)solutionSet.size();
    int         nbIns = solutionSet[0]->getNbInstruments();
    int         i, j;
    
    // Discard last element if population size is an odd number
    if (mod<int>(nbSol, 2) == 1)
    {
        solutionSet.pop_back();
        solutionIDs->delRow(solutionIDs->rows() - 1);
        nbSol--;
    }
    // Shuffle rows
    float *random = gen_random_float_array<float>(0, 1, nbSol);
    vector<int> randomIndex;
    mySort<float>(random, nbSol, random, randomIndex);
    free(random);
    // Rearrange solution set
    vector<SolutionPtr> tmpSolSet;
    for (i = 0; i < randomIndex.size(); i++)
        tmpSolSet.push_back(solutionSet[randomIndex[i]]);
    setSolutions(tmpSolSet);
    solutionIDs->reorder(randomIndex);
    tmpSolSet.clear();
    // Initialize map of Solutions
    vector<SolutionPtr> newSolSet;
    for (i = 0; i < nbSol; i++)
        newSolSet.push_back(SolutionPtr(new Solution(sSession, nbIns)));
    // We will simply go through the reordered set
    nbSol = (int)solutionSet.size();
    
    // We start at 1 as we exclude the point when crossing
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(1, nbIns);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    for (i = 0; i < nbSol / 2; i++)
    {
        matrixIPtr tmpIDSSet(new matrixI(2, nbIns));
        vector<IndividualPtr> tmpSolSet1, tmpSolSet2;
        for (j = 0; j < nbIns; j++)
        {
            tmpSolSet1.push_back(IndividualPtr());
            tmpSolSet2.push_back(IndividualPtr());
        }
        // Here select a random crossing point
        int crossPoint = (int)gen();
        for (j = 0; j < crossPoint; j++)
        {
            tmpSolSet1[j] = solutionSet[(i * 2)]->getIndividual(j);
            tmpSolSet2[j] = solutionSet[(i * 2) + 1]->getIndividual(j);
            tmpIDSSet->operator()(0, j) = solutionIDs->operator()((i * 2), j);
            tmpIDSSet->operator()(1, j) = solutionIDs->operator()((i * 2) + 1, j);
        }
        for (; j < nbIns; j++)
        {
            tmpSolSet1[j] = solutionSet[(i * 2) + 1]->getIndividual(j);
            tmpSolSet2[j] = solutionSet[(i * 2)]->getIndividual(j);
            tmpIDSSet->operator()(0, j) = solutionIDs->operator()((i * 2) + 1, j);
            tmpIDSSet->operator()(1, j) = solutionIDs->operator()((i * 2), j);
        }
        newSolSet[(i * 2)]->setIndividuals(tmpSolSet1);
        newSolSet[(i * 2) + 1]->setIndividuals(tmpSolSet2);
        for (j = 0; j < nbIns; j++)
        {
            solutionIDs->operator()((i * 2), j) = tmpIDSSet->operator()(0, j);
            solutionIDs->operator()((i * 2) + 1, j) = tmpIDSSet->operator()(1, j);
        }
    }
    setSolutions(newSolSet);
}

void            Population::shuffleRows()
{
    int         i, nbSol = (int)solutionSet.size();
    
    float *random = gen_random_float_array<float>(0, 1, nbSol);
    vector<int> randomIndex;
    mySort<float>(random, nbSol, random, randomIndex);
    free(random);
    // Rearrange solution set
    vector<SolutionPtr> tmpSolSet;
    for (i = 0; i < randomIndex.size(); i++)
        tmpSolSet.push_back(solutionSet[randomIndex[i]]);
    setSolutions(tmpSolSet);
    solutionIDs->reorder(randomIndex);
    criteriaSet->reorder(randomIndex);
    tmpSolSet.clear();
}

void            Population::crossoverProbas(vector<float> &probas)
{
    int         nbSol = (int)solutionSet.size();
    int         nbIns = solutionSet[0]->getNbInstruments();
    int         i, j;
    
    // Discard last element if population size is an odd number
    if (mod<int>(nbSol, 2) == 1)
    {
        solutionSet.pop_back();
        solutionIDs->delRow(solutionIDs->rows() - 1);
        nbSol--;
    }
    // Rearrange solution set
    vector<SolutionPtr> tmpSolSet;
    // Initialize map of Solutions
    vector<SolutionPtr> newSolSet;
    for (i = 0; i < nbSol; i++)
        newSolSet.push_back(SolutionPtr(new Solution(sSession, nbIns)));
    // We will simply go through the reordered set
    nbSol = (int)solutionSet.size();
    
    // This time let's trick the system (by block of 16)
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 65536);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    for (i = 0; i < nbSol / 2; i++)
    {
        if ((probas[i * 2] * 100) < (rand() % 100))
        {
            newSolSet[(i * 2)]->setIndividuals(solutionSet[(i * 2)]->getIndividuals());
            newSolSet[(i * 2) + 1]->setIndividuals(solutionSet[(i * 2) + 1]->getIndividuals());
            continue;
        }
        matrixIPtr tmpIDSSet(new matrixI(2, nbIns));
        vector<IndividualPtr> tmpSolSet1, tmpSolSet2;
        for (j = 0; j < nbIns; j++)
        {
            tmpSolSet1.push_back(IndividualPtr());
            tmpSolSet2.push_back(IndividualPtr());
        }
        // We will generate 16-bit long blocks of random mask
        int curMask;
        vector<int> bytes;
        for (j = 0; j < nbIns; j++)
        {
            if (j % 16 == 0)
                bytes = toBytes((int)gen(), 16);
            curMask = bytes[j % 16];
            tmpSolSet1[j] = solutionSet[curMask ? (i * 2) : ((i * 2) + 1)]->getIndividual(j);
            tmpSolSet2[j] = solutionSet[curMask ? ((i * 2) + 1) : (i * 2)]->getIndividual(j);
            tmpIDSSet->operator()(0, j) = solutionIDs->operator()(curMask ? (i * 2) : ((i * 2) + 1), j);
            tmpIDSSet->operator()(1, j) = solutionIDs->operator()(curMask ? ((i * 2) + 1) : (i * 2), j);
        }
        newSolSet[(i * 2)]->setIndividuals(tmpSolSet1);
        newSolSet[(i * 2) + 1]->setIndividuals(tmpSolSet2);
        for (j = 0; j < nbIns; j++)
        {
            solutionIDs->operator()((i * 2), j) = tmpIDSSet->operator()(0, j);
            solutionIDs->operator()((i * 2) + 1, j) = tmpIDSSet->operator()(1, j);
        }
    }
    setSolutions(newSolSet);
}

void            Population::uniformCrossover()
{
    int         nbSol = (int)solutionSet.size();
    int         nbIns = solutionSet[0]->getNbInstruments();
    int         i, j;
    
    // Discard last element if population size is an odd number
    if (mod<int>(nbSol, 2) == 1)
    {
        solutionSet.pop_back();
        solutionIDs->delRow(solutionIDs->rows() - 1);
        nbSol--;
    }
    // Shuffle rows
    float *random = gen_random_float_array<float>(0, 1, nbSol);
    vector<int> randomIndex;
    mySort<float>(random, nbSol, random, randomIndex);
    free(random);
    // Rearrange solution set
    vector<SolutionPtr> tmpSolSet;
    for (i = 0; i < randomIndex.size(); i++)
        tmpSolSet.push_back(solutionSet[randomIndex[i]]);
    setSolutions(tmpSolSet);
    solutionIDs->reorder(randomIndex);
    tmpSolSet.clear();
    // Initialize map of Solutions
    vector<SolutionPtr> newSolSet;
    for (i = 0; i < nbSol; i++)
        newSolSet.push_back(SolutionPtr(new Solution(sSession, nbIns)));
    // We will simply go through the reordered set
    nbSol = (int)solutionSet.size();
    
    // This time let's trick the system (by block of 16)
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 65536);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    for (i = 0; i < nbSol / 2; i++)
    {
        matrixIPtr tmpIDSSet(new matrixI(2, nbIns));
        vector<IndividualPtr> tmpSolSet1, tmpSolSet2;
        for (j = 0; j < nbIns; j++)
        {
            tmpSolSet1.push_back(IndividualPtr());
            tmpSolSet2.push_back(IndividualPtr());
        }
        // We will generate 16-bit long blocks of random mask
        int curMask;
        vector<int> bytes;
        for (j = 0; j < nbIns; j++)
        {
            if (j % 16 == 0)
                bytes = toBytes((int)gen(), 16);
            curMask = bytes[j % 16];
            tmpSolSet1[j] = solutionSet[curMask ? (i * 2) : ((i * 2) + 1)]->getIndividual(j);
            tmpSolSet2[j] = solutionSet[curMask ? ((i * 2) + 1) : (i * 2)]->getIndividual(j);
            tmpIDSSet->operator()(0, j) = solutionIDs->operator()(curMask ? (i * 2) : ((i * 2) + 1), j);
            tmpIDSSet->operator()(1, j) = solutionIDs->operator()(curMask ? ((i * 2) + 1) : (i * 2), j);
        }
        newSolSet[(i * 2)]->setIndividuals(tmpSolSet1);
        newSolSet[(i * 2) + 1]->setIndividuals(tmpSolSet2);
        for (j = 0; j < nbIns; j++)
        {
            solutionIDs->operator()((i * 2), j) = tmpIDSSet->operator()(0, j);
            solutionIDs->operator()((i * 2) + 1, j) = tmpIDSSet->operator()(1, j);
        }
    }
    setSolutions(newSolSet);
}


void            Population::onsetsCrossover()
{
    int         nbSol = (int)solutionSet.size();
    int         nbIns = solutionSet[0]->getNbInstruments();
    int         i, j;
    
    // Discard last element if population size is an odd number
    if (mod<int>(nbSol, 2) == 1)
    {
        solutionSet.pop_back();
        solutionIDs->delRow(solutionIDs->rows() - 1);
        nbSol--;
    }
    // Shuffle rows
    float *random = gen_random_float_array<float>(0, 1, nbSol);
    vector<int> randomIndex;
    mySort<float>(random, nbSol, random, randomIndex);
    free(random);
    // Rearrange solution set
    vector<SolutionPtr> tmpSolSet;
    for (i = 0; i < randomIndex.size(); i++)
        tmpSolSet.push_back(solutionSet[randomIndex[i]]);
    setSolutions(tmpSolSet);
    solutionIDs->reorder(randomIndex);
    tmpSolSet.clear();
    // Initialize map of Solutions
    vector<SolutionPtr> newSolSet;
    for (i = 0; i < nbSol; i++)
        newSolSet.push_back(SolutionPtr(new Solution(sSession, nbIns)));
    // We will simply go through the reordered set
    nbSol = (int)solutionSet.size();
    
    // This time let's trick the system (by block of 16)
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 65536);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    for (i = 0; i < nbSol / 2; i++)
    {
        matrixIPtr tmpIDSSet(new matrixI(2, nbIns));
        vector<IndividualPtr> tmpSolSet1, tmpSolSet2;
        for (j = 0; j < nbIns; j++)
        {
            tmpSolSet1.push_back(IndividualPtr());
            tmpSolSet2.push_back(IndividualPtr());
        }
        // We will generate 16-bit long blocks of random mask
        int curMask;
        vector<int> bytes;
        for (j = 0; j < nbIns; j++)
        {
            if (j % 16 == 0)
                bytes = toBytes((int)gen(), 16);
            curMask = bytes[j % 16];
            if (curMask)
            {
                tmpSolSet1[j] = solutionSet[(i * 2)]->getIndividual(j);
                tmpSolSet1[j]->setOnset(solutionSet[((i * 2) + 1)]->getIndividual(j)->getOnset());
                tmpSolSet2[j] = solutionSet[(i * 2) + 1]->getIndividual(j);
                tmpSolSet2[j]->setOnset(solutionSet[(i * 2)]->getIndividual(j)->getOnset());
            }
            else
            {
                tmpSolSet1[j] = solutionSet[(i * 2)]->getIndividual(j);
                tmpSolSet2[j] = solutionSet[((i * 2) + 1)]->getIndividual(j);
            }
            tmpIDSSet->operator()(0, j) = solutionIDs->operator()((i * 2), j);
            tmpIDSSet->operator()(1, j) = solutionIDs->operator()((i * 2) + 1, j);
        }
        newSolSet[(i * 2)]->setIndividuals(tmpSolSet1);
        newSolSet[(i * 2) + 1]->setIndividuals(tmpSolSet2);
        for (j = 0; j < nbIns; j++)
        {
            solutionIDs->operator()((i * 2), j) = tmpIDSSet->operator()(0, j);
            solutionIDs->operator()((i * 2) + 1, j) = tmpIDSSet->operator()(1, j);
        }
    }
    setSolutions(newSolSet);
}

void Population::crossover()
{
    int i, j, k;
    // Discard last element if population size is an odd number
    int nbSol = (int)solutionSet.size();
    int nbIns = solutionSet[0]->getNbInstruments();
    
    if (mod<int>(nbSol, 2) == 1)
    {
        solutionSet.pop_back();
        solutionIDs->delRow(solutionIDs->rows() - 1);
        nbSol--;
    }
    
    // Shuffle rows
    float *random = gen_random_float_array<float>(0, 1, nbSol);
    vector<int> randomIndex;
    mySort<float>(random, nbSol, random, randomIndex);
    free(random);
    
    vector<SolutionPtr> tmpSolSet;
    for (i = 0; i < randomIndex.size(); i++)
        tmpSolSet.push_back(solutionSet[randomIndex[i]]);
    setSolutions(tmpSolSet);
    solutionIDs->reorder(randomIndex);
    tmpSolSet.clear();
    
    // Build crossover index vector
    nbSol = (int)solutionSet.size();
    vector<int> i1, i2;
    for (i = 0; i < nbSol / 2; i++)
    {
        i1.push_back(i * 2);
        i2.push_back(i * 2 + 1);
    }
    matrixIPtr I1(new matrixI(nbSol / 2, nbIns));
    matrixIPtr I2(new matrixI(nbSol / 2, nbIns));
    vector<float> rand = gen_random_float_vector<float>(0, 1, nbSol / 2 * nbIns);
    for (i = 0; i < nbSol / 2; i++)
        for (j = 0; j < nbIns; j++)
        {
            (rand[nbIns * i + j] > 0.5) ? I1->operator()(i, j) = 1 : I1->operator()(i, j) = 0;
            I2->operator()(i, j) = 1 - I1->operator()(i, j);
        }
    matrixIPtr I(new matrixI(nbSol, nbIns));
    for (i = 0; i < i1.size(); i++)
    {
        for (j = 0; j < nbIns; j++)
        {
            I->operator()(i1[i], j) = I1->operator()(i, j);
            I->operator()(i2[i], j) = I2->operator()(i, j);
        }
    }
    
    // Re-arrange instruments
    matrixIPtr instVector(new matrixI(nbSol, nbIns));
    for (i = 0; i < instVector->rows(); i++)
    {
        int cnt = 1;
        for (j = 0; j < instVector->cols(); j++)
            instVector->operator()(i, j) = cnt++;
    }
    matrixIPtr A = matrixIPtr(new matrixI(instVector));
    matrixIPtr B = matrixIPtr(new matrixI(instVector));
    A->operator*(I);
    for (i = 0; i < I->rows(); i++)
        for (j = 0; j < I->cols(); j++)
            I->operator()(i, j) = 1 - I->operator()(i, j);
    B->operator*(I);

    // Initialize map of Solutions
    vector<SolutionPtr> newSolSet;
    for (i = 0; i < nbSol; i++)
        newSolSet.push_back(SolutionPtr(new Solution(sSession, nbIns)));

    for (i = 0; i < nbSol / 2; i++)
    {
        matrixIPtr tmpIDSSet(new matrixI(2, nbIns));
        vector<IndividualPtr> tmpSolSet1, tmpSolSet2;
        for (j = 0; j < nbIns; j++)
        {
            tmpSolSet1.push_back(IndividualPtr());
            tmpSolSet2.push_back(IndividualPtr());
        }
        
        for (j = 0; j < 2; j++)
        {
            matrixIPtr  idA;
            vector<int> idF;
            if (j == 0)
            {
                idA = A;
                idF = i1;
            }
            else
            {
                idA = B;
                idF = i2;
            }
            vector<int> vInstruA = idA->getRowVec(i1[i]);
            vector<int> vInstruB = idA->getRowVec(i2[i]);
            
            for (k = 0; k < vInstruA.size(); k++)
            {
                if (vInstruA[k] > 0)
                {
                    if (j == 0)
                        tmpSolSet1[vInstruA[k] - 1] = solutionSet[i1[i]]->getIndividual(vInstruA[k] - 1);
                    else
                        tmpSolSet2[vInstruA[k] - 1] = solutionSet[i1[i]]->getIndividual(vInstruA[k] - 1);
                    
                    tmpIDSSet->operator()(j, vInstruA[k] - 1) = solutionIDs->operator()(i1[i], vInstruA[k] - 1);
                }
            }
            for (k = 0; k < vInstruB.size(); k++)
            {
                if (vInstruB[k] > 0)
                {
                    if (j == 0)
                        tmpSolSet1[vInstruB[k] - 1] = solutionSet[i2[i]]->getIndividual(vInstruB[k] - 1);
                    else
                        tmpSolSet2[vInstruB[k] - 1] = solutionSet[i2[i]]->getIndividual(vInstruB[k] - 1);
                    
                    tmpIDSSet->operator()(j, vInstruB[k] - 1) = solutionIDs->operator()(i2[i], vInstruB[k] - 1);
                }
            }
        }
        
        if (!tmpSolSet1.empty())
            newSolSet[i1[i]]->setIndividuals(tmpSolSet1);
        if (!tmpSolSet2.empty())
            newSolSet[i2[i]]->setIndividuals(tmpSolSet2);
        
        for (j = 0; j < tmpIDSSet->cols(); j++)
        {
            solutionIDs->operator()(i1[i], j) = tmpIDSSet->operator()(0, j);
            solutionIDs->operator()(i2[i], j) = tmpIDSSet->operator()(1, j);
        }
    }
    setSolutions(newSolSet);
}

matrixPtr Population::uniqueSolutions()
{
    vector<int> index = solutionIDs->uniqueRows();
    int         i;
    if (index.size() != 0)
    {
        vector<SolutionPtr> solSet;
        for (i = 0; i < index.size(); i++)
            solSet.push_back(solutionSet[index[i]]);
        setSolutions(solSet);
        criteriaSet->reorder(index);
        vector<FeaturesPtr> optFeatures = sSession->getFeatures();
        for (i = 0; i < optFeatures.size(); i++)
        {
            string curFeatName = optFeatures[i]->getFeatureName();
            if (featuresSet.find(curFeatName) != featuresSet.end())
                boost::get<matrixPtr>(featuresSet[curFeatName])->reorder(index);
        }
    }
    return criteriaSet;
}

void Population::smoothPopulation(int popsize)
{
    int i;
    if (solutionSet.size() > popsize)
    {
        // Compute local density
        vector<int> density, cell_idx;
        density_PADE(criteriaSet, density, cell_idx);
        vector<int> tmpSolIDX;
        for (i = 0; i < solutionSet.size(); i++)
            tmpSolIDX.push_back(i);
        
        // Remove elements with higher local density
        while (tmpSolIDX.size() > popsize)
        {
            // Pick the first element with highest density
            int denser_cell, max, remID = 0;
            getMax(&density[0], (int)density.size(), max, denser_cell);
            for (i = 0; i < cell_idx.size(); i++)
                if (cell_idx[i] == denser_cell)
                {
                    remID = i;
                    break;
                }
            // Decrement the local density in the cell
            density[denser_cell]--;
            // Remove individual
            tmpSolIDX.erase(tmpSolIDX.begin() + remID);
            cell_idx.erase(cell_idx.begin() + remID);
        }
        
        criteriaSet->reorder(tmpSolIDX);
        solutionIDs->reorder(tmpSolIDX);
        
        vector<SolutionPtr> solSet;
        for (i = 0; i < tmpSolIDX.size(); i++)
            solSet.push_back(solutionSet[tmpSolIDX[i]]);
        setSolutions(solSet);
    }
}

void Population::discardNaN()
{
    int i, j;
    vector<int> index;
    for (i = 0; i < criteriaSet->rows(); i++)
    {
        bool isNan = false;
        for (j = 0; j < criteriaSet->cols(); j++)
        {
            if (isnan(criteriaSet->operator()(i, j)) || isinf(criteriaSet->operator()(i, j)) )
                isNan = true;
        }
        if (!isNan)
            index.push_back(i);
    }

    criteriaSet->reorder(index);
    solutionIDs->reorder(index);
    vector<SolutionPtr> solSet;
    for (i = 0; i < index.size(); i++)
        solSet.push_back(solutionSet[index[i]]);
    setSolutions(solSet);
}

void Population::dumpPopulation(string filename, int dump_all)
{
    boost::filesystem::path output(filename);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int i, j;
    /* Create the directory, read/write/access owner only. */
    FILE *f = fopen((filename + ".txt").c_str(), "w");
    vector<FeaturesPtr> fList = sSession->getFeatures();
    for (i = 0; i < solutionIDs->rows(); i++)
    {
        for (j = 0; j < solutionIDs->cols(); j++)
            fprintf(f, "%i ", solutionIDs->operator()(i, j));
        fprintf(f, "\n");
        for (j = 0; j < solutionIDs->cols(); j++)
            fprintf(f, "%i ", solutionSet[i]->getIndividual(j)->getOnset());
        fprintf(f, "\n");
        for (j = 0; j < solutionIDs->cols(); j++)
            fprintf(f, "%i ", solutionSet[i]->getIndividual(j)->getDuration());
        fprintf(f, "\n");
        for (j = 0; j < solutionIDs->cols(); j++)
            fprintf(f, "%i ", solutionSet[i]->getIndividual(j)->getMicrotonality());
        fprintf(f, "\n");
        if (dump_all)
        {
            solutionSet[i]->dumpSolution(filename + "/solution" + to_string(i));
            ExportSound e(this->sSession, SOL_DIR);
            e.exportSingleSolution(solutionSet[i], filename + "/solution" + to_string(i) + ".wav");
        }
    }
    fclose(f);
    ExportRaw eR(this->sSession, SOL_DIR);
    eR.exportPopulation(this, filename + ".symbolic.txt");
    f = fopen((filename + ".criteria.txt").c_str(), "w");
    for (i = 0; i < criteriaSet->rows(); i++)
    {
        for (j = 0; j < criteriaSet->cols(); j++)
            fprintf(f, "%f\t\t", criteriaSet->operator()(i, j));
        fprintf(f, "\n");
    }
    fclose(f);
}