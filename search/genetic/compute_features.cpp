//
//  compute_features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "compute_features.h"


features compute_features(PopulationPtr population, vector<FeaturesPtr>& optFeatures)
{
    int i;
    features soundsets_features;
    for (i = 0; i < optFeatures.size(); i++)
    {
        try {
            soundsets_features[optFeatures[i]->getFeatureName()] = optFeatures[i]->addition(population);
        } catch (const char* e) {
            throw e;
        }
    }
    for (i = 0; i < population->getNbSolutions(); i++)
        population->getSolution(i)->setComputed();
    return soundsets_features;
}