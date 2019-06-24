//
//  target.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 17/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "target.h"

Target::Target(Session* s)
{
    isComputed  = false;
    sSession    = s;
    sSession->setMultiTarget(false);
}

bool Target::getComputed()
{
    return isComputed;
}

void Target::dumpTargetFeatures(string filename, matrixPtr tSp, matrixPtr tMSp)
{
    matrixPtr curDesc;
    FILE *curFile;
    int i, j, t;
    
    curFile = fopen((filename + ".HarmonicFilters.txt").c_str(), "w+");
    vector<vector<string> > hFilters = getHarmonicFilters();
    // For the first time instant, push up the non-empty partials
    for (i = 0; i < hFilters.size(); i++)
    {
        for (j = 0; j < hFilters[i].size(); j++)
            fprintf(curFile, "%s ", hFilters[i][j].c_str());
        fprintf(curFile, "\n");
    }
    fclose(curFile);
    curFile = fopen((filename + ".MelAmplitude.txt").c_str(), "w+");
    curDesc = features.melbands.melAmplitude;
    for (i = 0; i < curDesc->rows(); i++)
    {
        for (j = 0; j < curDesc->cols(); j++)
            fprintf(curFile, "%f ", curDesc->operator()(i, j));
        fprintf(curFile, "\n");
    }
    fclose(curFile);
    curFile = fopen((filename + ".MelMeanAmplitude.txt").c_str(), "w+");
    curDesc = features.melbands.melMeanAmp;
    for (i = 0; i < curDesc->rows(); i++)
    {
        for (j = 0; j < curDesc->cols(); j++)
            fprintf(curFile, "%f ", curDesc->operator()(i, j));
        fprintf(curFile, "\n");
    }
    fclose(curFile);
    curFile = fopen((filename + ".PartialsAmplitude.txt").c_str(), "w+");
    curDesc = tSp;
    for (i = 0; curDesc && i < curDesc->rows(); i++)
    {
        for (j = 0; j < curDesc->cols(); j++)
        fprintf(curFile, "%f ", curDesc->operator()(i, j));
        fprintf(curFile, "\n");
    }
    fclose(curFile);
    curFile = fopen((filename + ".PartialsMeanAmplitude.txt").c_str(), "w+");
    curDesc = tMSp;
    for (i = 0; curDesc && i < curDesc->rows(); i++)
    {
        for (j = 0; j < curDesc->cols(); j++)
            fprintf(curFile, "%f ", curDesc->operator()(i, j));
        fprintf(curFile, "\n");
    }
    fclose(curFile);
    for (t = 0; t < features.descriptors.size(); t++)
    {
        curFile = fopen((filename + "." + features.descriptors[t].name + ".txt").c_str(), "w+");
        curDesc = features.descriptors[t].resampled_val;
        for (i = 0; i < curDesc->rows(); i++)
        {
            for (j = 0; j < curDesc->cols(); j++)
                fprintf(curFile, "%f ", curDesc->operator()(i, j));
            fprintf(curFile, "\n");
        }
        fclose(curFile);
        curFile = fopen((filename + "." + features.descriptors[t].name + "Mean.txt").c_str(), "w+");
        fprintf(curFile, "%f\n",features.descriptors[i].mean);
        fclose(curFile);
        curFile = fopen((filename + "." + features.descriptors[t].name + "StdDev.txt").c_str(), "w+");
        fprintf(curFile, "%f\n",features.descriptors[i].stdDev);
        fclose(curFile);
    }
}

