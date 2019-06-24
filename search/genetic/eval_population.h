//
//  eval_population.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__eval_population__
#define __Orchids__eval_population__

#include "Orchids.h"

/**
 *  @brief Compute the vector distances between the individuals of a population and the target sound. Ouput is a matrixPtr where each line is an distance vector associated with one individual, and each column is the perceptual dissimilarity along one optimization criteria
 *  @param population Population to evaluate
 *  @param target_features Target features
 *  @param optFeatures Features to optimize
 */
matrixPtr eval_population(
                       PopulationPtr        population,
                       sqlStruct&           target_features,
                       vector<FeaturesPtr>& optFeatures);

#endif /* defined(__Orchids__eval_population__) */
