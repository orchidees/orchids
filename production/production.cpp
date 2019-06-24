//
//  production.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "production.h"

Production::Production(Session* sessObj, vector<string> &instrus, int resolution)
{
    sSession            = sessObj;
    allowedInstruments  = instrus;
    allowedResolutions.push_back(1);
    allowedResolutions.push_back(2);
    allowedResolutions.push_back(4);
    allowedResolutions.push_back(8);
    setResolution(resolution);
    harmonicFiltering   = true;
    staticMode          = false;
    isInited            = false;
}

void Production::setHarmonicFiltering(bool hFilt)
{
    harmonicFiltering = hFilt;
    needInit();
}

void Production::setStaticMode(bool sMode)
{
    staticMode = sMode;
    maxOnsetValues.clear();
    maxDurationValues.clear();
    filteredOnsets.clear();
    filteredDurations.clear();
    sSession->getTarget()->emptyHarmonicFilters();
    needInit();
}

void Production::setResolution(int res)
{
    if (std::find(allowedResolutions.begin(), allowedResolutions.end(), res) == allowedResolutions.end())
    {
        string err = "Production:setResolution:BadResolution, Allowed values for microtonic resolution: " + to_string(allowedResolutions[0]) + to_string(allowedResolutions[1]) + to_string(allowedResolutions[2]) + to_string(allowedResolutions[3]) + "(fractions of semitones)";
        throw err;
    }
    microtonicResolution = res;
}

void Production::setInstruments(vector<string> instrus)
{
    allowedInstruments = instrus;
}

void Production::setFilters(vector<boost::shared_ptr<Filters> > filters)
{
    filtersSet = filters;
}

vector<string> Production::getDefaultOrchestra()
{
    return vector<string>();
}

void Production::emptyVariableDomains()
{
    variableDomains.clear();
    variableTable.clear();
    variableResolution.clear();
    variableIndividuals.clear();
    variableOnsetsIDs.clear();
    variableOnsetsDomains.clear();
    variableDurationsDomains.clear();
    if (sSession->getTarget() != nullptr)
        sSession->getTarget()->emptyHarmonicFilters();
}

vector<string> &Production::getInstruments()
{
    return allowedInstruments;
}

int Production::getResolution()
{
    return microtonicResolution;
}

vector<int> &Production::getMaxOnsetValues()
{
    return maxOnsetValues;
}

vector<vector<int> > &Production::getFilteredOnsets()
{
    return filteredOnsets;
}

vector<int> &Production::getMaxDurationValues()
{
    return maxDurationValues;
}

vector<vector<int> > &Production::getFilteredDurations()
{
    return filteredDurations;
}