matrixPtr Target::getFeature(string fName)
{
    if (features.descriptors.size() == 0 || !isComputed)
        throw "Target::getFeature, Target features are not computed";
    
    int i;
    // LoudnessFactor Case
    if (fName == "LoudnessFactor")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = features.melbands.loudnessFactor;
        return t;
    }
    
    if (fName == "MelAmplitude")
        return features.melbands.melAmplitude;
    
    if (fName == "MelMeanAmplitude")
        return features.melbands.melMeanAmp;
    
    if (fName == "MelEnergy")
        return features.melbands.melEnergy;
    
    if (fName == "MelMeanEnergy")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = features.melbands.melMeanEnergy;
        return t;
    }
    
    // Partials Case
    if (fName == "PartialsMeanEnergy")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = features.partials.partialsMeanEnergy;
        return t;
    }
    if (fName == "PartialsEnergy")
    {
        matrixPtr t;
        t->setRows(1);
        t->setCols(features.partials.partialsEnergySize);
        t->setData(features.partials.partialsEnergy, features.partials.partialsEnergySize);
        t->setOwnsData(true);
        return t;
    }
    if (fName == "PartialsAmplitude")
    {
        return features.partials.partialsAmplitude;
    }
    if (fName == "PartialsFrequency")
    {
        return features.partials.partialsFrequency;
    }
    if (fName == "PartialsMeanAmplitude")
    {
        matrixPtr t(new matrix(features.partials.partialsMeanAmplitudeSize, 1));
        for (i = 0; i < features.partials.partialsMeanAmplitudeSize; i++)
            t->operator()(i) = features.partials.partialsMeanAmplitude[i];
        return t;
    }
    if (fName == "PartialsMeanFrequency")
    {
        matrixPtr t(new matrix(features.partials.partialsMeanFrequencySize, 1));
        for (i = 0; i < features.partials.partialsMeanFrequencySize; i++)
            t->operator()(i) = features.partials.partialsMeanFrequency[i];
        return t;
    }
    
    // Descriptor Case
    bool mean = false;
    size_t pos = fName.find("Mean");
    if (pos != string::npos)
    {
        mean = true;
        fName.erase(pos);
    }
    
    bool stdDev = false;
    pos = fName.find("StdDev");
    if (pos != string::npos)
    {
        stdDev = true;
        fName.erase(pos);
    }
    
    for (i = 0; i < features.descriptors.size(); i++)
        if (features.descriptors[i].name == fName)
        {
            if (mean)
            {
                matrixPtr t(new matrix(1, 1));
                t->operator()(0, 0) = features.descriptors[i].mean;
                return t;
            }
            else if (stdDev)
            {
                matrixPtr t(new matrix(1, 1));
                t->operator()(0, 0) = features.descriptors[i].stdDev;
                return t;
            }
            else
                return features.descriptors[i].resampled_val;
        }

    return matrixPtr(new matrix());
}

matrixPtr Target::getFeature(sqlStruct& targetFeatures, string fName)
{
    if (targetFeatures.descriptors.size() == 0 || !isComputed)
        throw "Target::getFeature, Target features are not computed";
    
    int i;
    // LoudnessFactor Case
    if (fName == "LoudnessFactor")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = targetFeatures.melbands.loudnessFactor;
        return t;
    }
    
    if (fName == "MelAmplitude")
        return targetFeatures.melbands.melAmplitude;
    
    if (fName == "MelMeanAmplitude")
        return targetFeatures.melbands.melMeanAmp;
    
    if (fName == "MelEnergy")
        return targetFeatures.melbands.melEnergy;
    
    if (fName == "MelMeanEnergy")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = targetFeatures.melbands.melMeanEnergy;
        return t;
    }
    
    // Partials Case
    if (fName == "PartialsMeanEnergy")
    {
        matrixPtr t(new matrix(1, 1));
        t->operator()(0, 0) = targetFeatures.partials.partialsMeanEnergy;
        return t;
    }
    if (fName == "PartialsEnergy")
    {
        matrixPtr t;
        t->setRows(1);
        t->setCols(targetFeatures.partials.partialsEnergySize);
        t->setData(targetFeatures.partials.partialsEnergy, targetFeatures.partials.partialsEnergySize);
        t->setOwnsData(true);
        return t;
    }
    if (fName == "PartialsAmplitude")
    {
        return targetFeatures.partials.partialsAmplitude;
    }
    if (fName == "PartialsFrequency")
    {
        return targetFeatures.partials.partialsFrequency;
    }
    if (fName == "PartialsMeanAmplitude")
    {
        matrixPtr t(new matrix(targetFeatures.partials.partialsMeanAmplitudeSize, 1));
        for (i = 0; i < targetFeatures.partials.partialsMeanAmplitudeSize; i++)
            t->operator()(i) = targetFeatures.partials.partialsMeanAmplitude[i];
        return t;
    }
    if (fName == "PartialsMeanFrequency")
    {
        matrixPtr t(new matrix(targetFeatures.partials.partialsMeanFrequencySize, 1));
        for (i = 0; i < targetFeatures.partials.partialsMeanFrequencySize; i++)
            t->operator()(i) = targetFeatures.partials.partialsMeanFrequency[i];
        return t;
    }
    
    // Descriptor Case
    bool mean = false;
    size_t pos = fName.find("Mean");
    if (pos != string::npos)
    {
        mean = true;
        fName.erase(pos);
    }
    
    bool stdDev = false;
    pos = fName.find("StdDev");
    if (pos != string::npos)
    {
        stdDev = true;
        fName.erase(pos);
    }
    
    for (i = 0; i < targetFeatures.descriptors.size(); i++)
        if (targetFeatures.descriptors[i].name == fName)
        {
            if (mean)
            {
                matrixPtr t(new matrix(1, 1));
                t->operator()(0, 0) = targetFeatures.descriptors[i].mean;
                return t;
            }
            else if (stdDev)
            {
                matrixPtr t(new matrix(1, 1));
                t->operator()(0, 0) = targetFeatures.descriptors[i].stdDev;
                return t;
            }
            else
                return targetFeatures.descriptors[i].resampled_val;
        }
    
    return matrixPtr(new matrix());
}

