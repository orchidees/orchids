//
//  calcN6.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "calcN6.h"

float calcN6(matrixPtr meln_s)
{
    int i;
    matrixPtr amp_m1 = matrixPtr(new matrix(meln_s));
    amp_m1->operator^(0.6);
    matrixPtr amp_m2 = matrixPtr(new matrix(meln_s));
    amp_m2->operator^(2);
    float          *loud_v, *ener_v;
    float          maxener;
    vector<int>    pos_v;
    
    loud_v      = amp_m1->getSum(1);
    ener_v      = amp_m2->getSum(1);
    maxener     = getMax(ener_v, amp_m2->cols());
    
    for (i = 0; i < amp_m2->cols(); i++)
        if (ener_v[i] > (0.001 * maxener))
            pos_v.push_back(i);
    reorder(loud_v, amp_m1->cols(), pos_v, loud_v);
    
    float calc;
    if (pos_v.size() == 0)
        calc = 0;
    else
        calc = 10 * log2(percentile(loud_v, (int)pos_v.size(), 0.94));
    free(loud_v);
    free(ener_v);
    return calc;
}