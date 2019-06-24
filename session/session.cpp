//
//  session.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "session.h"

void Session::setOSCStart()
{
    // Initialize OSC server
    sOSCHandle = OSCListenerPtr(new OSCListener(this));
    sOSCHandle->startThread();
}

void Session::setOSCJoin()
{
    sOSCHandle->join();
}

void Session::constructDefaultSession()
{
    // Connect to database and initialize knowledge
    sKnowledge = KnowledgeBDBPtr(new KnowledgeBDB(this, BDB_DIR, "OrchidsDb.db"));
    // Synth
    //sKnowledge = KnowledgeBDBPtr(new KnowledgeBDB(this, "SynthDB_v0.1", "OrchidsDb.db"));
    sKnowledge->buildKnowledge();
    
    // Initialize production in orchestra mode
    sProduction = ProductionPtr(new ProductionOrchestra(this, vector<string>(), 1));
    sProduction->setInstruments(sProduction->getDefaultOrchestra());
    sProduction->constructBaseFilters();
    
    // Set critaria list to default
    setCriteriaList(getDefaultCriteria());
    
    // Initialize target to abstract mode
    sTarget = TargetPtr(new TargetAbstract(this));
    sTarget->computeFeatures();
    
    // Initialize search to genetic algorithm
    sSearch = SearchPtr(new SearchGenetic(this));
    //sSearch = SearchPtr(new SearchOptimalWarping(this));
    sSearch->setDefaultParameters();
    
    // Initialize an empty solution Population
    sSolution = PopulationPtr(new Population(this));
    
    sendOSCMessage("/Orchids/session", "ready");
}

void Session::exitSession()
{
    sOSCHandle->Stop();
}

void Session::setTarget(TargetPtr t)
{
    sTarget = t;
}

TargetPtr Session::getTarget()
{
    return sTarget;
}

bool Session::computeTargetFeatures()
{
    if (sTarget->getComputed())
        return true;
    try {
        sTarget->computeFeatures();
    } catch (const char* e) {
        throw e;
    }
    return true;
}

bool Session::computeTargetFeatures(sqlStruct& oldFeatures)
{
    if (sTarget->getComputed())
        return true;
    try {
        sTarget->computeFeatures(oldFeatures);
    } catch (const char* e) {
        throw e;
    }
    return true;
}

void Session::setSearch(SearchPtr s)
{
    sSearch = s;
}

SearchPtr Session::getSearch()
{
    return sSearch;
}

PopulationPtr Session::getSolution()
{
    return sSolution;
}

OSCListenerPtr Session::getHandle()
{
    return sOSCHandle;
}

void Session::setKnowledge(KnowledgeBDBPtr k)
{
    sKnowledge = k;
}

KnowledgeBDBPtr Session::getKnowledge()
{
    return sKnowledge;
}

void Session::setProduction(ProductionPtr p)
{
    sProduction = p;
}

void Session::setOrchestra(vector<string> orch)
{
    sProduction->setInstruments(orch);
}

ProductionPtr Session::getProduction()
{
    return sProduction;
}

void Session::setCriteriaList(vector<string> critList)
{
    int i;
    vector<string>          allowedFeatures     = sKnowledge->getCriteriaList();
    knowledgeDescription    a                   = sKnowledge->getFieldsList();
    sFeatures.clear();
    
    for (i = 0; i < critList.size(); i++)
    {
        string curCriteria = critList[i];
        
        if (std::find(allowedFeatures.begin(), allowedFeatures.end(), curCriteria) == allowedFeatures.end())
        {
            string err = "Session::setCriteriaList, Unrecognized criteria : " + curCriteria;
            throw err;
        }
        else
        {
            FeaturesPtr newFeature;
            if (curCriteria == "Loudness")
                newFeature = FeaturesPtr(new FeaturesLoudness(curCriteria));
            else if (curCriteria == "PartialsAmplitude")
                newFeature = FeaturesPtr(new FeaturesPartialsAmplitude(curCriteria));
            else if (curCriteria == "PartialsMeanAmplitude")
                newFeature = FeaturesPtr(new FeaturesPartialsMeanAmplitude(curCriteria));
            //else if (curCriteria == "PartialsEnergy")
                //newFeature = FeaturesPtr(new FeaturesGenericTemporalEnergy(curCriteria));
            else if (curCriteria == "SpectralCentroid")
                newFeature = FeaturesPtr(new FeaturesSpectralCentroid(curCriteria));
            else if (curCriteria == "SpectralCentroidMean")
                newFeature = FeaturesPtr(new FeaturesSpectralCentroidMean(curCriteria));
            else if (curCriteria == "SpectralSpread")
                newFeature = FeaturesPtr(new FeaturesSpectralSpread(curCriteria));
            else if (curCriteria == "SpectralSpreadMean")
                newFeature = FeaturesPtr(new FeaturesSpectralSpreadMean(curCriteria));
            else if (curCriteria == "MelAmplitude")
                newFeature = FeaturesPtr(new FeaturesMelAmplitude(curCriteria));
            else if (curCriteria == "MelMeanAmplitude")
                newFeature = FeaturesPtr(new FeaturesMelMeanAmplitude(curCriteria));
            else
            {
                vector<string>::iterator it = std::find(a.fields.begin(), a.fields.end(), curCriteria);
                string type = a.types[it - a.fields.begin()];
                if (type == "descriptor")
                    if (curCriteria.find("Energy") != string::npos)
                        newFeature = FeaturesPtr(new FeaturesGenericTemporalEnergy(curCriteria));
                    else
                        newFeature = FeaturesPtr(new FeaturesGenericTemporal(curCriteria));
                else
                    newFeature = FeaturesPtr(new FeaturesGenericStatic(curCriteria));
            }
            newFeature->setSession(this);
            sFeatures.push_back(newFeature);
        }
    }
    sProduction->emptyVariableDomains();
    sFeatList = critList;
    setTotalFeaturesList();
}

