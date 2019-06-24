//
//  session.h
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__session__
#define __Orchids__session__

#include "Orchids.h"

#define DEBUG_MODE  0
#define DEBUG_DIR   string("/tmp/debug/")
#define SOL_DIR     string("/Users/esling/Desktop/SOL_0.9c_HQ")

class KnowledgeBDB;
class OSCListener;
class Search;
class Target;
class Production;
class Features;

/**
 *  @class Session
 *
 *  @brief Main class for user interactivity
 *
 *  This class centralize every aspect of a search problem. It can be considered as a workspace which contains every defined parts of the search
 */
class Session
{    
protected:
    vector<FeaturesPtr> sFeatures;      ///< Features object to use
    vector<string>      sFeatList;      ///< List of string features
    vector<string>      tFeatures;      ///< Total features list (with dependancies)
    SearchPtr           sSearch;        ///< Search algorithm used
    PopulationPtr       sSolution;      ///< Solution set to the search
    TargetPtr           sTarget;        ///< Target to optimize
    KnowledgeBDBPtr     sKnowledge;     ///< Knowledge source
    ProductionPtr       sProduction;    ///< Type of production output
    OSCListenerPtr      sOSCHandle;     ///< Handles for OSC server
    bool                isMultiTarget;  ///< The target is segmented or not
    int                 debugMode;      ///< Output plots and verbose
    
public:
    /**
     *  @brief Constructor
     */
    Session(): debugMode(0) {};
    /**
     *  @brief Destructor
     */
    ~Session(){};
    
    /**
     *  @brief Set the session in debug mode
     */
    void                    setDebug() { debugMode = 1; };
    /**
     *  @brief Check if the session is in debug mode
     */
    int                     isDebug()  { return debugMode; };
    
    void                    setOSCStart();
    void                    setOSCJoin();
    
    /**
     *  @brief Fill the session with default values
     */
    void                    constructDefaultSession();
    /**
     *  @brief Close a session
     */
    void                    exitSession();
    
    /**
     *  @brief Set the current target
     */
    void                    setTarget(TargetPtr t);
    /**
     *  @brief Retrieve the current target
     */
    TargetPtr               getTarget();
    /**
     *  @brief Launch target features analysis
     */
    bool                    computeTargetFeatures();
    /**
     *  @brief Launch target features analysis and keep freezed descriptors
     */
    bool                    computeTargetFeatures(sqlStruct& oldFeatures);
    
    /**
     *  @brief Set the current search algorithm
     */
    void                    setSearch(SearchPtr s);
    /**
     *  @brief Retrieve the current search algorithm
     */
    SearchPtr               getSearch();
    
    /**
     *  @brief Set the current knowledge
     */
    void                    setKnowledge(KnowledgeBDBPtr k);
    /**
     *  @brief Retrieve the current knowledge source
     */
    KnowledgeBDBPtr         getKnowledge();
    
    /**
     *  @brief Set the current production type
     */
    void                    setProduction(ProductionPtr p);
    /**
     *  @brief Set the current orchestra
     */
    void                    setOrchestra(vector<string> orch);
    /**
     *  @brief Retrieve the actual production model
     */
    ProductionPtr           getProduction();
    
    /**
     *  @brief Retrieve the current solutions set
     */
    PopulationPtr           getSolution();
    /**
     *  @brief Get handle for the OSC communication
     */
    OSCListenerPtr          getHandle();
    
    /**
     *  @brief Set the list of optimization criteria
     */
    void                    setCriteriaList(vector<string> critList);
    /**
     *  @brief Get the current criteria list
     */
    vector<string>          getCriteriaList();
    /**
     *  @brief Get a list of default criteria
     */
    vector<string>          getDefaultCriteria();
    
    /**
     *  @brief Retrieve the current features
     */
    vector<FeaturesPtr>     getFeatures();
    /**
     *  @brief Get features name list
     */
    vector<string>          getFeaturesList();
    /**
     *  @brief Set the full features list
     */
    void                    setTotalFeaturesList();
    /**
     *  @brief Get complete features list
     */
    vector<string>          getTotalFeaturesList();
    /**
     *  @brief Set the MultiTarget flag
     */
    void                    setMultiTarget(bool set);
    /**
     *  @brief Return the MultiTarget flag
     */
    bool                    getIsMultiTarget();
    
    /**
     *  @brief Launch the search algorithm. First update the search structure to obtain the right domains
     */
    PopulationPtr           launchSearch();
    /**
     *  @brief AutoPilot computes the list of descriptors with high variation and set Search and Target mode to optimize the orchestration problem
     */
    int                     autoPilot();
};

#endif /* defined(__Orchids__session__) */