vector<string> Target::getFeaturesNames()
{
    if (features.descriptors.size() == 0 || !isComputed)
        throw "Target::getFeaturesNames, Target features are not computed";
    
    int i;
    vector<string> names;
    for (i = 0; i < features.descriptors.size(); i++)
        names.push_back(features.descriptors[i].name);
    
    return names;
}


sqlStruct Target::getFeatures()
{
    return features;
}


void Target::modifyFeature(string fName, matrixPtr fValue)
{
    int i;
    bool found = false;
    for (i = 0; i < features.descriptors.size(); i++)
    {
        if (features.descriptors[i].name == fName)
        {
            found = true;
            features.descriptors[i].mean            = getMean(fValue->c_data(), fValue->size());
            features.descriptors[i].stdDev          = std_dev(fValue->c_data(), fValue->size(), features.descriptors[i].mean);
            features.descriptors[i].max             = getMax(fValue->c_data(), fValue->size());
            features.descriptors[i].min             = getMin(fValue->c_data(), fValue->size());
            // Normalisation
            fValue->operator-(features.descriptors[i].min);
            fValue->operator/(features.descriptors[i].max - features.descriptors[i].min);
            features.descriptors[i].resampled_val   = fValue;
            break;
        }
    }
    
    if (!found)
    {
        string err = "Target::modifyFeature::Unknown - " + fName + " is not a target feature";
        throw err;
    }
}

sqlStruct Target::getFeaturesList()
{
    return features;
}

void Target::setAnalysisParameters(string configFile)
{
    getConfigAnalysisParams(analysisParameters, configFile);
    int err = checkConfigParams(analysisParameters);
    if (err != -1)
    {
        string error = "Target::setAnalysisParameters::Value error in configuration file. Parameter " + analysisParameters.irc_desc_params[err].name + " is out of range";
        throw error;
    }
}

analysis_params Target::getTargetParameters()
{
    return analysisParameters;
}

void Target::emptyHarmonicFilters()
{
    harmonicFilters.clear();
}

vector<vector<string> > Target::getHarmonicFilters()
{
    int i, j;
    if (harmonicFilters.empty())
    {
        matrixPtr  partialsFrequency    = features.partials.partialsFrequency;
        matrixPtr  partialsAmplitude    = features.partials.partialsAmplitude;
        int        resolution           = sSession->getProduction()->getResolution();
        for (i = 0; i < partialsFrequency->cols(); i++)
        {
            vector<float>   mtmidi;
            vector<string>  finalNoteValues;
            //printf("***\n %d \n ***\n", i);
            for (j = 0; j < partialsFrequency->rows(); j++)
            {
                //printf("%f (%f)\n", partialsFrequency->operator()(j, i), partialsAmplitude->operator()(j, i));
                if (partialsAmplitude->operator()(j, i) > 0.005)
                {
                    //printf("SELECT : %f (%f) - ", partialsFrequency->operator()(j, i), partialsAmplitude->operator()(j, i));
                    if (resolution > 1)
                    {
                        //printf(" Pushed midicent (MICRO) : %f\n", round(hz2midi(partialsFrequency->operator()(j, i))));
                        mtmidi.push_back(round(hz2midi(partialsFrequency->operator()(j, i))));
                    }
                    //printf("%f\n", round(hz2midi(partialsFrequency->operator()(j, i)) * resolution) / resolution);
                    mtmidi.push_back(round(hz2midi(partialsFrequency->operator()(j, i)) * resolution) / resolution);
                }
            }
            finalNoteValues = midi2mtnotes(mtmidi);
            harmonicFilters.push_back(finalNoteValues);
            harmonicFilters.push_back(finalNoteValues);
        }
        /*
        printf("****\n****\n FINAL FILTERS ****\n****\n");
        for (i = 0; i < harmonicFilters.size(); i++)
        {
            printf("**** %d ****\n", i);
            vector<string> curFilters = harmonicFilters[i];
            for (j = 0; j < curFilters.size(); j++)
                printf("%s ", curFilters[j].c_str());
            printf("\n");
        }
        printf("****\n****\n");
         */
    }
    return harmonicFilters;
}

