//
//  approximateEntropy.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 08/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//
// Author: Chesnokov Yuriy

#include "approximateEntropy.h"


float approximateEntropy(int m, float r, float std, vector<float>& data)
{
    int N = (int)data.size();
    int Cm = 0, Cm1 = 0;
    double err = 0.0, sum = 0.0;
    
    err = std * r;
    
    for (unsigned int i = 0; i < N - (m + 1) + 1; i++) {
        Cm = Cm1 = 0;
        for (unsigned int j = 0; j < N - (m + 1) + 1; j++) {
            bool eq = true;
            //m - length series
            for (unsigned int k = 0; k < m; k++) {
                if (abs(data[i+k] - data[j+k]) > err) {
                    eq = false;
                    break;
                }
            }
            if (eq) Cm++;
            
            //m+1 - length series
            int k = m;
            if (eq && abs(data[i+k] - data[j+k]) <= err)
                Cm1++;
        }
        
        if (Cm > 0 && Cm1 > 0)
            sum += log((double)Cm / (double)Cm1);
    }
    
    return sum / (double)(N - m);
}