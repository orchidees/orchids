//
//  density_PADE.h
//  Orchids
//
//  Created by Antoine Bouchereau on 02/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__density_PADE__
#define __Orchids__density_PADE__

#include "Orchids.h"

/**
 *  @brief Count the occurences of all values between 1 and 'maxValue' in each line of a matrix. If A is the input NxP matrix, countAllValues(A,M) returns a NxM matrixPtr B where B(k,l) is the number of occurences of l in A(k,:)
 */
void countAllValues(vector<int>& K, vector<int>& density, const int maxCount);

/**
 *  @brief Population-Adaptive Density Estimation. Given a input population C (as a set of vectors in the criteria space), output a 1xN density vector where N is the number of cells in the adaptive grid and each element is the number of individuals in the associated cell. Also ouput a cell_indices vector which map each individual to a cell index. Given a vector of criteria C(i), the local density at that point of the criteria space is density(cell_indices(i))
 */
void density_PADE(
                  matrixPtr        C,
                  vector<int>&  density,
                  vector<int>&  cell_indices);

#endif /* defined(__Orchids__density_PADE__) */