vector<string> Session::getCriteriaList()
{
    return sFeatList;
}

vector<string> Session::getDefaultCriteria()
{
    vector<string> critLists;
    critLists.push_back("SpectralCentroidMean");
    critLists.push_back("SpectralSpreadMean");
    critLists.push_back("PartialsMeanAmplitude");
    critLists.push_back("MelMeanAmplitude");
    return critLists;
}

void Session::setTotalFeaturesList()
{
    int i;
    tFeatures.clear();
    // Get every feature dependancy
    for (i = 0; i < sFeatures.size(); i++)
    {
        vector<string> depandancies = sFeatures[i]->depandancy();
        tFeatures.insert(tFeatures.begin(), depandancies.begin(), depandancies.end());
    }
    vecUnique<string>(tFeatures);
    
    // Put PartialsFrequency before PartialsAmplitude for the convert/resampling process in Production::updateIndividualFeatures
    vector<string>::iterator freq = std::find(tFeatures.begin(), tFeatures.end(), "PartialsFrequency");
    vector<string>::iterator amp  = std::find(tFeatures.begin(), tFeatures.end(), "PartialsAmplitude");
    if (freq != tFeatures.end() && amp != tFeatures.end())
        std::swap(tFeatures[freq - tFeatures.begin()], tFeatures[amp - tFeatures.begin()]);
    
    // Put PartialsMeanFrequency before PartialsMeanAmplitude for the convert/resampling process in Production::updateIndividualFeatures
    vector<string>::iterator freqMean = std::find(tFeatures.begin(), tFeatures.end(), "PartialsMeanFrequency");
    vector<string>::iterator ampMean  = std::find(tFeatures.begin(), tFeatures.end(), "PartialsMeanAmplitude");
    if (freqMean != tFeatures.end() && ampMean != tFeatures.end())
        std::swap(tFeatures[freqMean - tFeatures.begin()], tFeatures[ampMean - tFeatures.begin()]);
    
    // Add the duration information
    tFeatures.push_back("LoudnessMean");
    tFeatures.push_back("LoudnessFactor");
    tFeatures.push_back("duration");
}

vector<FeaturesPtr > Session::getFeatures()
{
    return sFeatures;
}

vector<string> Session::getFeaturesList()
{
    return sFeatList;
}

vector<string> Session::getTotalFeaturesList()
{
    return tFeatures;
}

void Session::setMultiTarget(bool set)
{
    isMultiTarget = set;
}

bool Session::getIsMultiTarget()
{
    return isMultiTarget;
}

PopulationPtr Session::launchSearch()
{
    try {
        sSearch->initialize();
        if (getIsMultiTarget())
        {
            vector<PopulationPtr> solutions;
            for (int i = 0; i < boost::dynamic_pointer_cast<MultiTarget>(sTarget)->getSegments().size() + 1; i++)
            {
                cout << "Orchestrating segment " << i + 1 << endl;
                try {
                    // Export segment and analyze
                    sqlStruct s = boost::dynamic_pointer_cast<MultiTarget>(sTarget)->segmentSoundFile(i);
                    solutions.push_back(sSearch->launchSearch(s));
                } catch (const char* e) {
                    // Analyze segment went bad... push neutral population
                    PopulationPtr ntrPop(new Population(this));
                    ntrPop->createNeutralPop();
                    solutions.push_back(ntrPop);
                }
            }
            sSolution = boost::dynamic_pointer_cast<MultiTarget>(sTarget)->setContinuity(solutions);
        }
        else
        {
            sqlStruct s = sTarget->getFeaturesList();
            sSolution   = sSearch->launchSearch(s);
        }
    } catch (const char* e) {
        throw e;
    }
    return sSolution;
}

