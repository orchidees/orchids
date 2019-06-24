//
//  eval_population.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "eval_population.h"


matrixPtr eval_population(PopulationPtr population, sqlStruct& target_features, vector<FeaturesPtr>& optFeatures)
{
    int i, j;
    // Estimate the perceptual features of the population individuals
    features population_features;
    try {
        population_features = compute_features(population, optFeatures);
    } catch (const char* e) {
        throw e;
    }
    // Compute the dissimilarities between the population individuals and target
    map<string, matrixPtr> criteria_s;
    try {
        criteria_s = compare_features(target_features, population_features, optFeatures);
    } catch (const char* e) {
        throw e;
    }
    
    // Convert the structure output by compare_features into a matrix
    matrixPtr criteria(new matrix(criteria_s[optFeatures[0]->getFeatureName()]->rows(), (int)optFeatures.size()));
    for (i = 0; i < optFeatures.size(); i++)
        for (j = 0; j < criteria->rows(); j++)
            criteria->operator()(j, i) = criteria_s[optFeatures[i]->getFeatureName()]->operator()(j, 0);
    population->setCriteria(criteria);
    
    // We keep in memory only descriptors features (useful for adaptive optimization function)
    features::iterator it;
    for (it = population_features.begin(); it != population_features.end(); ++it)
        if (it->second.which() == 0)
            // Case matrixPtr (we don't want partials)
            population->setFeature(it->first, boost::get<matrixPtr>(it->second));

    return criteria;
}