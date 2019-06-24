//
//  target.h
//  Orchids
//
//  Created by Antoine Bouchereau on 17/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__target__
#define __Orchids__target__

#include "Orchids.h"

class Session;

/**
 *  @class Target
 *
 *  @brief Abstract interface for target definition
 *
 *  This class is the superclass for any kind of target definition
 */
class Target
{
protected:
    Session*                sSession;               ///< Current user session
    sqlStruct               features;               ///< Target features
    analysis_params         analysisParameters;     ///< Parameters of spectral analysis
    vector<vector<string> >  harmonicFilters;        ///< Set of (optional) harmonic filters
    bool                    isComputed;             ///< Flag for computation of the target
    
public:
    /**
     *  @brief Constructor
     *  @param s Pointer to the current session
     */
    Target(Session* s);
    /**
     *  @brief Destructor
     */
    ~Target()
    {
        if (isComputed)
            features.deallocate();
    }
    
    /**
     *  @brief Return the soundfile name
     */
    virtual string          getSoundName()                                  = 0;
    /**
     *  @brief Return the soundfile size
     */
    virtual int             getSoundSize()                                  = 0;
    /**
     *  @brief Launch computation of the target features
     */
    virtual bool            computeFeatures()                               = 0;
    /**
     *  @brief Launch computation of the target features and keep freezed features
     */
    virtual bool            computeFeatures(sqlStruct& oldFeatures)         = 0;
    /**
     *  @brief Add a specfic feature to the target
     */
    virtual void            addFeature(string fName, matrixPtr fValue)      = 0;
    /**
     *  @brief Compute events on a waveform
     */
    virtual vector<int>     computeEvents()                                 = 0;
    /**
     *  @brief Compute events on a waveform
     */
    virtual void            setLoopRegion(int begin, int end)               = 0;
    
    /**
     *  @brief Check if target has been analyzed
     */
    bool                    getComputed();
    /**
     *  @brief Retrieve the value of a specific feature
     */
    matrixPtr               getFeature(string fName);
    /**
     *  @brief Retrieve all features
     */
    sqlStruct               getFeatures();
    /**
     *  @brief Retrieve the value of a specific feature
     */
    matrixPtr               getFeature(sqlStruct& targetFeatures, string fName);
    /**
     *  @brief Retrieve the names of all features
     */
    vector<string>          getFeaturesNames();
    /**
     *  @brief Modify the value of a target feature
     */
    void                    modifyFeature(string fName, matrixPtr fValue);
    /**
     *  @brief Retrieve all features
     */
    sqlStruct               getFeaturesList();
    /**
     *  @brief Set parameters for the features analysis
     */
    void                    setAnalysisParameters(string configFile);
    /**
     *  @brief Retrieve analysis parameters
     */
    analysis_params         getTargetParameters();
    /**
     *  @brief Clear harmonic filters
     */
    void                    emptyHarmonicFilters();
    /**
     *  @brief Compute harmonic filters
     */
    vector<vector<string> >  getHarmonicFilters();
    /**
     *  @brief Get unique harmonic filters
     */
    vector<string>          getUniqueHarmonicFilters();
    /**
     *  @brief Freeze or unfreeze on feature
     */
    void                    freezeFeature(bool freeze, string descriptor);
    /**
     *  @brief Get feature feature
     */
    int                     getFreeze(string descriptor);
    /**
     *  @brief Set partials frequency of the target
     */
    void                    setPartialsFrequency(matrixPtr parFreq);
    /**
     *  @brief Set partials amplitude of the target
     */
    void                    setPartialsAmplitude(matrixPtr parAmp);
    /**
     *  @brief Set duration of the target
     */
    void                    setDuration(float dur);
    /**
     *  @brief Copy the previous features to abstract target
     */
    bool                    copyFeatures(sqlStruct& oldFeatures);
    /**
     *  @brief Dump target features for debug
     */
    void                    dumpTargetFeatures(string filename, matrixPtr tSp, matrixPtr tMSp);
};

/**
 *  @class TargetSound
 *
 *  @brief Main class for soundfile target
 *
 *  This class allows to define a target from a soundfile
 */
