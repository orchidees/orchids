//
//  select_population.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "select_population.h"


vector<float> select_population(
                                vector<float>    &fitness,
                                int              mating_pool_size,
                                vector<int>&     mating_pool_idx)
{
    int             i, j;
    int             n       = (int)fitness.size();
    vector<float>   If      = gen_random_float_vector<float>(0, n, mating_pool_size * 2);
    matrixIPtr      I(new matrixI(mating_pool_size, 2));
    int             index   = 0;
    for (i = 0; i < I->cols(); i++)
        for (j = 0; j < I->rows(); j++)
            I->operator()(j, i) = floor(If[index++]);

    matrixPtr F(new matrix(I->rows(), I->cols()));
    for (i = 0; i < F->rows(); i++)
        for (j = 0; j < F->cols(); j++)
            F->operator()(i, j) = fitness[I->operator()(i, j)];

    vector<float>   min;
    vector<int>     pos;
    F->Min(2, min, pos);
    int* mating_pool_array;
    linspace(0, mating_pool_size - 1, mating_pool_size, mating_pool_array);
    for (i = 0; i < mating_pool_size; i++)
    {
        mating_pool_array[i] = mating_pool_array[i] * 2 + pos[i];
        mating_pool_idx.push_back(I->operator()(mating_pool_array[i]));
    }
    free(mating_pool_array);
    return min;
}