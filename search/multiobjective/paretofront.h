//
//  paretofront.h
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__paretofront__
#define __Orchids__paretofront__

#include "Orchids.h"

/**
 *  @brief Returns the logical Pareto Front of a set of points
 */
void paretofront(bool * front, float * M, unsigned int row, unsigned int col);

#endif /* defined(__Orchids__paretofront__) */