int Session::autoPilot()
{
    if (!this->getTarget()->getComputed())
        return 0;
    
    int i;
    // Open and read mean and stddev references for descriptors
    boost::filesystem::path path = getResourcesFolder<string>();
    path += "referenceDescriptors.txt";
    map<string, matrixPtr> descStructure;
    std::ifstream infile(path.string().c_str());
    for (i = 0; i < 20; i++)
    {
        string descName;
        infile >> descName;
        matrixPtr m(new matrix(1, 2));
        float minMean, maxMean, minStdDev, maxStdDev, meanMean, meanStdDev;
        infile >> minMean >> maxMean >> minStdDev >> maxStdDev >> meanMean >> meanStdDev;
        m->operator()(0, 0) = meanMean;
        m->operator()(0, 1) = (maxStdDev + 2 * meanStdDev) / 3;
        descStructure[descName] = m;
    }
    infile.close();
    
    // Auto-pilot list of descriptors with high variation
    vector<string>  autoDescriptors;
    sqlStruct       s = getTarget()->getFeaturesList();
    for (i = 0; i < s.descriptors.size(); i++)
    {
        string descName = s.descriptors[i].name;
        if (s.descriptors[i].stdDev > descStructure[descName]->operator()(0, 1))
            autoDescriptors.push_back(descName);
    }
    
    // Compute number of events
    vector<int> eventsTime = getTarget()->computeEvents();
    
    // Analyze results
    if (eventsTime.size() < 3 && autoDescriptors.size() < 2)
    {
        // Static Target
        // Set criterias
        autoDescriptors.clear();
        autoDescriptors.push_back("PartialsMeanAmplitude");
        autoDescriptors.push_back("SpectralCentroidMean");
        autoDescriptors.push_back("SpectralSpreadMean");
        setCriteriaList(autoDescriptors);
        // Set Search
        map<string, boost::any> params = this->getSearch()->getSearchParameters();
        setSearch(SearchPtr(new SearchGenetic(this)));
        getSearch()->setDefaultParameters();
        this->getSearch()->setSearchParameters(params);
        // Set Target
        string soundName = getTarget()->getSoundName();
        setTarget(TargetPtr(new TargetSound(this, soundName)));
        computeTargetFeatures();
        // Set Static mode
        getProduction()->setStaticMode(true);
        return 1;
    }
    else if (eventsTime.size() < 3 && autoDescriptors.size() >= 2)
    {
        // Temporal Target
        // Set criterias
        autoDescriptors.push_back("PartialsAmplitude");
        setCriteriaList(autoDescriptors);
        // Set Search
        map<string, boost::any> params = this->getSearch()->getSearchParameters();
        setSearch(SearchPtr(new SearchOptimalWarping(this)));
        getSearch()->setDefaultParameters();
        this->getSearch()->setSearchParameters(params);
        // Set Target
        string soundName = getTarget()->getSoundName();
        setTarget(TargetPtr(new TargetSound(this, soundName)));
        computeTargetFeatures();
        // Set Static mode
        getProduction()->setStaticMode(false);
        return 2;
    }
    else if (eventsTime.size() >= 3)
    {
        // Multi Target
        // Set criterias
        autoDescriptors.push_back("PartialsAmplitude");
        setCriteriaList(autoDescriptors);
        // Set Search
        map<string, boost::any> params = this->getSearch()->getSearchParameters();
        setSearch(SearchPtr(new SearchGenetic(this)));
        getSearch()->setDefaultParameters();
        this->getSearch()->setSearchParameters(params);
        // Set Static mode
        getProduction()->setStaticMode(false);
        // Set MultiTarget
        string soundName = getTarget()->getSoundName();
        setTarget(TargetPtr(new MultiTarget(this, soundName)));
        computeTargetFeatures();
        // Set MultiTarget markers
        boost::dynamic_pointer_cast<MultiTarget>(getTarget())->setSegments(eventsTime);
        return 3;
    }
    return 0;
}
