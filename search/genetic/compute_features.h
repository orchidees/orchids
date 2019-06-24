//
//  compute_features.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__compute_features__
#define __Orchids__compute_features__

#include "Orchids.h"

/**
 *  @brief Ouput a structure of feature matrices for each feature in the input feature cell array. In each feature matrix, each line i is the estimation of the feature vector for the soundset i. COMPUTE_FEATURES is a generic method that calls appropriate methods in the /compute/ sub-directory
 *  @param population Population to compute
 *  @param optFeatures Features to optimize
 *  @param partials_compute Structure to fill with partials addition
 */
features compute_features(
                          PopulationPtr         population,
                          vector<FeaturesPtr>&  optFeatures);

#endif /* defined(__Orchids__compute_features__) */
