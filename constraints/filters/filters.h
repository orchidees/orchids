//
//  filters.h
//  Orchids
//
//  Created by Antoine Bouchereau on 20/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__filters__
#define __Orchids__filters__

#include "Orchids.h"

class Session;

/**
 *  @class Filters
 *
 *  @brief Virtual class for filters
 */
class Filters
{
protected:
    Session*    sSession;      ///< Current session
    string      fAttribute;     ///< Attribute on which the filter apply
    string      fMode;          ///< Mode of application
    string      fName;          ///< Name of the filter
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param name Filter name
     *  @param attribute Filter type
     */
    Filters(Session* sessObj, string name, string attribute);
    /**
     *  @brief Virtual destructor
     */
    virtual ~Filters(){};
    
    /**
     *  @brief Return filter attribute
     */
    string          getAttribute();
    /**
     *  @brief Return filter mode
     */
    string          getMode();
    /**
     *  @brief Return filter name
     */
    string          getName();
    /**
     *  @brief Set filter mode
     *  @param mode New filer mode
     */
    void            setMode(string mode);
    /**
     *  @brief Initialize filter
     */
    void            initialize();
    /**
     *  @brief Check filter feasibility
     */
    bool            checkFeasibility();
};

/**
 *  @class FiltersSymbolic
 *
 *  @brief Filter on symbolic informations
 */
class FiltersSymbolic : public Filters
{
protected:
    vector<string> includeList;             ///< Values to include in the search space
    vector<string> valueList;               ///< Complete list of values
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param name Filter name
     *  @param attribute Filter type
     */
    FiltersSymbolic(Session* sessObj, string name, string attribute);
    /**
     *  @brief Destructor
     */
    ~FiltersSymbolic(){};
    
    /**
     *  @brief Set values to filter
     *  @param vals Values to apply
     */
    void            setValuesList(vector<string> vals);
    /**
     *  @brief Set include values to filter
     *  @param vals Values to include
     */
    void            setIncludeList(vector<string> vals);
    /**
     *  @brief Add values to include values list
     *  @param vals Values to include
     */
    void            addIncludeList(vector<string> vals);
    /**
     *  @brief Return filter values
     */
    vector<string>  apply();
};

/**
 *  @class FiltersTemporal
 *
 *  @brief Filter on temporal series
 */
class FiltersTemporal : public Filters
{
protected:
    vector<float> timeShape;    ///< Time shape of temporal filter
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param name Filter name
     *  @param attribute Filter type
     */
    FiltersTemporal(Session* sessObj, string name, string attribute);
    /**
     *  @brief Destructor
     */
    ~FiltersTemporal(){};
};

/**
 *  @class FiltersSpectral
 *
 *  @brief Filter on spectral values
 */
class FiltersSpectral : public Filters
{
protected:
    pair<float, float> valuesRange; ///< Complete list of values
    pair<float, float> filterRange; ///< Values range of spectral filter
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param name Filter name
     *  @param attribute Filter type
     */
    FiltersSpectral(Session* sessObj, string name, string attribute);
    /**
     *  @brief Destructor
     */
    ~FiltersSpectral(){};
    
    /**
     *  @brief Set spectral values range
     *  @param range Range values
     */
    void                setValuesRange(pair<float, float> range);
    /**
     *  @brief Set filter values range
     *  @param range Range values
     */
    void                setFilterRange(pair<float, float> range);
    /**
     *  @brief Return filter values
     */
    pair<float, float>  apply();
};

#endif /* defined(__Orchids__filters__) */
