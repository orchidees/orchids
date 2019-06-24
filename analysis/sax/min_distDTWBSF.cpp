//
//  min_distDTWBSF.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "min_distDTWBSF.h"

float min_distDTWBSF(float *Tshape, int TshapeSize, float *U, float *L)
{
    int i;
    float tmpVal = 0;
    for (i = 0; i < TshapeSize; i++)
        if (Tshape[i] > U[i])
            tmpVal += pow(2, Tshape[i] - U[i]);
        else if (Tshape[i] < L[i])
            tmpVal += pow(2, L[i] - Tshape[i]);
    
    return sqrt(tmpVal);
}