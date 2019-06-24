//
//  draw_jaszkiewicz_weights.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "draw_jaszkiewicz_weights.h"

// DRAW_JASZKIEWICZ_WEIGHTS - Draw a set of N K-dimension weight
// vectors uniformly spread on the hyperplane w1+w2+...+wK = 1. The
// generative method is the one suggested by Jaszkiewicz in
// Andrzej Jaszkiewicz, Comparison of Local Search-Based
// Metaheuristics on the Multiple Objective Knapsack Problem,
// Foundations of Computing and Design Sciences 26 (2001), no. 1, 99-120.

matrixPtr draw_jaszkiewicz_weights(int N, int K)
{
    int i, j;
    matrixPtr W(new matrix(N, K));
    vector<float> E = gen_random_float_vector<float>(0, 1, (K - 1) * N);
    vector<float> thisE(E.begin(), E.begin() + N);
    for (i = 0; i < N; i++)
    {
        thisE[i] = pow(thisE[i], 1.0f / ((float)K - 1));
        W->operator()(i, 0) = 1 - thisE[i];
    }
    for (i = 2; i <= K - 1; i++)
    {
        thisE.clear();
        thisE.insert(thisE.begin(), E.begin() + (i - 1) * N, E.begin() + i * N);
        float *sum = W->getSum(2);
        for (j = 0; j < thisE.size(); j++)
            W->operator()(j, i - 1) = (1 - sum[j]) * (1 - pow(thisE[j], 1.0f / ((float)(K - i))));
        free(sum);
    }
    float *sum = W->getSum(2);
    for (i = 0; i < N; i++)
        W->operator()(i, K - 1) = 1 - sum[i];
    free(sum);
    
    return W;
}