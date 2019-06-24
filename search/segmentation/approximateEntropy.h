//
//  approximateEntropy.h
//  Orchids
//
//  Created by Antoine Bouchereau on 08/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__approximateEntropy__
#define __Orchids__approximateEntropy__

#include "Orchids.h"

/**
 *  @brief Compute approximate entropy
 *  @param dim Embedded dimension
 *  @param r Tolerance (usually 0.2)
 *  @param std Standard deviation
 *  @param data Time-series data
 */
float approximateEntropy(int dim, float r, float std, vector<float>& data);

#endif /* defined(__Orchids__approximateEntropy__) */
