//
//  scalarize.h
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__scalarize__
#define __Orchids__scalarize__

#include "Orchids.h"

/**
 *  @brief Fitness computation. First scale the population in the unit hypercube (thanks to current ideal point and ranges info), then apply a weighted Chebychev norm
 */
vector<float> scalarize(
                        matrixPtr           criteria,
                        float*           weights,
                        int              sizeWeights,
                        vector<float>&   ideal,
                        vector<float>&   ranges);

#endif /* defined(__Orchids__scalarize__) */
