//
//  paretoGroup.h
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__paretoGroup__
#define __Orchids__paretoGroup__

#include "Orchids.h"

/**
 *  @brief To get the Pareto Front from a given set of points
 *
 *  Identify the Pareto Front from a set of points in objective space is the most important and also the most time-consuming task in multi-objective optimization. This code splits the given objective set into several smaller groups to be examined by the efficient paretofront algorithm. Then, the Pareto Fronts of each group are combined as one set to be checked by the paretofront algorithm to determine the overall Pareto Front. In this way, the overal computation time can be reduced about half
 */
bool* paretoGroup(matrixPtr X);

#endif /* defined(__Orchids__paretoGroup__) */
