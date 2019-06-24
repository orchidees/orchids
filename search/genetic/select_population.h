//
//  select_population.h
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__select_population__
#define __Orchids__select_population__

#include "Orchids.h"

/**
 *  @brief Perform a binary tournament (with replacement) procedure to select individuals on the basis of their fitness. Output is an index vector
 */
vector<float> select_population(
                                vector<float>    &fitness,
                                int              mating_pool_size,
                                vector<int>&     mating_pool_idx);

#endif /* defined(__Orchids__select_population__) */