vector<string> Target::getUniqueHarmonicFilters()
{
    if (harmonicFilters.empty())
        harmonicFilters = getHarmonicFilters();
    vector<string> uniqueHM;
    vector< vector<string> >::iterator it;
    for (vector< vector<string> >::iterator it = harmonicFilters.begin(); it != harmonicFilters.end(); ++it)
        uniqueHM.insert(uniqueHM.end(), it->begin(), it->end());
    vecUnique(uniqueHM);
    return uniqueHM;
}

void Target::freezeFeature(bool freeze, string descriptor)
{
    int i;
    for (i = 0; i < features.descriptors.size(); i++)
        if (features.descriptors[i].name == descriptor)
        {
            features.descriptors[i].isFreezed = freeze;
            break;
        }
}

int Target::getFreeze(string descriptor)
{
    int i;
    for (i = 0; i < features.descriptors.size(); i++)
        if (features.descriptors[i].name == descriptor)
            return features.descriptors[i].isFreezed;
    
    return -1;
}

void Target::setPartialsAmplitude(matrixPtr parAmp)
{
    features.partials.partialsAmplitude = parAmp;
}

void Target::setPartialsFrequency(matrixPtr parFreq)
{
    features.partials.partialsFrequency = parFreq;
    harmonicFilters.clear();
}

void Target::setDuration(float dur)
{
    features.duration = dur;
}

bool Target::copyFeatures(sqlStruct& oldFeatures)
{
    int i;
    features.descriptors = vector<feature<float> >(oldFeatures.descriptors);
    for (i = 0; i < oldFeatures.descriptors.size(); i++)
        features.descriptors[i] = feature<float>(oldFeatures.descriptors[i]);
    features.partials = partials(oldFeatures.partials);
    features.partials.partialsMeanAmplitude = (float *)calloc(oldFeatures.partials.partialsMeanAmplitudeSize, sizeof(float));
    features.partials.partialsMeanFrequency = (float *)calloc(oldFeatures.partials.partialsMeanFrequencySize, sizeof(float));
    features.partials.partialsEnergy = (float *)calloc(oldFeatures.partials.partialsEnergySize, sizeof(float));
    for (i = 0; i < oldFeatures.partials.partialsMeanAmplitudeSize; i++)
    {
        features.partials.partialsMeanAmplitude[i] = oldFeatures.partials.partialsMeanAmplitude[i];
        features.partials.partialsMeanFrequency[i] = oldFeatures.partials.partialsMeanFrequency[i];
        features.partials.partialsEnergy[i] = oldFeatures.partials.partialsEnergy[i];
    }
    features.duration = oldFeatures.duration;
    features.melbands = oldFeatures.melbands;
    isComputed = true;
    return true;
}



TargetSound::TargetSound(Session* sessObj, string filename)
: Target(sessObj)
{
    analysisParameters = defaultAnalysisParams();
    if (boost::filesystem::exists(boost::filesystem::path("/tmp/orchidsAnalysis.cfg")))
        setAnalysisParameters("/tmp/orchidsAnalysis.cfg");
    loadSoundFile(filename);
    sSession->setMultiTarget(false);
}

string TargetSound::getSoundName()
{
    return soundFile;
}

