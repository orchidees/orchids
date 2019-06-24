//
//  hzToERB.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

//Use:
//      for (i = 0; i<vector.size(); i++) {
//          vector[i] = hzToERB(vector[i], string formula);
//      }

#include "hzToERB.h"

float hzToERB(float f, string formula)
{
    if (formula.empty())
        formula = "glasberg90";
    
    float e = 0;
    if (formula.compare("glasberg90")==0) {
        e = 9.26*logf(0.00437*f + 1);
    }else if (formula.compare("moore83")==0){
        float erb_k1 = 11.17;
        float erb_k2 = 0.312;
        float erb_k3 = 14.675;
        float erb_k4 = 43;
        f = f/1000;
        e = erb_k1 * log((f + erb_k2) / (f + erb_k3)) + erb_k4;
    }else{
        cout << "Analysis::Unexpected formula" << endl;
    }
    return e;
}