//
//  compare_features.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "compare_features.h"


map<string, matrixPtr> compare_features(sqlStruct& target_features, features& soundset_features, vector<FeaturesPtr>& optFeatures)
{
    int i;
    map<string, matrixPtr> criteria;
    for (i = 0; i < optFeatures.size(); i++)
    {
        try {
            criteria[optFeatures[i]->getFeatureName()] = optFeatures[i]->compare(soundset_features, target_features);
        } catch (const char* e) {
            throw e;
        }
    }
    return criteria;
}