vector<pair<string, vector<int> > > &Production::getVariableDomains()
{
    if (variableDomains.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableDomains;
}

map<string, vector<int> > &Production::getVariableOnsetsDomains()
{
    if (variableOnsetsDomains.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableOnsetsDomains;
}

map<string, vector<int> > &Production::getVariableOnsetsIDs()
{
    if (variableOnsetsIDs.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableOnsetsIDs;
}

map<string, vector<int> > &Production::getVariableDurationsDomains()
{
    if (variableDurationsDomains.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableDurationsDomains;
}

vector<int> &Production::getVariableTable()
{
    if (variableTable.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableTable;
}

vector< vector<int> > &Production::getVariableResolution()
{
    if (variableResolution.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableResolution;
}

vector<IndividualPtr> &Production::getVariableIndividuals()
{
    if (variableIndividuals.empty() || !isInited)
    {
        try {
            computeVariableDomains();
        } catch (const char* e) {
            throw e;
        }
    }
    return variableIndividuals;
}

vector<boost::shared_ptr<Filters> > Production::getFilters()
{
    return filtersSet;
}

void Production::resetFilters()
{
    filtersSet.clear();
}

void Production::needInit()
{
    isInited = false;
}

bool Production::isStatic()
{
    return staticMode;
}

void Production::constructBaseFilters()
{
    if (filtersSet.size() != 0)
        return;

    KnowledgeBDBPtr k                   = sSession->getKnowledge();
    attributeDomains                    = k->getAttributeDomains();
    knowledgeDescription knowledgeDesc  = k->getFieldsList();
    int i;
    for (i = 0; i < knowledgeDesc.fields.size(); i++)
    {
        if (knowledgeDesc.types[i] == "string")
        {
            boost::shared_ptr<FiltersSymbolic> f(new FiltersSymbolic(sSession, knowledgeDesc.fields[i], knowledgeDesc.types[i]));
            f->setValuesList(std::get<2>(attributeDomains[knowledgeDesc.fields[i]]));
            f->setMode("bypass");
            filtersSet.push_back(f);
        }
        else if (knowledgeDesc.types[i] == "float")
        {
            boost::shared_ptr<FiltersSpectral> f(new FiltersSpectral(sSession, knowledgeDesc.fields[i], knowledgeDesc.types[i]));
            f->setValuesRange( pair<float, float>(std::get<0>(attributeDomains[knowledgeDesc.fields[i]]), std::get<1>(attributeDomains[knowledgeDesc.fields[i]])) );
            f->setMode("bypass");
            filtersSet.push_back(f);
        }
        // descriptor / array / array2D
        else
        {
            boost::shared_ptr<FiltersTemporal> f(new FiltersTemporal(sSession, knowledgeDesc.fields[i], knowledgeDesc.types[i]));
            f->setMode("bypass");
            filtersSet.push_back(f);
        }
    }
}

void Production::computeVariableDomains()
{
    KnowledgeBDBPtr knowledge = sSession->getKnowledge();
    if (filtersSet.size() == 0)
        constructBaseFilters();
    int                         i, j, k, l, m;
    int                         ntrElem     = knowledge->getNeutralID();
    vector<vector<string> >     hmFilters   = sSession->getTarget()->getHarmonicFilters();
    vector<string>              hmNotes     = sSession->getTarget()->getUniqueHarmonicFilters();
    // Iterate on instruments
    for (i = 0; i < allowedInstruments.size(); i++)
    {
        vector<Query>   queries;
        //Vector of results
        vector<int>     soundIDs;
        string          instrument = allowedInstruments[i];
        printf("Preparing search spaces : %s\n", instrument.c_str());

        // Add all instrument (or instrument group) sounds to domain
        // Iterate on filters
        for (j = 0; j < filtersSet.size(); j++)
        {
            if (filtersSet[j]->getName() == "instrument")
            {
                Query q("REGEXP", "instrument", instrument);
                queries.push_back(q);
                if (filtersSet[j]->getMode() != "bypass")
                {
                    // Query knowledge for matching items and reduce the variable domain
                    Query q("REGEXP", filtersSet[j]->getName());
                    vector<string> values = boost::static_pointer_cast<FiltersSymbolic>(filtersSet[j])->apply();
                    string queryValues;
                    for (k = 0; k < values.size(); k++)
                        queryValues = queryValues + values[k] + "|";
                    // Remove last "|"
                    queryValues = queryValues.substr(0, queryValues.size() - 1);
                    q.setValue(queryValues);
                    queries.push_back(q);
                }
            }
            else if (filtersSet[j]->getName() == "note")
            {
                if (harmonicFiltering)
                {
                    Query q("REGEXP", "note");
                    string notes;
                    for (k = 0; k < hmNotes.size(); k++)
                        notes = notes + hmNotes[k] + "|";
                    // Remove last "|"
                    notes = notes.substr(0, notes.size() - 1);
                    q.setValue(notes);
                    queries.push_back(q);
                }
                else
                {
                    // No harmonic filtering = take all notes from the database. So we don't need to make a query.
                    continue;
                }
            }
            else
            {
                if (filtersSet[j]->getMode() != "bypass")
                {
                    if (filtersSet[j]->getAttribute() == "string")
                    {
                        // Query knowledge for matching items and reduce the variable domain
                        Query q("REGEXP", filtersSet[j]->getName());
                        vector<string> values = boost::static_pointer_cast<FiltersSymbolic>(filtersSet[j])->apply();
                        string queryValues;
                        for (k = 0; k < values.size(); k++)
                            queryValues = queryValues + values[k] + "|";
                        // Remove last "|"
                        queryValues = queryValues.substr(0, queryValues.size() - 1);
                        q.setValue(queryValues);
                        queries.push_back(q);
                    }
                    else if (filtersSet[j]->getAttribute() == "float")
                    {
                        Query q(filtersSet[j]->getMode(), filtersSet[j]->getName());
                        pair<float, float> p = boost::static_pointer_cast<FiltersSpectral>(filtersSet[j])->apply();
                        q.setValue(p);
                        queries.push_back(q);
                    }
                }
            }
        }
        // Query knowledge for matching items
        for (j = 0; j < queries.size(); j++)
            queries[j].print();
        knowledge->computeQuery(queries, soundIDs);
        soundIDs.push_back(ntrElem);
        variableDomains.push_back(pair<string, vector<int> >(instrument, soundIDs));
        variableTable.insert(variableTable.end(), soundIDs.begin(), soundIDs.end() - 1);
    }

    //Final variable table
    std::sort(variableTable.begin(), variableTable.end());
    std::vector<int>::iterator it;
    it = std::unique (variableTable.begin(), variableTable.end());
    variableTable.resize( std::distance(variableTable.begin(),it) );
    
    if (variableTable.size() == 0)
        throw "Production::emptySearchSpace, Search space is empty ! Check filters and orchestra";
    
    printf("Evaluating search sub-spaces\n");
    // Retrieve the duration of features to know the max onset value
    vector<string>  noteFeatures;
    vector<float>   durationFeatures;
    vector<string>  instrument;
    knowledge->getFieldsValues("duration", variableTable, noteFeatures, durationFeatures);
    knowledge->getFieldsValues("note", variableTable, noteFeatures, durationFeatures);
    knowledge->getFieldsValues("instrument", variableTable, instrument, durationFeatures);
    // Constructing variableOnsetsDomains
    for (i = 0; i < variableTable.size(); i++)
    {
        string key = instrument[i] + "-" + noteFeatures[i];
        variableOnsetsIDs[key].push_back(i);
        variableOnsetsDomains[key].push_back(variableTable[i]);
        variableDurationsDomains[key].push_back(variableTable[i]);
        variableResolution.push_back(vector<int>());
        variableResolution[i].push_back(0);
        if (harmonicFiltering && microtonicResolution > 1)
        {
            for (j = 0; j < hmNotes.size(); j++)
                if ((strncmp(noteFeatures[i].c_str(), hmNotes[j].c_str(), noteFeatures[i].length())) == 0)
                    if (hmNotes[j][noteFeatures[i].length()] == '+')
                    {
                        int shift = hmNotes[j][noteFeatures[i].length() + 1] - '0';
                        int res = hmNotes[j][noteFeatures[i].length() + 3] - '0';
                        res = (res == 1 ? 16 : res);
                        variableResolution[i].push_back(shift * ((microtonicResolution * 2) / res));
                    }
        }
        else
        {
            for (j = 1; j < microtonicResolution; j++)
                variableResolution[i].push_back(j);
        }
    }
    vector<int> ntrRes; ntrRes.push_back(0);
    variableResolution.push_back(ntrRes);
    variableTable.push_back(ntrElem);
    
    
    // Obtain related notes
    // Retrieving duration of the target for relative onsets
    float targetDuration = sSession->getTarget()->getFeaturesList().duration;
    int minStepDuration = _min((int)ceil(0.3 / (targetDuration / 128)), 128);
    
    // Constructing array of individuals
    for (i = 0; i < variableTable.size(); i++)
        variableIndividuals.push_back(IndividualPtr(new Individual(0, 0)));
    
    // Constructing array of maximal onsets
    if (!staticMode)
    {
        map<string, vector<int> > hmOnsets;
        map<string, vector<int> > hmDurations;
        if (harmonicFiltering)
        {
            //hmNotes.erase(hmNotes.begin());
            for (i = 0; i < hmNotes.size(); i++)
                for (j = 0; j < hmFilters.size(); j++)
                    for (k = 0; k < hmFilters[j].size(); k++)
                        if (hmFilters[j][k] == hmNotes[i])
                        {
                            string note = hmNotes[i];
                            replaceAll<string>(note, "#", "d");
                            for (l = 1; (l + j) < hmFilters.size(); l++)
                            {
                                for (m = 0; m < hmFilters[l + j].size(); m++)
                                    if (hmFilters[l + j][m] == hmNotes[i])
                                        break;
                                if (m == hmFilters[l + j].size())
                                    break;
                            }
                            if (l >= minStepDuration)
                            {
                                //printf("Onset = %d - %d\n", j, l);
                                hmOnsets[note].push_back(j);
                                hmDurations[note].push_back(l);
                            }
                        }
        }
        
        for (i = 0; i < variableTable.size(); i++)
        {
            maxOnsetValues.push_back( _max(0, 128 - (floor((durationFeatures[i] / targetDuration) * 128) + 1)));
            maxDurationValues.push_back(floor((durationFeatures[i] / targetDuration) * 128) + 1);
        }
        maxOnsetValues.push_back(0);
        printf("Evaluating search sub-spaces\n");
        for (i = 0; i < variableTable.size() - 1; i++)
        {            
            if (harmonicFiltering)
            {
                string instNote = noteFeatures[i];
                replaceAll<string>(instNote, "#", "d");
                filteredOnsets.push_back(hmOnsets[instNote]);
                filteredDurations.push_back(hmDurations[instNote]);
            }
            else
            {
                filteredOnsets.push_back(linspace(0, maxOnsetValues[i], maxOnsetValues[i] + 1));
                filteredDurations.push_back(linspace(minStepDuration, maxDurationValues[i], (maxDurationValues[i] - minStepDuration) + 1));
            }
        }
        // For the neutral element
        filteredOnsets.push_back(vector<int>());
        filteredDurations.push_back(vector<int>());
    }
    
    printf("Evaluating search sub-spaces\n");
    // Computing neutral element values
    IndividualPtr neutralInd = variableIndividuals[variableIndividuals.size() - 1];
    neutralInd->setInstrument(ntrElem);
    neutralInd->setOnset(0);
    neutralInd->setDuration(0);
    // Retrieve the optimization features
    vector<FeaturesPtr> optFeatures = sSession->getFeatures();
    // Retrieving each neutral value
    for (i = 0; i < optFeatures.size(); i++)
    {
        string fName    = optFeatures[i]->getFeatureName();
        matrixPtr fValue   = optFeatures[i]->neutral();
        neutralInd->setFeature(fName, fValue);
    }
    // Adding the neutral generic features
    matrixPtr neutral(new matrix(1, 1));
    matrixPtr neutralArray(new matrix(1, 128));
    matrixPtr neutralPartials(new matrix(maxFreq, 64));
    matrixPtr neutralPartialsEnergy(new matrix(1, 64));
    neutralInd->setFeature("duration", neutral);
    neutralInd->setFeature("Loudness", neutralArray);
    neutralInd->setFeature("LoudnessMean", neutral);
    neutralInd->setFeature("LoudnessFactor", neutral);
    neutralInd->setFeature("EnergyEnvelope", neutralArray);
    neutralInd->setFeature("EnergyEnvelopeMean", neutral);
    neutralInd->setFeature("SpectralCentroid", neutralArray);
    neutralInd->setFeature("SpectralCentroidMean", neutral);
    neutralInd->setFeature("PartialsFrequency", neutralPartials);
    neutralInd->setFeature("PartialsMeanEnergy", neutral);
    neutralInd->setFeature("PartialsEnergy", neutralPartialsEnergy);
    neutralInd->computeFinalFeatures();
    variableIndividuals[variableIndividuals.size() - 1] = neutralInd;
    if (!staticMode)
    {
        maxOnsetValues[maxOnsetValues.size() - 1] = 0;
        maxDurationValues[maxOnsetValues.size() - 1] = 0;
    }
    if (staticMode)
    {
        maxOnsetValues.empty();
        maxDurationValues.empty();
        for (i = 0; i < variableTable.size(); i++)
        {
            maxOnsetValues.push_back(0);
            maxDurationValues.push_back(0);
        }
        maxOnsetValues.push_back(0);
        maxDurationValues.push_back(0);
    }
    isInited = true;
}

void Production::updateIndividualFeatures(matrixIPtr idSetMatrix, vector<IndividualPtr>& uInds, vector<int>& idSet)
{
    int i, j, k, l, col;
    // Sorting ids and keeping uniques
    vector<int> uniqueIdSet(idSetMatrix->begin(), idSetMatrix->end());
    vecUnique<int>(uniqueIdSet);
    idSet = uniqueIdSet;
    // Individuals which need update
    vector<int> needUpdate, updateIDs;

    // Retrieving features
    sqlStruct       targetFeatures  = sSession->getTarget()->getFeaturesList();
    vector<string>  compFeatures    = sSession->getTotalFeaturesList();
    float           targetDuration  = targetFeatures.duration;
    
    // Checking which individuals need update
    vector<int> curID = isMember(uniqueIdSet, variableTable);
    for (i = 0; i < curID.size(); i++)
        if (!variableIndividuals[curID[i]]->getIsFilled())
        {
            needUpdate.push_back(uniqueIdSet[i]);
            updateIDs.push_back(curID[i]);
        }
    
    if (needUpdate.size() != 0)
    {
        knowledgeDescription a  = sSession->getKnowledge()->getFieldsList();
        vector<int> fId         = isMember(compFeatures, a.fields);
        
        // Retrieve the whole set of features for the allowed instruments
        map<int, vector<matrixPtr> > features;
        sSession->getKnowledge()->getBDBConnector()->getMultipleFinalOptimized(needUpdate, compFeatures, fId, features, true);
        for (i = 0; i < updateIDs.size(); i++)
        {
            // If ID was found
            if (features.find(needUpdate[i]) != features.end())
            {
                // Get duration
                float curDuration = features[needUpdate[i]][compFeatures.size() - 1]->operator()(0);
                for (j = 0; j < compFeatures.size(); j++)
                {
                    if (features[needUpdate[i]][j]->cols() > 1 && compFeatures[j] != "MelMeanAmplitude")
                    {
                        if (features[needUpdate[i]][j]->rows() == 1)
                        {
                            // Temporal descriptor case
                            matrixPtr   curSeries       = features[needUpdate[i]][j];
                            int         size            = curSeries->size();
                            int         nbSteps         = floor((curDuration / targetDuration) * (float)size) + 1;
                            float*      resampleSeries  = (float*)malloc(nbSteps * sizeof(float));
                            resampleData(curSeries->c_data(), size, resampleSeries, nbSteps, 4);
                            matrixPtr  resampleCurSeries(new matrix(1, size));
                            for (l = 0; l < nbSteps && l < size; l++)
                                resampleCurSeries->operator()(l) = resampleSeries[l];
                            free(resampleSeries);
                            variableIndividuals[updateIDs[i]]->setFeature(compFeatures[j], resampleCurSeries);
                        }
                        else
                        {
                            if (compFeatures[j] == "PartialsFrequency")
                                continue;
                            // PartialsAmplitude/Frequency and MelAmplitude case
                            matrixPtr solSpec;
                            if (compFeatures[j] == "PartialsAmplitude")
                            {
                                matrixPtr curAmps = features[needUpdate[i]][j];
                                matrixPtr curFreq = features[needUpdate[i]][j - 1];
                                // Convert to spectrogram
                                solSpec = matrixPtr(new matrix(maxFreq, 64));
                                for (k = 0; k < 64; k++)
                                    for (l = 0; l < curFreq->rows(); l++)
                                        if (curAmps->operator()(l, k) > 0 && curFreq->operator()(l, k) > 20 && curFreq->operator()(l, k) < 22000)
                                            solSpec->operator()(round(90.0f + (24.0f * log2f(curFreq->operator()(l, k) / 220.0f))), k) += curAmps->operator()(l, k);
                            }
                            else
                                solSpec = features[needUpdate[i]][j];
                            
                            // Resampling
                            int     dimArray        = solSpec->rows();
                            int     originalSteps   = solSpec->cols();
                            int     nbSteps         = floor((curDuration / targetDuration) * originalSteps) + 1;
                            matrixPtr  resampleCurArray(new matrix(dimArray, originalSteps));
                            //printf("%24s %f %f %d %d.\n", compFeatures[j].c_str(), curDuration, targetDuration, originalSteps, nbSteps);
                            #pragma omp parallel for shared(solSpec, resampleCurArray) private(col)
                            for (k = 0; k < dimArray; k++)
                            {
                                float* curArrayRow      = solSpec->getRow(k);
                                float* resampleCurRow   = (float*)malloc(nbSteps * sizeof(float));
                                resampleData(curArrayRow, originalSteps, resampleCurRow, nbSteps, 4);
                                for (col = 0; col < nbSteps && col < originalSteps; col++)
                                    resampleCurArray->operator()(k, col) = resampleCurRow[col];
                                free(resampleCurRow);
                            }
                            variableIndividuals[updateIDs[i]]->setFeature(compFeatures[j], resampleCurArray);
                        }
                    }
                    else
                    {
                        // One value descriptors (Mean/duration) and PartialsMean/MelMean case
                        if (compFeatures[j] == "PartialsMeanAmplitude")
                        {
                            // Convert to spectrogram
                            matrixPtr curMeanAmps = features[needUpdate[i]][j];
                            // Retrieve partialsFrequency
                            matrixPtr curMeanFreq = features[needUpdate[i]][j - 1];
                            matrixPtr solSpec(new matrix(maxFreq, 1));
                            for (l = 0; l < curMeanFreq->size(); l++)
                                if (curMeanAmps->operator()(l) > 0 && curMeanFreq->operator()(l) > 20 && curMeanFreq->operator()(l) < 22000)
                                    solSpec->operator()(round(90.0f + (24.0f * log2f(curMeanFreq->operator()(l) / 220.0f)))) = curMeanAmps->operator()(l);
                            variableIndividuals[updateIDs[i]]->setFeature(compFeatures[j], solSpec);
                        }
                        else
                            variableIndividuals[updateIDs[i]]->setFeature(compFeatures[j], features[needUpdate[i]][j]);
                    }
                }
            }
        }
    }
    uInds.clear();
    for (i = 0; i < curID.size(); i++)
        uInds.push_back(variableIndividuals[curID[i]]);
}



ProductionOrchestra::ProductionOrchestra(Session* mSession, vector<string> instrus, int resolution)
: Production(mSession, instrus, resolution)
{
    setInstruments(instrus);
}

vector<string> ProductionOrchestra::getDefaultOrchestra()
{
    string orchestraArray[17] = {"Fl", "Fl", "Ob", "Ob", "ClBb", "ClBb", "Bn", "Bn", "Hn", "Hn", "TpC", "TpC", "Vn", "Vn", "Va", "Vc", "Cb"};
    vector<string> orchestra(orchestraArray, orchestraArray + 17);
    return orchestra;
}

vector<string> ProductionOrchestra::getUserInstruments()
{
    // Get all instruments from database
    vector<string> instlist;
    sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("instrument", instlist);
    return instlist;
}

vector<string> ProductionOrchestra::getUserPlayingStyles()
{
    // Get all playing styles from database
    vector<string> instlist;
    sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("playingStyle", instlist);
    return instlist;
}

vector<string> ProductionOrchestra::getSources()
{
    // Get all sources from database
    vector<string> sourcelist;
    sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("source", sourcelist);
    // Return the list
    return sourcelist;
}

void ProductionOrchestra::setInstruments(vector<string> instrus)
{
    if (instrus.empty())
    {
        // Assign default instrument list if needed
        sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("instrument", allowedInstruments);
        return;
    }
    
    int i, j;
    // Get allowed instruments from database
    vector<string> fullInstruments;
    sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("instrument", fullInstruments);
    for (i = 0; i < instrus.size(); i++)
    {
        boost::regex e("[|]|[+]", boost::regex::normal | boost::regbase::icase);
        vector<string> l;
        string str = instrus[i];
        boost::regex_split(std::back_inserter(l), str, e);
        for (j = 0; j < l.size(); j++)
        {
            vector<string>::iterator it = std::find(fullInstruments.begin(), fullInstruments.end(), l[j]);
            if (it == fullInstruments.end())
            {
                printf("Warning: Instrument '%s' is not a valid instrument\n", instrus[i].c_str());
                instrus.erase(instrus.begin() + i);
                i--;
                break;
            }
        }
    }
    allowedInstruments = instrus;
}

vector<string> ProductionOrchestra::getScoreOrder()
{
    int i, j;
    // Get allowed instruments from database
    vector<string> instlist;
    sSession->getKnowledge()->getBDBConnector()->getSecondaryIndexList("instrument", instlist);
    // Get standardized nomenclature
    structNomenclature known_nomenclature = nomenclature();
    vector<string> known_families, known_instruments;
    vector<int> family_index;
    for (i = 0; i < known_nomenclature.size(); i++)
    {
        known_families.push_back(get<0>(known_nomenclature[i]));
        known_instruments.push_back(get<2>(known_nomenclature[i]));
        family_index.push_back(0);
    }
    for (i = 1; i < known_families.size(); i++)
        if (known_families[i] == known_families[i - 1])
            family_index[i] = family_index[i - 1];
        else
            family_index[i] = family_index[i - 1] + 1;
    // Initialize instrument rank vector
    vector<float> sort_instlist;
    for (i = 0; i < instlist.size(); i++)
        sort_instlist.push_back(0);
    // Iterate on instrument list
    for (i = 0; i < instlist.size(); i++)
    {
        // Is current instrument in the nomenclature?
        vector<string>::iterator it = std::find(known_instruments.begin(), known_instruments.end(), instlist[i]);
        // If yes ...
        if (it != known_instruments.end())
        {
            int idx = (int)(it - known_instruments.begin());
            // ... compute current instrument's rank
            string this_family = known_families[idx];
            vector<int> family_idx;
            for (j = 0; j < known_families.size(); j++)
                if (this_family == known_families[j])
                    family_idx.push_back(j);
            vector<int>::iterator it_family = std::find(family_idx.begin(), family_idx.end(), idx);
            int index_family = (int)(it_family - family_idx.begin());
            sort_instlist[i] = (float)family_index[idx] + ((float)index_family / (float)family_idx.size());
        }
        else
            printf("Warning: Instrument '%s' is not a valid instrument\n", instlist[i].c_str());
    }
    vector<int> I;
    mySort(sort_instlist, (int)sort_instlist.size(), sort_instlist, I);
    reorder(instlist, I, instlist);
    
    return instlist;
}

string ProductionOrchestra::get_family(string instrument)
{
    // Get all the database entries for the input instrument
    symbolics val;
    Dbt key;
    init_dbt(&key, (char*)instrument.c_str(), (int)(strlen(instrument.c_str()) + 1));
    sSession->getKnowledge()->getBDBConnector()->getSymbolics("instrument", &key, val);
    // Get the family of the first entry
    return val.family;
}

structNomenclature ProductionOrchestra::nomenclature()
{
    structNomenclature n;
    
    n.push_back(make_tuple("Flutes",            "Piccolo",                  "Picc"));
    n.push_back(make_tuple("Flutes",            "Flute",                    "Fl"));
    n.push_back(make_tuple("Flutes",            "Alto-Flute",               "AlFl"));
    n.push_back(make_tuple("Flutes",            "Bass-Flute",               "BFl"));
    n.push_back(make_tuple("Flutes",            "Contrabass-Flute",         "CbFl"));

    n.push_back(make_tuple("Oboes",             "Oboe",                     "Ob"));
    n.push_back(make_tuple("Oboes",             "Oboe-dAmore",              "ObAm"));
    n.push_back(make_tuple("Oboes",             "English-Horn",             "EH"));
    n.push_back(make_tuple("Oboes",             "Heckelphone",              "Heck"));
    
    n.push_back(make_tuple("Clarinets",         "Piccolo-Clarinet-Ab",      "PiClAb"));
    n.push_back(make_tuple("Clarinets",         "Clarinet-Eb",              "ClEb"));
    n.push_back(make_tuple("Clarinets",         "Piccolo-Clarinet-D",       "PiClD"));
    n.push_back(make_tuple("Clarinets",         "Clarinet-C",               "ClC"));
    n.push_back(make_tuple("Clarinets",         "Clarinet-Bb",              "ClBb"));
    n.push_back(make_tuple("Clarinets",         "Clarinet-A",               "ClA"));
    n.push_back(make_tuple("Clarinets",         "Basset-Horn-F",            "BstHn"));
    n.push_back(make_tuple("Clarinets",         "Alto-Clarinet-Eb",         "AlClEb"));
    n.push_back(make_tuple("Clarinets",         "Bass-Clarinet-Bb",         "BClBb"));
    n.push_back(make_tuple("Clarinets",         "Bass-Clarinet-A",          "BsClA"));
    n.push_back(make_tuple("Clarinets",         "Contra-Alto-Clarinet-Eb",  "CtAlClEb"));
    n.push_back(make_tuple("Clarinets",         "Contrabass-Clarinet-Bb",   "CbClBb"));

    n.push_back(make_tuple("Bassoons",          "Bassoon",                  "Bn"));
    n.push_back(make_tuple("Bassoons",          "Contrabassoon",            "Cbn"));

    n.push_back(make_tuple("Saxophones",        "Soprano-Sax",              "SoSax"));
    n.push_back(make_tuple("Saxophones",        "Alto-Sax",                 "ASax"));
    n.push_back(make_tuple("Saxophones",        "Tenor-Sax",                "TnSax"));
    n.push_back(make_tuple("Saxophones",        "Baritone-Sax",             "BarSax"));
    n.push_back(make_tuple("Saxophones",        "Bass-Sax",                 "BsSax"));
    n.push_back(make_tuple("Saxophones",        "Contrabass-Sax",           "CbSax"));

    n.push_back(make_tuple("Horns",             "Horn",                     "Hn"));
    
    n.push_back(make_tuple("Trumpets",          "Piccolo-Trumpet-Bb",       "PiTpBb"));
    n.push_back(make_tuple("Trumpets",          "Piccolo-Trumpet-A",        "PiTpA"));
    n.push_back(make_tuple("Trumpets",          "High-Trumpet-F",           "HgTpF"));
    n.push_back(make_tuple("Trumpets",          "High-Trumpet-Eb",          "HgTpEb"));
    n.push_back(make_tuple("Trumpets",          "High-Trumpet-D",           "HgTpD"));
    n.push_back(make_tuple("Trumpets",          "Cornet-Eb",                "CntEb"));
    n.push_back(make_tuple("Trumpets",          "Trumpet-C",                "TpC"));
    n.push_back(make_tuple("Trumpets",          "Trumpet-Bb",               "TpBb"));
    n.push_back(make_tuple("Trumpets",          "Cornet-Bb",                "CntBb"));
    n.push_back(make_tuple("Trumpets",          "Alto-Trumpet-F",           "AlTpF"));
    n.push_back(make_tuple("Trumpets",          "Bass-Trumpet-Eb",          "BsTpEb"));
    n.push_back(make_tuple("Trumpets",          "Bass-Trumpet-C",           "BsTpC"));
    n.push_back(make_tuple("Trumpets",          "Bass-Trumpet-Bb",          "BsTpBb"));

    n.push_back(make_tuple("Trombones",         "Alto-Trombone",            "AlTbn"));
    n.push_back(make_tuple("Trombones",         "Tenor-Trombone",           "TTbn"));
    n.push_back(make_tuple("Trombones",         "Bass-Trombone",            "BTbn"));
    n.push_back(make_tuple("Trombones",         "Contrabass-Trombone",      "CbTbn"));

    n.push_back(make_tuple("Tubas",             "Euphonium",                "Eup"));
    n.push_back(make_tuple("Tubas",             "Bass-Tuba",                "BTb"));
    n.push_back(make_tuple("Tubas",             "Contrabass-Tuba",          "CbTb"));

    n.push_back(make_tuple("Saxhorns",          "Flugelhorn",               "Flg"));

    n.push_back(make_tuple("Keyboards",         "Piano",                    "Pn"));
    n.push_back(make_tuple("Keyboards",         "Celesta",                  "Cel"));
    n.push_back(make_tuple("Keyboards",         "Harpsichord",              "Hpscd"));
    n.push_back(make_tuple("Keyboards",         "Accordion",                "Acc"));

    n.push_back(make_tuple("Plucked-Strings",   "Harp",                     "Hp"));
    n.push_back(make_tuple("Plucked-Strings",   "Guitar",                   "Gtr"));
    n.push_back(make_tuple("Plucked-Strings",   "Mandolin",                 "Mdl"));

    n.push_back(make_tuple("Strings",           "Violin",                   "Vn"));
    n.push_back(make_tuple("Strings",           "ViolinEnsemble",           "Vns"));
    n.push_back(make_tuple("Strings",           "Viola",                    "Va"));
    n.push_back(make_tuple("Strings",           "ViolaEnsemble",            "Vas"));
    n.push_back(make_tuple("Strings",           "Violoncello",              "Vc"));
    n.push_back(make_tuple("Strings",           "VioloncelloEnsemble",      "Vcs"));
    n.push_back(make_tuple("Strings",           "Contrabass",               "Cb"));
    n.push_back(make_tuple("Strings",           "ContrabassEnsemble",       "Cbs"));
    
    return n;
}



