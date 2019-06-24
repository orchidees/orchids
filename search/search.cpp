//
//  search.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 17/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "search.h"


void Search::setSolutionSet(vector<SolutionPtr>& solSet)
{
    int i;
    solutionSet.clear();
    for (i = 0; i < solSet.size(); i++)
        solutionSet.push_back(solSet[i]);
}

vector<SolutionPtr> Search::getSolutionSet()
{
    return solutionSet;
}


SearchGenetic::SearchGenetic(Session* s)
{
    sSession = s;
    //setDefaultParameters();
}

void SearchGenetic::setDefaultParameters()
{
    this->initPopSize               = 200;
    this->maxPopSize                = 500;
    this->matingPoolSize            = 50;
    this->paretoMaxSize             = 100;
    this->nIter                     = 100;
    //this->globalCrossProb           = 0.7;
    //this->globalMutateProb          = 0.01;
    //this->oneCross                  = false;
    //this->uniformCross              = false;
    //this->onsetCross                = false;
    this->selectiveMutation         = true;
    this->globalProba               = true;
    this->harmonicFix               = true;
    this->harmonicFixFrequency      = 5;
    this->localOptim                = false;
    this->adaptiveOptim             = true;
    this->adaptiveOptimFrequency    = 5;
    this->optimFullPopulation       = false;
    this->optimPopSubsampling       = 50;
    this->optimGenIndividuals       = 3;
    this->optimGenSubsampling       = 50;
    this->TSIndex                   = false;
    this->popConstrained            = 0.5;
    this->popSparsity               = 0.4;
}

void SearchGenetic::setParameter(string name, float value)
{
    if (name == "initPopSize")
        initPopSize = value;
    else if (name == "maxPopSize")
        maxPopSize = value;
    else if (name == "matingPoolSize")
        matingPoolSize = value;
    else if (name == "paretoMaxSize")
        paretoMaxSize = value;
    else if (name == "nIter")
        nIter = value;
    else
        throw "SearchGenetic::setParameter::Invalid, wrong parameter name";
}

map<string, boost::any> SearchGenetic::getSearchParameters()
{
    map<string, boost::any> params;
    params["initPopSize"]               = initPopSize;
    params["maxPopSize"]                = maxPopSize;
    params["matingPoolSize"]            = matingPoolSize;
    params["paretoMaxSize"]             = paretoMaxSize;
    params["nIter"]                     = nIter;
    params["selectiveMutation"]         = selectiveMutation;
    params["globalProba"]               = globalProba;
    params["harmonicFix"]               = harmonicFix;
    params["harmonicFixFrequency"]      = harmonicFixFrequency;
    params["localOptim"]                = localOptim;
    params["adaptiveOptim"]             = adaptiveOptim;
    params["adaptiveOptimFrequency"]    = adaptiveOptimFrequency;
    params["TSIndex"]                   = TSIndex;
    return params;
}

