//
//  tsMultiLevelSegment.h
//  Orchids
//
//  Created by Antoine Bouchereau on 08/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__tsMultiLevelSegment__
#define __Orchids__tsMultiLevelSegment__

#include "Orchids.h"

/**
 *  @brief Perform segmentation on a time series
 */
void tsMultiLevelSegment(
                         matrixPtr                    tmpSeries,
                         int                        nbSegments,
                         vector<vector<float> >&    segmentSeries,
                         vector<int>&               tmpOn,
                         vector<int>&               durations);

#endif /* defined(__Orchids__tsMultiLevelSegment__) */
