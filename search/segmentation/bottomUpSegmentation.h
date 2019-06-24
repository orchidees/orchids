//
//  bottomUpSegmentation.h
//  Orchids
//
//  Created by Antoine Bouchereau on 11/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__bottomUpSegmentation__
#define __Orchids__bottomUpSegmentation__

#include "Orchids.h"

/**
 *  @brief The basic algorithm works by creating a fine segmented representation then merging the lowest cost segments until only 'num_segments' remain
 *  @param data Time-series data
 *  @param num_segments Number of segments to obtain
 */
vector<segment> bottomUpSegmentation(matrixPtr data, int num_segments);

#endif /* defined(__Orchids__bottomUpSegmentation__) */
