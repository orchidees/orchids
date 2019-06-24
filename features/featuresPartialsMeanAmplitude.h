//
//  featuresPartialsMeanAmplitude.h
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__featuresPartialsMeanAmplitude__
#define __Orchids__featuresPartialsMeanAmplitude__

#include "Orchids.h"

/**
 *  @class FeaturesPartialsMeanAmplitude
 *  @brief Feature definition
 *  This class defines the behavior of the mean values for partials amplitude structure
 */
class FeaturesPartialsMeanAmplitude : public Features
{
public:
    /**
     *  @brief Constructor
     *  @param name Feature name
     */
    FeaturesPartialsMeanAmplitude(string name): Features(name){};

    /**
     *  @brief     This function allows to obtain a list of dependancy for the feature. Each item on the list is needed for subsequent operation
     */
    vector<string>          depandancy();
    /**
     *  @brief     Function for computing the value of the features. The proposed source could be a soundfile or straight signal
     */
    void                    analysis();
    /**
     *  @brief     Function for computing the addition of values for this feature over a set of solutions.
     */
    variant                 addition(PopulationPtr population);
    /**
     *  @brief     Function for computing the distances between a set of possible solutions features and the features of a target
     */
    matrixPtr                  compare(features& soundsetFeatures, sqlStruct& targetFeatures);
    /**
     *  @brief     This function allows to obtain the transposition of a features value. This is particularly useful for microtonic resolutions
     */
    void                    transpose(IndividualPtr individual);
    /**
     *  @brief     This function returns the neutral element value for this feature
     */
    matrixPtr                  neutral();
};


#endif