bool TargetSound::computeFeatures()
{
    if (isComputed)
        return true;
    // Launch analysis
    try {
        analyzeTarget(soundFile, analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    
    float *signal;
    importSignal(soundFile, sRate, signal, signalSize);
    free(signal);
    
    isComputed = true;
    return true;
}

bool TargetSound::computeFeatures(sqlStruct& oldFeatures)
{
    if (isComputed)
        return true;
    // Launch analysis
    try {
        analyzeTarget(soundFile, analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    
    float *signal;
    importSignal(soundFile, sRate, signal, signalSize);
    free(signal);
    
    // Keep the freezed descriptors
    int i;
    for (i = 0; i < oldFeatures.descriptors.size(); i++)
        if (oldFeatures.descriptors[i].isFreezed)
            features.descriptors[i] = oldFeatures.descriptors[i];
    
    isComputed = true;
    return true;
}

void TargetSound::loadSoundFile(string filename)
{
    soundFile   = filename;
    isComputed  = false;
}

void TargetSound::addFeature(string fName, matrixPtr fValue)
{
    
}

vector<int> TargetSound::computeEvents()
{
    int i, d;
    feature<float> loudness;
    for (i = 0; i < features.descriptors.size(); i++)
    {
        if (features.descriptors[i].name == "Loudness")
        {
            loudness = features.descriptors[i];
            break;
        }
    }
    
    int     resampleSize = loudness.values->size() / 2;
    float*  resampValues = new float[resampleSize];
    resampleData(loudness.values->c_data(), loudness.values->size(), resampValues, resampleSize, SRC_LINEAR);
    // Compute the approximate derivative
    vector<float> resampVal(resampValues, resampValues + resampleSize);
    delete resampValues;
    vector<float> derivative = diff(resampVal);
    // Launch tau-thompson
    vector<int> dev1        = tauThompsonDistribution(derivative);
    float       lastTime    = 0;
    vector<int> eventsTime;
    for (d = 0; d < dev1.size(); d++)
    {
        float d2 = (float)d * (float)signalSize / (float)dev1.size();
        if (dev1[d] == 1)
        {
            if ((((d2 / sRate) * 1000) - lastTime > 200) && ((((float)signalSize / sRate) * 1000) - ((d2 / sRate) * 1000) > 200))
            {
                lastTime = (d2 / sRate) * 1000;
                eventsTime.push_back(d2);
            }
        }
    }
    return eventsTime;
}

void TargetSound::setLoopRegion(int begin, int end)
{
    int         size    = end - begin;
    string      outpath = "/tmp/tmpLoopRegionTarget.wav";
    float*      instruSignal;
    float       sr_hz;
    int         signal_size;
    try {
        importSignal(soundFile, sr_hz, instruSignal, signal_size);
        wavWrite(instruSignal + begin, size, sr_hz, 32, outpath.c_str());
        analyzeTarget(outpath, analysisParameters, features);
        sRate = sr_hz;
//        signalSize = size;
        emptyHarmonicFilters();
    } catch (const char *e) {
        throw e;
    }
}


TargetAbstract::TargetAbstract(Session* sessObj): Target(sessObj)
{
    analysisParameters = defaultAnalysisParams();
    if (boost::filesystem::exists(boost::filesystem::path("/tmp/orchidsAnalysis.cfg")))
        setAnalysisParameters("/tmp/orchidsAnalysis.cfg");
    isComputed = false;
    sSession->setMultiTarget(false);
}

string TargetAbstract::getSoundName()
{
    return "emptyTarget.wav";
}

bool TargetAbstract::computeFeatures()
{
    if (isComputed)
        return true;
    // Launch analysis
    try {
        analyzeTarget("emptyTarget.wav", analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    isComputed = true;
    return true;
}

bool TargetAbstract::computeFeatures(sqlStruct& oldFeatures)
{
    if (isComputed)
    return true;
    // Launch analysis on empty target
    try {
        analyzeTarget("emptyTarget.wav", analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    
    // Keep the freezed descriptors
    int i;
    for (i = 0; i < oldFeatures.descriptors.size(); i++)
        if (oldFeatures.descriptors[i].isFreezed)
            features.descriptors[i] = oldFeatures.descriptors[i];
    features.targetMeanSpec = oldFeatures.targetMeanSpec;
    features.targetSpec = oldFeatures.targetSpec;
    features.partials = oldFeatures.partials;
    features.duration = oldFeatures.duration;
    features.melbands = oldFeatures.melbands;
    isComputed = true;
    return true;
}

void TargetAbstract::addFeature(string fName, matrixPtr fValue)
{
    feature<float> f(fName, "");
    f.resampled_val = fValue;
    f.min           = getMin(f.resampled_val->c_data(), f.resampled_val->size());
    f.max           = getMin(f.resampled_val->c_data(), f.resampled_val->size());
    f.mean          = getMean(f.resampled_val->c_data(), f.resampled_val->size());
    f.stdDev        = std_dev(fValue->c_data(), fValue->size(), f.mean);
    features.descriptors.push_back(f);
}

vector<int> TargetAbstract::computeEvents()
{
    return vector<int>();
}

void TargetAbstract::setLoopRegion(int begin, int end)
{
    int         size    = end - begin;
    string      soundFile = "emptyTarget.wav";
    string      outpath = "/tmp/tmpLoopRegionTarget.wav";
    float*      instruSignal;
    float       sr_hz;
    int         signal_size;
    try {
        importSignal(soundFile, sr_hz, instruSignal, signal_size);
        wavWrite(instruSignal + begin, size, sr_hz, 32, outpath.c_str());
        analyzeTarget(outpath, analysisParameters, features);
        emptyHarmonicFilters();
    } catch (const char *e) {
        throw e;
    }
}


MultiTarget::MultiTarget(Session* sessObj, string filename): Target(sessObj)
{
    analysisParameters = defaultAnalysisParams();
    if (boost::filesystem::exists(boost::filesystem::path("/tmp/orchidsAnalysis.cfg")))
        setAnalysisParameters("/tmp/orchidsAnalysis.cfg");
    loadSoundFile(filename);
    sSession->setMultiTarget(true);
}

string MultiTarget::getSoundName()
{
    return soundFile;
}

bool MultiTarget::computeFeatures()
{
    if (isComputed)
        return true;
    // Launch analysis
    try {
        analyzeTarget(soundFile, analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    
    float *signal;
    importSignal(soundFile, sRate, signal, signalSize);
    free(signal);
    
    setSegments(computeEvents());
    
    isComputed = true;
    return true;
}

bool MultiTarget::computeFeatures(sqlStruct& oldFeatures)
{
    if (isComputed)
        return true;

    try {
        analyzeTarget(soundFile, analysisParameters, features);
    } catch (const char *e) {
        throw e;
    }
    
    float *signal;
    importSignal(soundFile, sRate, signal, signalSize);
    free(signal);
        
    // Keep the freezed descriptors
    int i;
    for (i = 0; i < oldFeatures.descriptors.size(); i++)
        if (oldFeatures.descriptors[i].isFreezed)
            features.descriptors[i] = oldFeatures.descriptors[i];
    
    setSegments(computeEvents());
    
    isComputed = true;
    return true;
}

void MultiTarget::addFeature(string fName, matrixPtr fValue)
{

}

void MultiTarget::loadSoundFile(string filename)
{
    soundFile   = filename;
    isComputed  = false;
}

void MultiTarget::setSegments(vector<int> segments)
{
    int i;
    segmentsMarker.clear();
    // Fill segments marker structure
    if (segments.empty())
        segmentsMarker.push_back(pair<int, int>(0, signalSize));
    else
    {
        segmentsMarker.push_back(pair<int, int>(0, segments[0]));
        for (i = 0; i < segments.size() - 1; i++)
            segmentsMarker.push_back(pair<int, int>(segments[i], segments[i + 1]));
        segmentsMarker.push_back(pair<int, int>(segments[segments.size() - 1], signalSize));
    }
}

vector<int> MultiTarget::getSegments()
{
    int i;
    vector<int> eventsTime;
    for (i = 0; i < segmentsMarker.size() - 1; i++)
        eventsTime.push_back(segmentsMarker[i].second);
    return eventsTime;
}

sqlStruct MultiTarget::segmentSoundFile(int k)
{
    if (segmentsMarker.size() == 1)
        return features;
    else
    {
        // Compute begin and end markers
        int         begin   = segmentsMarker[k].first;
        int         end     = segmentsMarker[k].second;
        int         size    = end - begin;
        sqlStruct   s;
        string      outpath = "/tmp/tmpSegmentedTarget.wav";
        float*      instruSignal;
        float       sr_hz;
        int         signal_size;
        try {
            importSignal(soundFile, sr_hz, instruSignal, signal_size);
            wavWrite(instruSignal + begin, size, sr_hz, 32, outpath.c_str());
            free(instruSignal);
            analyzeTarget(outpath, analysisParameters, s);
        } catch (const char *e) {
            throw e;
        }
        return s;
    }
}

int MultiTarget::getSignalSize()
{
    return signalSize;
}

vector<int> MultiTarget::computeEvents()
{
    int i, d;
    matrixPtr loudness;
    for (i = 0; i < features.descriptors.size(); i++)
    {
        if (features.descriptors[i].name == "Loudness")
        {
            loudness = features.descriptors[i].values;
            break;
        }
    }
    
    //int     resampleSize = loudness.values.size() / 2;
    //float*  resampValues = new float[resampleSize];
    //resampleData(loudness.values.c_data(), loudness.values.size(), resampValues, resampleSize, SRC_LINEAR);
    // Compute the approximate derivative
    vector<float> resampVal(loudness->begin(), loudness->end());
    //delete resampValues;
    vector<float> derivative = diff(resampVal);
    for (i = 0; i < derivative.size(); i++)
        derivative[i] = abs(derivative[i]);
    // Launch tau-thompson
    vector<int> dev1        = tauThompsonDistribution(derivative);
    float       lastTime    = 0;
    vector<int> eventsTime;
    for (d = 0; d < dev1.size(); d++)
    {
        float d2 = (float)d * (float)signalSize / (float)dev1.size();
        if (dev1[d] == 1)
        {
            if ((((d2 / sRate) * 1000) - lastTime > 200) && ((((float)signalSize / sRate) * 1000) - ((d2 / sRate) * 1000) > 200))
            {
                lastTime = (d2 / sRate) * 1000;
                eventsTime.push_back(d2);
            }
        }
    }
    return eventsTime;
}

PopulationPtr MultiTarget::setContinuity(vector<PopulationPtr> solutions)
{
    int i, k, c, critPos = -1;
    vector<FeaturesPtr> optFeatures = sSession->getFeatures();
    // Keep most optimize solution for partials amplitude
    PopulationPtr       finalPop = PopulationPtr(new Population(sSession));
    TArray2<int>        solutionIDs((int)solutions.size(), (int)sSession->getProduction()->getDefaultOrchestra().size());
    matrixPtr           crit(new matrix((int)solutions.size(), (int)optFeatures.size()));
    for (i = 0; i < optFeatures.size(); i++)
        if (optFeatures[i]->getFeatureName() == "PartialsAmplitude")
            critPos = i;
    if (critPos == -1)
        critPos = floor(rand() * optFeatures.size());
    
    for (k = 0; k < solutions.size(); k++)
    {
        matrixPtr   criteria = solutions[k]->getCriteria();
        // Get PartialsAmplitude
        float*      col = criteria->getCol(critPos);
        float       best;
        int         bestPos;
        getMin(col, criteria->rows(), best, bestPos);
        free(col);
        finalPop->setSolution(SolutionPtr(new Solution(*solutions[k]->getSolution(bestPos))));
        vector<int> individualsIDs = solutions[k]->getSolution(bestPos)->getIndividualsID();
        for (c = 0; c < individualsIDs.size(); c++)
            solutionIDs(k, c) = individualsIDs[c];
        vector<float> critRow = solutions[k]->getCriteria()->getRowVec(bestPos);
        for (c = 0; c < critRow.size(); c++)
            crit->operator()(k, c) = critRow[c];
    }
    finalPop->setSolutionsIDs(matrixIPtr(new matrixI(solutionIDs)));
    finalPop->setCriteria(crit);
    
    return finalPop;
}

void MultiTarget::setLoopRegion(int begin, int end)
{
    int         size    = end - begin;
    if (size < 100)
        throw "TargetSelection::Error - Selection size is too small";
    string      outpath = "/tmp/tmpLoopRegionTarget.wav";
    float*      instruSignal;
    float       sr_hz;
    int         signal_size;
    try {
        importSignal(soundFile, sr_hz, instruSignal, signal_size);
        wavWrite(instruSignal + begin, size, sr_hz, 32, outpath.c_str());
        analyzeTarget(outpath, analysisParameters, features);
        sRate = sr_hz;
        signalSize = size;
        emptyHarmonicFilters();
    } catch (const char *e) {
        throw e;
    }
}

float MultiTarget::getSegmentDuration(int k)
{
    return features.duration * (segmentsMarker[k].second - segmentsMarker[k].first) / signalSize;
}

