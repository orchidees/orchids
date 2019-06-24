//
//  computeEffortsTimes.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 05/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "computeEffortsTimes.h"


vector<int> computeEffortsTimes(vector<float>& s, vector<float>& thresholds)
{
    int i;
    vector<int> effortTimes(thresholds.size(), 0);
    int         currentThresh = 0;
    for (i = 0; i < s.size(); i++)
    {
        // If s[i] >= thresholds[currentThresh]
        if (fabs(s[i] - thresholds[currentThresh]) < std::numeric_limits<float>::epsilon() || s[i] > thresholds[currentThresh])
        {
            effortTimes[currentThresh] = i;
            currentThresh++;
            if (currentThresh == thresholds.size())
                break;
        }
    }
    return effortTimes;
}