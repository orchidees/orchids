//
//  tauThompsonDistribution.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 02/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "tauThompsonDistribution.h"


vector<int> tauThompsonDistribution(vector<float>& distribution)
{
    int i;
    // Mean of distribution
    float meanDistrib = getMean(&distribution[0], (int)distribution.size());
    // Deviation of distribution
    float devDistrib = std_dev(&distribution[0], (int)distribution.size(), meanDistrib);
    // Size of the distribution
    int n  = (int)distribution.size();
    // Result vector
    vector<int> isOutlier(n, 0);
    if (n <= 2)
        return isOutlier;

    float curValue, curDeviation, tAlph, reject;
    for (i = 0; i < n; i++)
    {
        // Current value
        curValue = distribution[i];
        // Deviation of current value to mean distribution
        curDeviation = abs(meanDistrib - curValue);
        // Critical value of inverse t-Student
        tAlph = alglib::invstudenttdistribution(n - 2, 0.99);
        // Rejection region
        reject = ((tAlph * (n - 1)) / (sqrt(n) * sqrt(n - 1 + (pow(tAlph, 2))))) * devDistrib;
        // Test if deviation is in the rejection region
        if (curDeviation > reject || isnan(reject))
            isOutlier[i] = 1;
    }
    
    return isOutlier;
}