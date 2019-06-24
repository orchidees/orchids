//
//  fitness_advance.h
//  Orchids
//
//  Created by Philippe Esling on 01/11/2014.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__fitness_advance__
#define __Orchids__fitness_advance__

#include "Orchids.h"

float                   fitness_advance(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights);
vector<float>           compute_mutation_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights);
vector<float>           compute_cross_probas(vector<float> &idealCriteria, vector<float> &nadirCriteria, matrixPtr newPopCriteria, float* weights);

#endif /* defined(__Orchids__fitness_advance__) */
