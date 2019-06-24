//
//  sample_criteria_space.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 09/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "sample_criteria_space.h"


matrixPtr sample_criteria_space(matrixPtr C, int randDraws, int& n)
{
    int i, j, k;
    int dim = C->cols();
    float* random = gen_random_float_array<float>(0, 1, randDraws * dim);
    matrixPtr W(new matrix(random, true, randDraws, dim));
    
    for (i = 0; i < randDraws; i++)
    {
        float* row = W->getRow(i);
        matrixPtr onesW(new matrix(C->rows(), W->cols()));
        for (j = 0; j < onesW->rows(); j++)
            for (k = 0; k < onesW->cols(); k++)
                onesW->operator()(j, k) = row[k];
        matrixPtr CW = C;
        CW->operator*(onesW);
        
        vector<float>   max = CW->Max(2);
        float           min;
        int             pos;
        getMin(&max[0], CW->rows(), min, pos);
        
        float* P = C->getRow(pos);
        onesW->resizeTo(C->rows(), C->cols(), false);
        for (j = 0; j < onesW->rows(); j++)
            for (k = 0; k < onesW->cols(); k++)
                onesW->operator()(j, k) = P[k];
        
        matrixPtr idx(new matrix(C->rows(), C->cols()));
        for (j = 0; j < idx->rows(); j++)
            for (k = 0; k < idx->cols(); k++)
                if (C->operator()(j, k) > onesW->operator()(j, k))
                    idx->operator()(j, k) = 1;
        
        vector<float>   minF = idx->Min(2);
        vector<int>     indexes;
        for (j = 0; j < minF.size(); j++)
            if (minF[j] == 0)
                indexes.push_back(j);
        if (!indexes.empty())
            C->reorder(indexes);
        
        //free(row);
        //free(P);
    }
    
    n = C->rows();
    return C;
}