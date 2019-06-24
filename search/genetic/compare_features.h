//
//  compare_features.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__compare_features__
#define __Orchids__compare_features__

#include "Orchids.h"

/**
 *  @brief Ouput a structure of perceptual dissimilaries between target and sound combinations for each feature in the input soundset feature structure. In each dissimilarity vector each element i is the dissimilarity between the target and soundset i for the associated descriptor. COMPARE_FEATURES is a generic method that calls appropriate methods in the /compare/ sub-directory
 *  @param target_features Target features
 *  @param soundset_features Features to compare
 *  @param optFeatures Features to optimize
 *  @param partials_compare Structure to fill with partials comparison 
 */
map<string, matrixPtr> compare_features(
                                     sqlStruct&            target_features,
                                     features&             soundset_features,
                                     vector<FeaturesPtr>&  optFeatures);

#endif /* defined(__Orchids__compare_features__) */
