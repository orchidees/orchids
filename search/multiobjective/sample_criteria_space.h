//
//  sample_criteria_space.h
//  Orchids
//
//  Created by Antoine Bouchereau on 09/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__sample_criteria_space__
#define __Orchids__sample_criteria_space__

#include "Orchids.h"

/**
 *  @brief Remove dominated elements from a criteria set by random direction drawing. Call this method before extracting pareto sets on large population to decrease the computational cost
 */
matrixPtr sample_criteria_space(matrixPtr C, int k, int& n);

#endif /* defined(__Orchids__sample_criteria_space__) */
