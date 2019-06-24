//
//  is_dominated_by.h
//  Orchids
//
//  Created by Antoine Bouchereau on 12/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__is_dominated_by__
#define __Orchids__is_dominated_by__

#include "Orchids.h"

/**
 *  @brief Pareto domination test. Return 1 if critB Pareto dominates critA. 'mode' is used to decide between a maximization or minimization problem
 */
vector<int> is_dominated_by(vector<float> critA, matrixPtr critB, string mode);

#endif /* defined(__Orchids__is_dominated_by__) */
