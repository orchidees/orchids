//
//  features.h
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__features__
#define __Orchids__features__

#include "Orchids.h"

class Population;
class Individual;
class Session;


/**
 *  @class Features
 *  @brief Abstract definition for the features system
 *  This class defines the abstract behavior required for the definition of any optimizable feature.
 */
class Features
{
protected:
    vector<float>   m_weights;      ///< Weights used in the feature relevance feedback
    string          m_name;         ///< Name of optimization feature
    Session*        sSession;       ///< Current orchestration session
    
public:
    /**
     *  @brief Constructor
     *  @param name Feature name
     */
    Features(string name): m_name(name) {};
    /**
     *  @brief Destructor
     */
    ~Features(){};
    
    /**
     *  @brief Set user current session
     *  @param s Pointer to the current session
     */
    void    setSession(Session* s){ sSession = s; }
    /**
     *  @brief Get the current feature name
     */
    string  getFeatureName(){ return m_name; }
    
    /**
     *  @brief     This function allows to obtain a list of dependancy for the feature. Each item on the list is needed for subsequent operation
     */
    virtual vector<string>  depandancy()                                                    = 0;
    /**
     *  @brief     Function for computing the value of the features. The proposed source could be a soundfile or straight signal
     */
    virtual void            analysis()                                                      = 0;
    /**
     *  @brief     Function for computing the addition of values for this feature over a set of solutions.
     */
    virtual variant         addition(PopulationPtr population)                              = 0;
    /**
     *  @brief     Function for computing the distances between a set of possible solutions features and the features of a target
     */
    virtual matrixPtr          compare(features& soundsetFeatures, sqlStruct& targetFeatures)  = 0;
    /**
     *  @brief     This function allows to obtain the transposition of a features value. This is particularly useful for microtonic resolutions
     */
    virtual void            transpose(IndividualPtr individual)                             = 0;
    /**
     *  @brief     This function returns the neutral element value for this feature
     */
    virtual matrixPtr          neutral()                                                       = 0;
};


#endif 