class TargetSound : public Target
{
protected:
    string  soundFile;      ///< Name of the target soundfile
    int     signalSize;     ///< Length of signal
    float   sRate;          ///< Sampling rate of audio
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param filename Name of the target soundfile
     */
    TargetSound(Session* sessObj, string filename);
    /**
     *  @brief Destructor
     */
    ~TargetSound(){};
    
    /**
     *  @brief Return the soundfile name
     */
    string          getSoundName();
    /**
     *  @brief Return the soundfile size
     */
    int             getSoundSize(){return signalSize;};
    /**
     *  @brief Launch computation of the target features
     */
    bool            computeFeatures();
    /**
     *  @brief Launch computation of the target features and keep freezed features
     */
    bool            computeFeatures(sqlStruct& oldFeatures);
    /**
     *  @brief Add a specfic feature to the target
     */
    void            addFeature(string fName, matrixPtr fValue);
    /**
     *  @brief Load a different soundfile target
     */
    void            loadSoundFile(string filename);
    /**
     *  @brief Compute events on a waveform
     */
    vector<int>     computeEvents();
    void            setLoopRegion(int begin, int end);
};

/**
 *  @class TargetAbstract
 *
 *  @brief Main class for abstract target
 *
 *  This class allows to define an abstract target
 */
class TargetAbstract : public Target
{
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     */
    TargetAbstract(Session* sessObj);
    /**
     *  @brief Destructor
     */
    ~TargetAbstract(){};
    
    /**
     *  @brief Return the soundfile name
     */
    string          getSoundName();
    /**
     *  @brief Return the soundfile size
     */
    int             getSoundSize(){return 0;};
    /**
     *  @brief Launch computation of the target features
     */
    bool            computeFeatures();
    /**
     *  @brief Launch computation of the target features and keep freezed features
     */
    bool            computeFeatures(sqlStruct& oldFeatures);
    /**
     *  @brief Add a specfic feature to the target
     */
    void            addFeature(string fName, matrixPtr fValue);
    /**
     *  @brief Compute events on a waveform
     */
    vector<int>     computeEvents();
    void            setLoopRegion(int begin, int end);
};

/**
 *  @class MultiTarget
 *
 *  @brief Main class for multi target
 *
 *  This class allows to define a multi target
 */
class MultiTarget : public Target
{
protected:
    string                  soundFile;      ///< Name of the target soundfile
    int                     signalSize;     ///< Length of signal
    float                   sRate;          ///< Sampling rate of audio
    vector<pair<int, int> > segmentsMarker; ///< Markers to segment the soundfile
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     */
    MultiTarget(Session* sessObj, string filename);
    /**
     *  @brief Destructor
     */
    ~MultiTarget(){};
    
    /**
     *  @brief Return the soundfile name
     */
    string          getSoundName();
    /**
     *  @brief Return the soundfile size
     */
    int             getSoundSize(){return signalSize;};
    /**
     *  @brief Launch computation of the target features
     */
    bool            computeFeatures();
    /**
     *  @brief Launch computation of the target features and keep freezed features
     */
    bool            computeFeatures(sqlStruct& oldFeatures);
    /**
     *  @brief Add a specfic feature to the target
     */
    void            addFeature(string fName, matrixPtr fValue);
    /**
     *  @brief Load a different soundfile target
     */
    void            loadSoundFile(string filename);
    /**
     *  @brief Set segments markers
     */
    void            setSegments(vector<int> segments);
    /**
     *  @brief Set segments markers
     */
    vector<int>     getSegments();
    /**
     *  @brief Segment the soundfile at markers at index i
     */
    sqlStruct       segmentSoundFile(int i);
    /**
     *  @brief Return the signal size
     */
    int             getSignalSize();
    /**
     *  @brief Compute events on a waveform
     */
    vector<int>     computeEvents();
    /**
     *  @brief Keep one solution per segment based on its PartialsAmplitude optimization
     */
    PopulationPtr   setContinuity(vector<PopulationPtr> sols);
    void            setLoopRegion(int begin, int end);
    float           getSegmentDuration(int k);
};

#endif /* defined(__Orchids__target__) */
