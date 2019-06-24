//
//  preserveDiversity.h
//  Orchids
//
//  Created by Antoine Bouchereau on 16/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__preserveDiversity__
#define __Orchids__preserveDiversity__

#include "Orchids.h"

/**
 *  @brief Sample a vector set in such a way to maximize density homogeneity (i.e. to preserve diversity). Vector in denser regions are iteratively removed from the set, until the set size falls under a given threshold N. Density is computed thanks to the PADE method
 */
vector<int> preserveDiversity(matrixPtr C, int N, string mode);

#endif /* defined(__Orchids__preserveDiversity__) */
