//
//  production.h
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__production__
#define __Orchids__production__

#include "Orchids.h"

typedef vector<boost::tuple<string, string, string> > structNomenclature;

class Filters;

/**
 *  @class Production
 *
 *  @brief Abstract interface for defining a production mean
 *  This class allows to define an abstract production
 */
class Production
{
protected:
    vector<int>                             variableTable;              ///< Full table of variable domains
    vector<vector<int>>                     variableResolution;         ///< Full table of variable domains
    vector<IndividualPtr>                   variableIndividuals;        ///< Table of all allowed individuals
    vector<pair<string, vector<int> > >     variableDomains;            ///< Domains of allowed variable
    map<string, vector<int> >               variableOnsetsDomains;      ///< Domains of allowed variable by harmonic filter
    map<string, vector<int> >               variableOnsetsIDs;          ///< Domains of allowed variable by harmonic filter
    vector<vector<int> >                    filteredOnsets;             ///< Table of onset depending on harmonics
    map<string, vector<int> >               variableDurationsDomains;   ///< Domains of allowed durations by harmonic filter
    vector<vector<int> >                    filteredDurations;          ///< Table of onset depending on harmonics
    vector<int>                             maxOnsetValues;             ///< Maximum value of instrument onset
    vector<int>                             maxDurationValues;          ///< Maximum value of instrument duration
    map<string, tpl>                        attributeDomains;           ///< Domains of allowed attributes
    vector<string>                          allowedInstruments;         ///< Instruments used for production
    int                                     microtonicResolution;       ///< Microtonic production resolution
    bool                                    harmonicFiltering;          ///< Flag for harmonic filtering
    bool                                    staticMode;                 ///< Flag for static mode
    vector<int>                             allowedResolutions;         ///< List of allowed resolutions
    vector<boost::shared_ptr<Filters> >     filtersSet;                 ///< Set of available filters
    bool                                    isInited;                   ///< Flag for server mode
    Session*                                sSession;                   ///< Current session
    
public:
    /**
     *  @brief Constructor
     *  @param mSession Pointer to the current session
     *  @param instrus List of instruments
     *  @param resolution Microtonic resolution
     */
    Production(Session* mSession, vector<string> &instrus, int resolution);
    /**
     *  @brief Destructor
     */
    ~Production(){};
    
    /**
     *  @brief Set activation of harmonic filtering
     */
    void            setHarmonicFiltering(bool hFilt);
    /**
     *  @brief Set activation of static mode
     */
    void            setStaticMode(bool sMode);
    /**
     *  @brief Modify the microtonic resolution of production
     */
    void            setResolution(int res);
    /**
     *  @brief Set the list of allowed instruments
     */
    virtual void    setInstruments(vector<string> instrus);
    /**
     *  @brief Set a list of filters
     */
    void            setFilters(vector<boost::shared_ptr<Filters> > filters);
    
    /**
     *  @brief Get a default orchestra
     */
    virtual vector<string>              getDefaultOrchestra();
    /**
     *  @brief Do the production need to be initialized?
     */
    void                                needInit();
    /**
     *  @brief Empty all domains
     */
    void                                emptyVariableDomains();
    /**
     *  @brief Retrieve the list of allowed instruments
     */
    vector<string>                      &getInstruments();
    /**
     *  @brief Retrieve the current resolution
     */
    int                                 getResolution();
    /**
     *  @brief Retrieve the maximal onsets
     */
    vector<int>                         &getMaxOnsetValues();
    /**
     *  @brief Retrieve the harmonic filtered onset values
     */
    vector<vector<int> >                 &getFilteredOnsets();
    /**
     *  @brief Retrieve the maximal duration
     */
    vector<int>                         &getMaxDurationValues();
    /**
     *  @brief Retrieve the harmonic filtered onset values
     */
    vector<vector<int> >                 &getFilteredDurations();
    /**
     *  @brief Retrieve organized variable domains
     */
    vector<pair<string, vector<int> > >   &getVariableDomains();
    /**
     *  @brief Retrieve organized variable domains
     */
    vector< vector<int> >               &getVariableResolution();
    /**
     *  @brief Retrieve organized variable domains by instrument and note
     */
    map<string, vector<int> >            &getVariableOnsetsIDs();
    /**
     *  @brief Retrieve organized variable domains by instrument and note
     */
    map<string, vector<int> >            &getVariableOnsetsDomains();
    /**
     *  @brief Retrieve organized variable domains by instrument and note
     */
    map<string, vector<int> >            &getVariableDurationsDomains();
    /**
     *  @brief Get the variable table
     */
    vector<int>                         &getVariableTable();
    /**
     *  @brief Get the variable individuals
     */
    vector<IndividualPtr>               &getVariableIndividuals();
    /**
     *  @brief Get the list of filters
     */
    vector<boost::shared_ptr<Filters> > getFilters();
    /**
     *  @brief Clear all filters
     */
    void                                resetFilters();
    /**
     *  @brief Return static flag
     */
    bool                                isStatic();
    
    /**
     *  @brief Create the set of empty filters and compute their original values
     */
    void constructBaseFilters();
    /**
     *  @brief Constructor
     */
    void computeVariableDomains();
    /**
     *  @brief Compute possible values for every instruments group
     */
    void updateIndividualFeatures(matrixIPtr idSetMatrix, vector<IndividualPtr>& uInds, vector<int>& idSet);
    
    /**
     *  @brief Retrieving the instruments order for score export
     */
    virtual vector<string> getScoreOrder() = 0;
};

/**
 *  @class ProductionOrchestra
 *
 *  @brief Defining an orchestra for production
 *  This class allows to define an orchestra as a production mode
 */
class ProductionOrchestra : public Production
{
public:
    /**
     *  @brief Constructor
     *  @param mSession Pointer to the current session
     *  @param instrus List of instruments
     *  @param resolution Microtonic resolution
     */
    ProductionOrchestra(Session* mSession, vector<string> instrus, int resolution);
    /**
     *  @brief Destructor
     */
    ~ProductionOrchestra(){};
    
    /**
     *  @brief Get a default orchestra
     */
    vector<string>      getDefaultOrchestra();
    /**
     *  @brief Return instruments added by the user
     */
    vector<string>      getUserInstruments();
    /**
     *  @brief Return instruments added by the user
     */
    vector<string>      getUserPlayingStyles();
    /**
     *  @brief Return instruments added by the user
     */
    vector<string>      getSources();
    /**
     *  @brief Set the list of allowed instruments
     */
    void                setInstruments(vector<string> instrus);
    /**
     *  @brief Retrieving the instruments order for score export
     */
    vector<string>      getScoreOrder();
    /**
     *  @brief Return the family of an instrument, specified by its symbol in the nomenclature
     */
    string              get_family(string instrument);
    /**
     *  @brief Theoretic table of instrument families, folders, and symbols. It may contains instruments that are NOT in the current database, in order to help the user when adding new instrument (hopefully avoiding errors when naming file and folders). It is also used for automatically sort the instrument list in the traditional orchestral order
     */
    structNomenclature  nomenclature();
};



#endif /* defined(__Orchids__production__) */
