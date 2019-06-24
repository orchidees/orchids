//
//  is_dominated_by.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 12/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "is_dominated_by.h"


vector<int> is_dominated_by(vector<float> critA, matrixPtr critB, string mode)
{
    int i, j;
    // Reshape critA to critB size
    matrixPtr cA(new matrix(critB->rows(), (int)critA.size()));
    for (i = 0; i < cA->rows(); i++)
        for (j = 0; j < cA->cols(); j++)
            cA->operator()(i, j) = critA[j];
    
    // Compare A & B vectors
    if (mode == "min")
    {
        // Minimization case
        matrixIPtr min(new matrixI(cA->rows(), cA->cols()));
        //for (i = 0; i < critA.size(); i++)
        //{
            for (i = 0; i < cA->rows(); i++)
                for (j = 0; j < cA->cols(); j++)
                    min->operator()(i, j) = (critB->operator()(i, j) < cA->operator()(i, j)) ? 1 : 0;
        //}
        return min->Min(2);
    }
    else
    {
        // Maximization case
        matrixIPtr max(new matrixI(cA->rows(), cA->cols()));
        //for (i = 0; i < critA.size(); i++)
        //{
            for (i = 0; i < cA->rows(); i++)
                for (j = 0; j < cA->cols(); j++)
                    max->operator()(i, j) = (critB->operator()(i, j) > cA->operator()(i, j)) ? 1 : 0;
        //}
        return max->Min(2);
    }
}