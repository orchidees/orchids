//
//  dynamicsToLoudness.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "dynamicsToLoudness.h"

#define dyn_size    11

int dynamicsToLoudness(string dyn_c)
{
    string dynref_c[dyn_size] = {"fff", "ff", "f", "mf", "mp", "p", "pp", "fp", "ppff", "ffpp", "ppmfpp"};
    int ldlref_v[dyn_size] = {33,28,23,18,15,8,3,15,18,16,8};
    int pos = static_cast<int>(find(boost::begin(dynref_c), boost::end(dynref_c), dyn_c) - boost::begin(dynref_c));
    return pos < dyn_size ? ldlref_v[pos] : 15;
}

string loudnessToDynamics(float loud)
{
    int i;
    string dynref_c[8] = {"fff", "ff", "f", "mf", "mp", "p", "pp", "ppp"};
    int ldlref_v[8] = {33,28,23,18,15,8,3,1};
    
    if (loud > 33)
        return dynref_c[0];
    
    float   minDist = 33; //Max distance
    int     pos     = 3;  //Default: the lowest dynamics
    for (i = 0; i < 8; i++)
    {
        float dist = abs(ldlref_v[i] - loud);
        if ( dist < minDist )
        {
            minDist = dist;
            pos = i;
        }
    }
    return dynref_c[pos];
}