void SearchGenetic::setSearchParameters(map<string, boost::any> params)
{
    map<string, boost::any>::iterator it;
    for (it = params.begin(); it != params.end(); ++it)
    {
        if (it->first == "selectiveMutation")
            setSelectiveMutation((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "globalProba")
            setGlobalProba((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "harmonicFix")
            setHarmonicFix((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "localOptim")
            setLocalOptim((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "adaptiveOptim")
            setAdaptiveOptim((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "TSIndex")
            setTSIndex((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "harmonicFixFrequency")
            setHarmonicFixFreq((int)boost::any_cast<int>(it->second));
        else if (it->first == "adaptiveOptimFrequency")
            setAdaptiveOptimFreq((int)boost::any_cast<int>(it->second));
        else
            setParameter(it->first, (int)boost::any_cast<int>(it->second));
    }
}

void SearchGenetic::initialize()
{
    // Update variable domains
    sSession->getProduction()->getVariableDomains();
}

PopulationPtr SearchGenetic::launchSearch(sqlStruct& targetFeatures)
{
    int i, j, k;
    if (sSession->getCriteriaList().size() == 0)
        throw "SearchGenetic::LaunchSearch, Criteria list is empty!";
    if (sSession->getProduction()->getInstruments().size() == 0)
        throw "SearchGenetic::LaunchSearch, Instruments list is empty!";
    
    // Get the target features
    //sqlStruct targetFeatures = sSession->getTarget()->getFeaturesList();
    vector<pair<string, vector<int>>>   variableDomains;
    vector<int>                         variableTable;
    vector<FeaturesPtr>                 optFeatures;
    vector<float>                       crossProbas, mutationProbas;
    try {
        // Retrieve the domains in which instruments can be chosen
        variableDomains = sSession->getProduction()->getVariableDomains();
        // Retrieve the variable table of indexes
        variableTable   = sSession->getProduction()->getVariableTable();
        // Retrieve the list of optimization features
        optFeatures     = sSession->getFeatures();
    } catch (const char* e) {
        throw e;
    }
    
    // Draw weights (Jaszkiewicz's method)
    matrixPtr weights;
    if (optFeatures.size() > 1)
        weights = draw_jaszkiewicz_weights(nIter, (int)optFeatures.size());
    else
    {
        weights = matrixPtr(new matrix(nIter, 1));
        weights->fill(1);
    }
#if DEBUG_MODE
    FILE* fWeight = fopen("/tmp/weights.txt", "w");
    for (i = 0; i < nIter; i++)
    {
        for (j = 0; j < optFeatures.size(); j++)
            fprintf(fWeight, "%f ", weights->operator()(i, j));
        fprintf(fWeight, "\n");
    }
    fclose(fWeight);
#endif
    PopulationPtr initPop(new Population(sSession));
    try {
        // Instantiate random initial population
        initPop->generateRandomPopulation(initPopSize, popSparsity);
        // Evaluate the initial population
        initPop->updatePopulationFeatures();
        eval_population(initPop, targetFeatures, optFeatures);
    } catch (const char * e) {
        throw e;
    }

    // Debugging population
#if DEBUG_MODE
    initPop->dumpPopulation(DEBUG_DIR + "0a.initPopulation", 0);
#endif
    // Remove NaN values from population
    initPop->discardNaN();
    matrixPtr pop_critr = initPop->getCriteria();
    // Extract the initial Pareto set
    bool *front = (bool *)calloc(pop_critr->size(), sizeof(bool));
    pop_critr->transpose();
    paretofront(front, pop_critr->c_data(), pop_critr->cols() , pop_critr->rows());
    pop_critr->transpose();
    //bool* front = paretoGroup(pop_critr);
    vector<int> loc;
    for (i = 0; i < pop_critr->rows(); i++)
        if (front[i])
            loc.push_back(i);
    free(front);
    PopulationPtr paretoPop = initPop->extractPopulation(loc);
    matrixPtr pareto_critr = paretoPop->getCriteria();
    // Debugging population
#if DEBUG_MODE
    sSession->getTarget()->dumpTargetFeatures(DEBUG_DIR + "target", targetFeatures.targetSpec, targetFeatures.targetMeanSpec);
    paretoPop->dumpPopulation(DEBUG_DIR + "Ob.initParetoPop", 0);
#endif
    // Ideal and nadir estimations
    vector<float> nadir_estimate = pop_critr->Max(1);
    vector<float> ideal_estimate = pop_critr->Min(1);
    vector<float> critr_ranges;
    for (i = 0; i < nadir_estimate.size(); i++)
        critr_ranges.push_back(nadir_estimate[i] - ideal_estimate[i]);
    for (i = 0; i < nIter; i++)
    {
#if DEBUG_MODE
        int debug_all = 0;
        if (mod<int>(i + 1, 10) == 0)
            debug_all = 1;
#endif
        string progression = to_string((((float)i + 1) / (float)nIter) * 100);
        sendOSCMessage("/Orchids/progression/orchestrate", progression);
        printf("Exploring search space ...\t%s\n", progression.c_str());
        // ***** Compute fitness *****
        float*          row         = weights->getRow(i);
        vector<float>   pop_fitness = scalarize(pop_critr, row, weights->cols(), ideal_estimate, critr_ranges);
        // ***** Selection *****
        vector<int>     selectedID;
        select_population(pop_fitness, matingPoolSize, selectedID);
        PopulationPtr   offspringPop    = initPop->extractPopulation(selectedID);
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/01.offspringPop", 0);
#endif
        // ***** Loudness-specific mutation *****
        if (selectiveMutation)
        {
            offspringPop->selectiveMutation(row);
            offspringPop->updatePopulationFeatures();
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/01b.selectiveMutated", 0);
#endif
        }
        // ***** Crossover and mutation based on adaptive probabilities *****
        if (globalProba)
        {
            offspringPop->shuffleRows();
            pop_critr = offspringPop->getCriteria();
            crossProbas = compute_cross_probas(ideal_estimate, nadir_estimate, pop_critr, row);
            mutationProbas = compute_mutation_probas(ideal_estimate, nadir_estimate, pop_critr, row);
            // ***** Probabilistic mutations *****
            offspringPop->mutationProbas(mutationProbas);
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/02.offspringPop_Mut", 0);
#endif
            // ***** Probabilistic crossover *****
            offspringPop->crossoverProbas(crossProbas);
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/03.offspringPop_Cross", 0);
#endif
        }
        // ***** Normal crossover / mutations *****
        if (!globalProba)
        {
            offspringPop->crossover();
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/04.offspringPop_Cross", 0);
#endif
            offspringPop->mutation();
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/05.offspringPop_Mut", 0);
#endif
        }
        // ***** Compute offspring criteria and audio fitness *****
        offspringPop->updatePopulationFeatures();
        try { eval_population(offspringPop, targetFeatures, optFeatures); }
        catch (const char* e) { throw e; }
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/06.offspringPop_Eval", 0);
#endif
        offspringPop->discardNaN();
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07.offspringPop_NaN", 0);
#endif
        matrixPtr offspring_critr = matrixPtr(new matrix(offspringPop->getCriteria()));
        if (mod<int>(i + 1, harmonicFixFrequency) == 0 && harmonicFix)
        {
            for (j = 0; j < optFeatures.size(); j++)
                if (optFeatures[j]->getFeatureName() == "PartialsMeanAmplitude")
                {
                    vector<int> Ibool, I;
                    PopulationPtr newPop, tmpPop;
                    if (optimFullPopulation)
                    {
                        Ibool = preserveDiversity(offspring_critr, optimPopSubsampling, "random");
                        for (k = 0; k < Ibool.size(); k++)
                            if (Ibool[k] == 1)
                                I.push_back(k);
                        tmpPop = offspringPop->extractPopulation(I);
                    }
                    else
                    {
                        Ibool = preserveDiversity(pareto_critr, optimPopSubsampling, "random");
                        for (k = 0; k < Ibool.size(); k++)
                            if (Ibool[k] == 1)
                                I.push_back(k);
                        tmpPop = paretoPop->extractPopulation(I);
                    }
#if DEBUG_MODE
                    tmpPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07a.harmonicFixed_Extracted", 0);
#endif
                    newPop = tmpPop->harmonicFix(targetFeatures, optimGenIndividuals, optimGenSubsampling);
                    if (newPop->getSolutionsIDs()->size() != 0)
                    {
                        newPop->updatePopulationFeatures();
                        try {
                            eval_population(newPop, targetFeatures, optFeatures);
                        } catch (const char* e) {
                            throw e;
                        }
                        offspringPop->combinePopulations(newPop);
                    }
#if DEBUG_MODE
                    if (newPop->getSolutionsIDs()->size() != 0)
                        newPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07b.harmonicFixed_Generated", 0);
#endif
                }
            offspring_critr = matrixPtr(new matrix(offspringPop->getCriteria()));
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07c.harmonicFixed_Comb", 0);
#endif
        }
        // Update ideal and nadir
        float* rowToMerge = (float*)malloc(nadir_estimate.size() * sizeof(float));
        for (j = 0; j < nadir_estimate.size(); j++)
            rowToMerge[j] = nadir_estimate[j];
        offspring_critr->quickmerge(rowToMerge);
        free(rowToMerge);
        nadir_estimate = offspring_critr->Max(1);
        offspring_critr->delRow(offspring_critr->rows() - 1);
        rowToMerge = (float*)malloc(ideal_estimate.size() * sizeof(float));
        for (j = 0; j < ideal_estimate.size(); j++)
            rowToMerge[j] = ideal_estimate[j];
        offspring_critr->quickmerge(rowToMerge);
        free(rowToMerge);
        ideal_estimate = offspring_critr->Min(1);
        critr_ranges.clear();
        for (j = 0; j < nadir_estimate.size(); j++)
            critr_ranges.push_back(nadir_estimate[j] - ideal_estimate[j]);
        // ***** Insert offspring in population *****
        initPop->combinePopulations(offspringPop);
#if DEBUG_MODE
        initPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/08.initPop_Combined", 0);
#endif
        // ***** Update pareto set *****
        paretoPop->combinePopulations(offspringPop);
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/09.offspringPop_Combined", 0);
#endif
        // ***** Keep only the unique solutions *****
        matrixPtr  pareto_critr_tmp    = paretoPop->uniqueSolutions();
        bool    *paretoFront = (bool *)calloc(pareto_critr_tmp->size(), sizeof(bool));
        pareto_critr_tmp->transpose();
        paretofront(paretoFront, pareto_critr_tmp->c_data(), pareto_critr_tmp->cols() , pareto_critr_tmp->rows());
        pareto_critr_tmp->transpose();
        vector<int> index;
        for (j = 0; j < pareto_critr_tmp->rows(); j++)
            if (paretoFront[j])
                index.push_back(j);
#if DEBUG_MODE
        FILE *pFrontCheck = fopen((DEBUG_DIR + "/iter_" + to_string(i) + "/paretoFront.txt").c_str(), "w");
        float minPar = MAXFLOAT;
        
        for (j = 0; j < pareto_critr_tmp->rows(); j++)
        {
            for (k = 0; k < pareto_critr_tmp->cols(); k++)
            {
                fprintf(pFrontCheck, "%f ", pareto_critr_tmp->operator()(j, k));
                if (pareto_critr_tmp->operator()(j, k) < minPar)
                    minPar = pareto_critr_tmp->operator()(j, k);
            }
            fprintf(pFrontCheck, "%d\n", paretoFront[j]);
        }
        fprintf(pFrontCheck, "Min found : %f\n", minPar);
        fprintf(pFrontCheck, "Selected elements :\n");
        for (j = 0; j < index.size(); j++)
        {
            fprintf(pFrontCheck, "%d : ", index[j]);
            for (k = 0; k < pareto_critr_tmp->cols(); k++)
                fprintf(pFrontCheck, "%f ", pareto_critr_tmp->operator()(index[j], k));
            fprintf(pFrontCheck, "\n");
        }
        fclose(pFrontCheck);
#endif
        free(paretoFront);
        paretoPop = paretoPop->extractPopulation(index);
        // ***** Pareto-wise local optimization *****
        if (localOptim)
        {
            paretoPop->localOptimization(targetFeatures);
            //paretoPop->combinePopulations(optimPop);
            paretoPop->updatePopulationFeatures();
            try { eval_population(paretoPop, targetFeatures, optFeatures); }
            catch (const char* e) { throw e; }
            //pareto_critr_tmp->deallocate();
            pareto_critr_tmp    = paretoPop->uniqueSolutions();
            paretoFront = (bool *)calloc(pareto_critr_tmp->size(), sizeof(bool));
            pareto_critr_tmp->transpose();
            paretofront(paretoFront, pareto_critr_tmp->c_data(), pareto_critr_tmp->cols() , pareto_critr_tmp->rows());
            pareto_critr_tmp->transpose();
            index.clear();
            for (j = 0; j < pareto_critr_tmp->rows(); j++)
                if (paretoFront[j])
                    index.push_back(j);
            free(paretoFront);
            paretoPop = paretoPop->extractPopulation(index);
        }
        pareto_critr = paretoPop->getCriteria();
        //pop_critr->deallocate();
        pop_critr = initPop->getCriteria();
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/10.paretoPop", 0);
#endif
        // ***** Population size handling *****
        vector<int> Jbool = pop_critr->isMember(pareto_critr);
        vector<int> J;
        for (j = 0; j < Jbool.size(); j++)
            if (Jbool[j] == 0)
                J.push_back(j);
        matrixPtr dominated_critr = matrixPtr(new matrix(pop_critr));
        dominated_critr->reorder(J);
        // Retriving dominated solutions
        PopulationPtr dominatedPop = initPop->extractPopulation(J);
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/11.dominatedPop", 0);
#endif
        // Reduce pareto set size if too large
        if (paretoPop->getNbSolutions() > paretoMaxSize)
        {
            vector<int> Ibool = preserveDiversity(pareto_critr, paretoMaxSize, "random");
            vector<int> I;
            for (j = 0; j < Ibool.size(); j++)
                if (Ibool[j] == 1)
                    I.push_back(j);
            paretoPop = paretoPop->extractPopulation(I);
            pareto_critr = paretoPop->getCriteria();
        }
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/12.paretoPopReduced", 0);
#endif
        // Preserve density for dominated individuals
        if (dominated_critr->rows() > (maxPopSize - paretoPop->getNbSolutions()))
            dominatedPop->smoothPopulation(maxPopSize - paretoPop->getNbSolutions());
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/13.dominatedPopReduced", 0);
#endif
        dominatedPop->uniqueSolutions();
        // Mix reduced Pareto set and individuals of homogeneous density into next generation
        dominatedPop->combinePopulations(paretoPop);
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/14.terminalPop", 0);
#endif
        initPop     = dominatedPop;
        pop_critr   = initPop->getCriteria();
        
        //initPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "_initPop", 0);
        //paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "_paretoPop", 0);
    }
    
    // Build output structure
    solutionSet = paretoPop->getSolutions();
    
    // Rescale criterias
    pareto_critr = paretoPop->getCriteria();
    vector<float> minsEnds = pareto_critr->Min(1);
    printf("%d - %d\n", pareto_critr->rows(), pareto_critr->cols());
    for (i = 0; i < pareto_critr->rows(); i++)
        for (j = 0; j < pareto_critr->cols(); j++)
            pareto_critr->operator()(i, j) -= minsEnds[j];
    minsEnds.empty();
    minsEnds = pareto_critr->Max(1);
    for (i = 0; i < pareto_critr->rows(); i++)
        for (j = 0; j < pareto_critr->cols(); j++)
            pareto_critr->operator()(i, j) /= minsEnds[j];
    pareto_critr->operator+(0.1);
#if DEBUG_MODE
    paretoPop->getCriteria()->print();
    paretoPop->dumpPopulation(DEBUG_DIR + "/finalSolutions", 1);
#endif

    return paretoPop;
}

SearchOptimalWarping::SearchOptimalWarping(Session* s)
{
    sSession = s;
    //setDefaultParameters();
}

void SearchOptimalWarping::setDefaultParameters()
{
    rndFactor                       = 0.5;
    nbSegBase                       = 12;
    this->initPopSize               = 200;
    this->maxPopSize                = 500;
    this->matingPoolSize            = 50;
    this->paretoMaxSize             = 100;
    nIter                           = 100;
    //this->globalCrossProb           = 0.7;
    //this->globalMutateProb          = 0.01;
    //this->oneCross                  = false;
    //this->uniformCross              = false;
    //this->onsetCross                = false;
    this->selectiveMutation         = true;
    this->globalProba               = true;
    this->harmonicFix               = true;
    this->harmonicFixFrequency      = 5;
    this->localOptim                = false;
    this->adaptiveOptim             = true;
    this->adaptiveOptimFrequency    = 5;
    this->optimFullPopulation       = false;
    this->optimPopSubsampling       = 50;
    this->optimGenIndividuals       = 2;
    this->optimGenSubsampling       = 50;
    this->popConstrained            = 0.9;
    this->popSparsity               = 0.4;
    this->TSIndex                   = false;
}

void SearchOptimalWarping::setParameter(string name, float value)
{
    if (name == "rndFactor")
        rndFactor = value;
    else if (name == "nbSegBase")
        nbSegBase = value;
    else if (name == "initPopSize")
        initPopSize = value;
    else if (name == "matingPoolSize")
        matingPoolSize = value;
    else if (name == "paretoMaxSize")
        paretoMaxSize = value;
    else if (name == "maxPopSize")
        maxPopSize = value;
    else if (name == "nIter")
        nIter = value;
    else
        throw "SearchOptimalWarping::setParameter::Invalid, wrong parameter name";
}

map<string, boost::any> SearchOptimalWarping::getSearchParameters()
{
    map<string, boost::any> params;
    params["initPopSize"]               = initPopSize;
    params["maxPopSize"]                = maxPopSize;
    params["matingPoolSize"]            = matingPoolSize;
    params["paretoMaxSize"]             = paretoMaxSize;
    params["nIter"]                     = nIter;
    params["selectiveMutation"]         = selectiveMutation;
    params["globalProba"]               = globalProba;
    params["harmonicFix"]               = harmonicFix;
    params["harmonicFixFrequency"]      = harmonicFixFrequency;
    params["localOptim"]                = localOptim;
    params["adaptiveOptim"]             = adaptiveOptim;
    params["adaptiveOptimFrequency"]    = adaptiveOptimFrequency;
    params["TSIndex"]                   = TSIndex;
    return params;
}

void SearchOptimalWarping::setSearchParameters(map<string, boost::any> params)
{
    map<string, boost::any>::iterator it;
    for (it = params.begin(); it != params.end(); ++it)
    {
        if (it->first == "selectiveMutation")
            setSelectiveMutation((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "globalProba")
            setGlobalProba((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "harmonicFix")
            setHarmonicFix((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "localOptim")
            setLocalOptim((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "adaptiveOptim")
            setAdaptiveOptim((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "TSIndex")
            setTSIndex((bool)boost::any_cast<bool>(it->second));
        else if (it->first == "harmonicFixFrequency")
            setHarmonicFixFreq((int)boost::any_cast<int>(it->second));
        else if (it->first == "adaptiveOptimFrequency")
            setAdaptiveOptimFreq((int)boost::any_cast<int>(it->second));
        else
            setParameter(it->first, (int)boost::any_cast<int>(it->second));
    }
}

void SearchOptimalWarping::initialize()
{
    // Update variable domains
    sSession->getProduction()->getVariableDomains();
}

PopulationPtr SearchOptimalWarping::launchSearch(sqlStruct& targetFeatures)
{
    int i, j, t, ins, inst, n, k, l, m, seg, vT, op;
    // Get the target features
    //sqlStruct targetFeatures = sSession->getTarget()->getFeaturesList();
    vector<pair<string, vector<int>>>   variableDomains;
    vector<int>                         variableTable;
    vector<FeaturesPtr>                 optFeatures;
    vector<float>                       crossProbas, mutationProbas;
    try {
        // Retrieve the domains in which instruments can be chosen
        variableDomains = sSession->getProduction()->getVariableDomains();
        // Retrieve the variable table of indexes
        variableTable   = sSession->getProduction()->getVariableTable();
        // Retrieve the list of optimization features
        optFeatures     = sSession->getFeatures();
    } catch (const char* e) {
        throw e;
    }
    
    // Draw weights (Jaszkiewicz's method)
    matrixPtr weights;
    if (optFeatures.size() > 1)
        weights = draw_jaszkiewicz_weights(nIter, (int)optFeatures.size());
    else
    {
        weights = matrixPtr(new matrix(nIter, 1));
        weights->fill(1);
    }
    
    // Instantiate random initial population
    PopulationPtr               initPop(new Population(sSession));
    vector<int>                 onsetList;
    vector<matrixIPtr>          onsetsProposals(variableDomains.size(), matrixIPtr());
    vector<int>                 durationList;
    float                       targetDuration = targetFeatures.duration;
    int                         minStepDuration = _min((int)ceil(0.3 / (targetDuration / 128)), 128);
    map<string, indexTreePtr>   indexes;
    
    for (ins = 0; ins < variableDomains.size(); ins++)
        onsetsProposals[ins] = matrixIPtr(new matrixI(0, 0));
    for (i = 0; i < optFeatures.size(); i++)
    {
        if (optFeatures[i]->getFeatureName() == "PartialsAmplitude")
        {
            // If we want a temporal partials amplitude, perform a harmonic delta function
            vector<vector<string>> deltaFilters = vector<vector<string>>(64);
            // Retrieve the harmonic filters
            vector<vector<string>> hFilters = sSession->getTarget()->getHarmonicFilters();
            // For the first time instant, push up the non-empty partials
            for (n = 0; n < hFilters[0].size(); n++)
            {
                onsetList.push_back(0);
                // For each instrument we push the list of best propositions
                for (ins = 0; ins < sSession->getProduction()->getInstruments().size(); ins++)
                {
                    string key = sSession->getProduction()->getInstruments()[ins] + "-" + hFilters[0][n];
                    vector<int> proposals = sSession->getProduction()->getVariableOnsetsDomains()[key];
                    matrixIPtr onProp(new matrixI(3, (int)proposals.size()));
                    for (k = 0; k < proposals.size(); k++)
                    {
                        onProp->operator()(0, k) = proposals[k];
                        onProp->operator()(1, k) = 0;
                        for (l = 1; l < hFilters.size(); l++)
                        {
                            for (m = 0; m < hFilters[l].size(); m++)
                                if (hFilters[l][m] == hFilters[0][n])
                                    break;
                            if (m == hFilters[l].size())
                                break;
                        }
                        if (l > minStepDuration)
                        {
                            onProp->operator()(2, k) = l;
                            durationList.push_back(l);
                        }
                        else
                            onProp->operator()(2, k) = 0;
                    }
                }
            }
            // Then we do the same for each time point
            for (t = 1; t < hFilters.size(); t++)
            {
                for (n = 0; n < hFilters[t].size(); n++)
                {
                    if (std::find(hFilters[t - 1].begin(), hFilters[t - 1].end(), hFilters[t][n]) == hFilters[t - 1].end())
                    {
                        for (ins = 0; ins < variableDomains.size(); ins++)
                        {
                            string key = sSession->getProduction()->getInstruments()[ins] + "-" + hFilters[t][n];
                            vector<int> proposals = sSession->getProduction()->getVariableOnsetsDomains()[key];
                            matrixIPtr onProp(new matrixI(3, (int)proposals.size()));
                            for (k = 0; k < proposals.size(); k++)
                            {
                                onProp->operator()(0, k) = proposals[k];
                                onProp->operator()(1, k) = t;
                                for (l = 1; (l + t) < hFilters.size(); l++)
                                {
                                    for (m = 0; m < hFilters[(l + t)].size(); m++)
                                        if (hFilters[(l+t)][m] == hFilters[t][n])
                                            break;
                                    if (m == hFilters[(l + t)].size())
                                        break;
                                }
                                if (l > minStepDuration)
                                {
                                    onProp->operator()(2, k) = l;
                                    durationList.push_back(l);
                                }
                                else
                                    onProp->operator()(2, k) = 0;
                            }
                            if (!onsetsProposals[ins])
                                onsetsProposals[ins] = onProp;
                            else
                                onsetsProposals[ins]->merge2(onProp);
                        }
                        onsetList.push_back(t);
                    }
                }
            }
        }
        if (optFeatures[i]->getFeatureName() != "PartialsMeanAmplitude" && optFeatures[i]->getFeatureName() != "MelMeanAmplitude")
        {
            matrixPtr curFeat;
            if (sSession->getIsMultiTarget())
                curFeat = sSession->getTarget()->getFeature(targetFeatures, optFeatures[i]->getFeatureName());
            else
                curFeat = sSession->getTarget()->getFeature(optFeatures[i]->getFeatureName());
            if (curFeat->size() > 1 && !(curFeat->cols() > 1) && TSIndex)
            {
                vector<vector<float> > resultSegments;
                vector<int> tmpOn;
                vector<int> durations;
                tsMultiLevelSegment(curFeat, nbSegBase, resultSegments, tmpOn, durations);
                onsetList.insert(onsetList.end(), tmpOn.begin(), tmpOn.end());
                if (TSIndex)
                {
                    printf("Performing segmentation on %s\n", optFeatures[i]->getFeatureName().c_str());
                    // Using Time Series indexing technique to retrieve segmented best propositions on different warping scales
                    indexTreePtr indexDesc = constructIndexesFromDB(sSession->getKnowledge(), optFeatures[i]->getFeatureName());
                    indexes[optFeatures[i]->getFeatureName()] = indexDesc;
                    for (j = 0; j < resultSegments.size(); j++)
                    {
                        float           sDuration = (targetDuration * ((float)resultSegments[j].size() / 128.0));
                        if (sDuration < 0.5)
                            continue;
                        vector<int>     results;
                        indexNodePtr    resNode;
                        indexDesc->approximateQueryConstrained(resultSegments[j], variableTable, 5, sDuration, results, resNode);
                        for (inst = 0; inst < variableDomains.size(); inst++)
                        {
                            vector<int> index = isMember(results, variableDomains[inst].second);
                            vector<int> tmpProposals;
                            reorder(variableDomains[inst].second, index, tmpProposals);
                            if (!tmpProposals.empty())
                            {
                                matrixIPtr proposals(new matrixI(2, (int)tmpProposals.size()));
                                for (k = 0; k < tmpProposals.size(); k++)
                                    proposals->operator()(0, k) = tmpProposals[k];
                                for (k = 0; k < tmpProposals.size(); k++)
                                    proposals->operator()(1, k) = tmpOn[j];
                                if (!onsetsProposals[inst])
                                    onsetsProposals[inst] = proposals;
                                else
                                    onsetsProposals[inst]->merge2(proposals);
                            }
                        }
                    }
                }
                /*
                else
                {
                    for (j = 0; j < resultSegments.size(); j++)
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
                        //printf("Querying %s with %f\n", optFeatures[i]->getFeatureName().c_str(), curValue);
                        queries.push_back(Query("BETWEEN", optFeatures[i]->getFeatureName() + "Mean", vals));
                        sSession->getKnowledge()->getBDBConnector()->getSoundsQuery(queries, soundIDs);
                        for (inst = 0; inst < variableDomains.size(); inst++)
                        {
                            vector<int> index = isMember(soundIDs, variableDomains[inst].second);
                            vector<int> tmpProposals;
                            reorder(variableDomains[inst].second, index, tmpProposals);
                            if (!tmpProposals.empty())
                            {
                                matrixIPtr proposals(new matrixI(2, (int)tmpProposals.size()));
                                for (k = 0; k < tmpProposals.size(); k++)
                                    proposals->operator()(0, k) = tmpProposals[k];
                                for (k = 0; k < tmpProposals.size(); k++)
                                    proposals->operator()(1, k) = tmpOn[j];
                                if (!onsetsProposals[inst])
                                    onsetsProposals[inst] = proposals;
                                else
                                    onsetsProposals[inst]->merge2(proposals);
                            }
                        }
                    }
                }
                */
            }
        }
    }
    vecUnique(onsetList);
    vecUnique(durationList);
    printf("Generating constrained population ...\n");
    initPop->generateConstrainedPopulation(initPopSize, onsetsProposals, onsetList, durationList, 0.5, popSparsity);
    // Evaluate the initial population
    try {
        eval_population(initPop, targetFeatures, optFeatures);
    } catch (const char* e) {
        throw e;
    }
    printf("Generating constrained population ...\n");
#if DEBUG_MODE
    initPop->dumpPopulation(DEBUG_DIR + "0a.initPopulation", 0);
#endif
    // Remove NaN values from population
    initPop->discardNaN();
    matrixPtr pop_critr = initPop->getCriteria();
    // Extract the initial Pareto set
    bool *front = (bool *)calloc(pop_critr->size(), sizeof(bool));
    pop_critr->transpose();
    paretofront(front, pop_critr->c_data(), pop_critr->cols() , pop_critr->rows());
    pop_critr->transpose();
    //bool* front = paretoGroup(pop_critr);
    vector<int> loc;
    for (i = 0; i < pop_critr->rows(); i++)
        if (front[i])
            loc.push_back(i);
    free(front);
    PopulationPtr paretoPop = initPop->extractPopulation(loc);
    matrixPtr pareto_critr = paretoPop->getCriteria();
#if DEBUG_MODE
    sSession->getTarget()->dumpTargetFeatures(DEBUG_DIR + "target", targetFeatures.targetSpec, targetFeatures.targetMeanSpec);
    paretoPop->dumpPopulation(DEBUG_DIR + "0b.initParetoPop", 0);
#endif
    // Ideal and nadir estimations
    vector<float> nadir_estimate = pop_critr->Max(1);
    vector<float> ideal_estimate = pop_critr->Min(1);
    vector<float> critr_ranges;
    for (i = 0; i < nadir_estimate.size(); i++)
        critr_ranges.push_back(nadir_estimate[i] - ideal_estimate[i]);
    printf("Generating constrained population ...\n");
    for (i = 0; i < nIter; i++)
    {
        string progression = to_string((((float)i + 1) / (float)nIter) * 100);
        sendOSCMessage("/Orchids/progression/orchestrate", progression);
        printf("Exploring search space ...\t%s\n", progression.c_str());
        // ***** Compute fitness *****
        float*          row         = weights->getRow(i);
        vector<float>   pop_fitness = scalarize(pop_critr, row, weights->cols(), ideal_estimate, critr_ranges);
        // ***** Selection *****
        vector<int> selectedID;
        select_population(pop_fitness, matingPoolSize, selectedID);
        PopulationPtr offspringPop = initPop->extractPopulation(selectedID);
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/01.offspringPop", 0);
#endif
        // ***** Loudness-specific mutation *****
        if (selectiveMutation)
        {
            offspringPop->selectiveMutation(row);
            offspringPop->updatePopulationFeatures();
        }
        // ***** Crossover and mutation based on adaptive probabilities *****
        if (globalProba)
        {
            offspringPop->shuffleRows();
            pop_critr = offspringPop->getCriteria();
            crossProbas = compute_cross_probas(ideal_estimate, nadir_estimate, pop_critr, row);
            mutationProbas = compute_mutation_probas(ideal_estimate, nadir_estimate, pop_critr, row);
            // ***** Probabilistic mutations *****
            offspringPop->mutationProbas(mutationProbas);
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/02.offspringPop_Mut", 0);
#endif
            // ***** Probabilistic crossover *****
            offspringPop->crossoverProbas(crossProbas);
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/03.offspringPop_Cross", 0);
#endif
        }
        // ***** Normal crossover / mutations *****
        if (!globalProba)
        {
            offspringPop->crossover();
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/04.offspringPop_Cross", 0);
#endif
            offspringPop->mutation();
#if DEBUG_MODE
            offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/05.offspringPop_Mut", 0);
#endif
        }
        // ***** Compute offspring criteria and audio fitness *****
        offspringPop->updatePopulationFeatures();
        try { eval_population(offspringPop, targetFeatures, optFeatures); }
        catch (const char* e) { throw e; }
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/06.offspringPop_Eval", 0);
#endif
        matrixPtr offspring_critr = matrixPtr(new matrix(offspringPop->getCriteria()));
        if (mod<int>(i + 1, harmonicFixFrequency) == 0 && harmonicFix)
        {
            for (j = 0; j < optFeatures.size(); j++)
                if (optFeatures[j]->getFeatureName() == "PartialsMeanAmplitude")
                {
                    vector<int> Ibool, I;
                    PopulationPtr newPop, tmpPop;
                    if (optimFullPopulation)
                    {
                        Ibool = preserveDiversity(offspring_critr, optimPopSubsampling, "random");
                        for (k = 0; k < Ibool.size(); k++)
                            if (Ibool[k] == 1)
                                I.push_back(k);
                        tmpPop = offspringPop->extractPopulation(I);
                    }
                    else
                    {
                        Ibool = preserveDiversity(pareto_critr, optimPopSubsampling, "random");
                        for (k = 0; k < Ibool.size(); k++)
                            if (Ibool[k] == 1)
                                I.push_back(k);
                        tmpPop = paretoPop->extractPopulation(I);
                    }
#if DEBUG_MODE
                    tmpPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07a.harmonicFixed_Extracted", 0);
#endif
                    newPop = tmpPop->harmonicFix(targetFeatures, optimGenIndividuals, optimGenSubsampling);
                    if (newPop->getSolutionsIDs()->size() != 0)
                    {
                        newPop->updatePopulationFeatures();
                        try {
                            eval_population(newPop, targetFeatures, optFeatures);
                        } catch (const char* e) {
                            throw e;
                        }
                        offspringPop->combinePopulations(newPop);
                    }
#if DEBUG_MODE
                    if (newPop->getSolutionsIDs()->size() != 0)
                        newPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07b.harmonicFixed_Generated", 0);
#endif
                }
            offspring_critr = matrixPtr(new matrix(offspringPop->getCriteria()));
        }
        if (mod<int>(i + 2, adaptiveOptimFrequency) == 0 && adaptiveOptim)
        {
            vector<int> Ibool, I;
            PopulationPtr newPop, tmpPop;
            if (optimFullPopulation)
            {
                Ibool = preserveDiversity(offspring_critr, optimPopSubsampling, "random");
                for (k = 0; k < Ibool.size(); k++)
                    if (Ibool[k] == 1)
                        I.push_back(k);
                tmpPop = offspringPop->extractPopulation(I);
            }
            else
            {
                Ibool = preserveDiversity(pareto_critr, optimPopSubsampling, "random");
                for (k = 0; k < Ibool.size(); k++)
                    if (Ibool[k] == 1)
                        I.push_back(k);
                tmpPop = paretoPop->extractPopulation(I);
            }
#if DEBUG_MODE
            tmpPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07c.adaptiveFixed_Extracted", 0);
#endif
            newPop = paretoPop->adaptiveOptimization(targetFeatures, TSIndex, indexes, optimGenIndividuals, optimGenSubsampling);
            if (newPop->getSolutionsIDs()->size() != 0)
            {
                try {
                    newPop->updatePopulationFeatures();
                    eval_population(newPop, targetFeatures, optFeatures);
                    offspringPop->combinePopulations(newPop);
                } catch (const char* e) {
                    throw e;
                }
            }
            
#if DEBUG_MODE
            if (newPop->getSolutionsIDs()->size() != 0)
                newPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07b.harmonicFixed_Generated", 0);
#endif
            offspring_critr = matrixPtr(new matrix(offspringPop->getCriteria()));
        }
        offspringPop->discardNaN();
#if DEBUG_MODE
        offspringPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/07.offspringPop_NaN", 0);
#endif
        // Update ideal and nadir
        float* rowToMerge = (float*)malloc(nadir_estimate.size() * sizeof(float));
        for (j = 0; j < nadir_estimate.size(); j++)
            rowToMerge[j] = nadir_estimate[j];
        offspring_critr->quickmerge(rowToMerge);
        free(rowToMerge);
        nadir_estimate = offspring_critr->Max(1);
        offspring_critr->delRow(offspring_critr->rows() - 1);
        rowToMerge = (float*)malloc(ideal_estimate.size() * sizeof(float));
        for (j = 0; j < ideal_estimate.size(); j++)
            rowToMerge[j] = ideal_estimate[j];
        offspring_critr->quickmerge(rowToMerge);
        free(rowToMerge);
        ideal_estimate = offspring_critr->Min(1);
        critr_ranges.clear();
        for (j = 0; j < nadir_estimate.size(); j++)
            critr_ranges.push_back(nadir_estimate[j] - ideal_estimate[j]);
        // ***** Insert offspring in population *****
        initPop->combinePopulations(offspringPop);
#if DEBUG_MODE
        initPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/08.initPop_Combined", 0);
#endif
        // ***** Update pareto set *****
        paretoPop->combinePopulations(initPop);
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/09.offspringPop_Combined", 0);
#endif
        // ***** Keep only the unique solutions *****
        matrixPtr pareto_critr_tmp = paretoPop->uniqueSolutions();
        bool *paretoFront = (bool *)calloc(pareto_critr_tmp->rows(), sizeof(bool));
        pareto_critr_tmp->transpose();
        paretofront(paretoFront, pareto_critr_tmp->c_data(), pareto_critr_tmp->cols() , pareto_critr_tmp->rows());
        pareto_critr_tmp->transpose();
        vector<int> index;
        for (j = 0; j < pareto_critr_tmp->rows(); j++)
            if (paretoFront[j])
                index.push_back(j);
        free(paretoFront);
        paretoPop = paretoPop->extractPopulation(index);
        // ***** Pareto-wise local optimization *****
        if (localOptim)
        {
            paretoPop->localOptimization(targetFeatures);
            //paretoPop->combinePopulations(optimPop);
            paretoPop->updatePopulationFeatures();
            try { eval_population(paretoPop, targetFeatures, optFeatures); }
            catch (const char* e) { throw e; }
            //pareto_critr_tmp->deallocate();
            pareto_critr_tmp = paretoPop->uniqueSolutions();
            paretoFront = (bool *)calloc(pareto_critr_tmp->size(), sizeof(bool));
            pareto_critr_tmp->transpose();
            paretofront(paretoFront, pareto_critr_tmp->c_data(), pareto_critr_tmp->cols() , pareto_critr_tmp->rows());
            pareto_critr_tmp->transpose();
            index.clear();
            for (j = 0; j < pareto_critr_tmp->rows(); j++)
                if (paretoFront[j])
                    index.push_back(j);
            free(paretoFront);
            paretoPop = paretoPop->extractPopulation(index);
        }
        pareto_critr = paretoPop->getCriteria();
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/10.paretoPop", 0);
#endif
        //pop_critr->deallocate();
        pop_critr = initPop->getCriteria();
        
        // ***** Population size handling *****
        vector<int> Jbool = pop_critr->isMember(pareto_critr);
        vector<int> J;
        for (j = 0; j < Jbool.size(); j++)
            if (Jbool[j] == 0)
                J.push_back(j);
        
        matrixPtr dominated_critr(new matrix(pop_critr));
        dominated_critr->reorder(J);
        // Retriving dominated solutions
        PopulationPtr dominatedPop = initPop->extractPopulation(J);
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/11.dominatedPop", 0);
#endif
        // Reduce pareto set size if too large
        if (paretoPop->getNbSolutions() > paretoMaxSize)
        {
            vector<int> Ibool = preserveDiversity(pareto_critr, paretoMaxSize, "random");
            vector<int> I;
            for (j = 0; j < Ibool.size(); j++)
                if (Ibool[j] == 1)
                    I.push_back(j);
            paretoPop = paretoPop->extractPopulation(I);
            pareto_critr = paretoPop->getCriteria();
        }
#if DEBUG_MODE
        paretoPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/12.paretoPopReduced", 0);
#endif
        // Preserve density for dominated individuals
        if (dominated_critr->rows() > (maxPopSize - paretoPop->getNbSolutions()))
            dominatedPop->smoothPopulation(maxPopSize - paretoPop->getNbSolutions());
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/13.dominatedPopReduced", 0);
#endif
        // Mix reduced Pareto set and individuals of homogeneous density into next generation
        dominatedPop->combinePopulations(paretoPop);
#if DEBUG_MODE
        dominatedPop->dumpPopulation(DEBUG_DIR + "/iter_" + to_string(i) + "/14.terminalPop", 0);
#endif
        initPop     = dominatedPop;
        pop_critr   = initPop->getCriteria();
        
    }
    // Build output structure
    solutionSet = paretoPop->getSolutions();
    // Rescale criterias
    pareto_critr = paretoPop->getCriteria();
    vector<float> minsEnds = pareto_critr->Min(1);
    printf("%d - %d\n", pareto_critr->rows(), pareto_critr->cols());
    for (i = 0; i < pareto_critr->rows(); i++)
        for (j = 0; j < pareto_critr->cols(); j++)
            pareto_critr->operator()(i, j) -= minsEnds[j];
    minsEnds.empty();
    minsEnds = pareto_critr->Max(1);
    for (i = 0; i < pareto_critr->rows(); i++)
        for (j = 0; j < pareto_critr->cols(); j++)
            pareto_critr->operator()(i, j) /= minsEnds[j];
    pareto_critr->operator+(0.1);
#if DEBUG_MODE
    paretoPop->getCriteria()->print();
    paretoPop->dumpPopulation(DEBUG_DIR + "/finalSolutions", 1);
#endif

    return paretoPop;
}