//
//  indexRepresentation.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__indexRepresentation__
#define __Orchids__indexRepresentation__

#include "Orchids.h"

/**
 *  @brief Return matrix reprensentation of the index
 */
matrix indexRepresentation(
                           vector<float>&  rawData,
                           int             winLen,
                           int             wordLen,
                           int             cardinality,
                           vector<float>&  cutPoints);

/**
 *  @brief Compute the index of a node from a binary vector
 */
int getHash(vector<int>& binary);

#endif /* defined(__Orchids__indexRepresentation__) */
