//
//  scalarize.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "scalarize.h"


vector<float> scalarize(
                        matrixPtr        criteria,
                        float*           weights,
                        int              sizeWeights,
                        vector<float>&   ideal,
                        vector<float>&   ranges)
{
    int i, j;
    /*
    int n = criteria.rows();
    matrixPtr repmatIdeal(n, (int)ideal.size());
    for (i = 0; i < repmatIdeal.rows(); i++)
        for (j = 0; j < repmatIdeal.cols(); j++)
            repmatIdeal(i, j) = ideal[j];
    criteria - repmatIdeal;

    matrixPtr repmatRanges(n, (int)ranges.size());
    for (i = 0; i < repmatRanges.rows(); i++)
        for (j = 0; j < repmatRanges.cols(); j++)
            repmatRanges(i, j) = ranges[j];
    criteria / repmatRanges;

    matrixPtr repmatWeights(n, sizeWeights);
    for (i = 0; i < repmatWeights.rows(); i++)
        for (j = 0; j < repmatWeights.cols(); j++)
            repmatWeights(i, j) = weights[j];
    criteria * repmatWeights;
    */
    
    matrixPtr critCopy = matrixPtr(new matrix(criteria));
    for (i = 0; i < critCopy->rows(); i++)
        for (j = 0; j < critCopy->cols(); j++)
            critCopy->operator()(i, j) = (critCopy->operator()(i, j) - ideal[j]) * weights[j] / ranges[j];
    
    return critCopy->Max(2);
}