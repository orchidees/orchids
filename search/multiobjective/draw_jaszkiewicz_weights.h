//
//  draw_jaszkiewicz_weights.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__draw_jaszkiewicz_weights__
#define __Orchids__draw_jaszkiewicz_weights__

#include "Orchids.h"

/**
 *  @brief Draw a set of N K-dimension weight vectors uniformly spread on the hyperplane w1+w2+...+wK = 1. The generative method is the one suggested by Jaszkiewicz in Andrzej Jaszkiewicz, Comparison of Local Search-Based Metaheuristics on the Multiple Objective Knapsack Problem, Foundations of Computing and Design Sciences 26 (2001), no. 1, 99-120
 */
matrixPtr draw_jaszkiewicz_weights(int N, int K);

#endif /* defined(__Orchids__draw_jaszkiewicz_weights__) */
