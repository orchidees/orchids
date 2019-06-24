//
//  distance.h
//  Orchids
//
//  Created by Antoine Bouchereau on 28/10/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__distance__
#define __Orchids__distance__

#include "Orchids.h"

/*
 * @brief      Computes the row-wise Dynamic Time Warping (DTW) between two matrix
 *
 * For each of the row, we compute the time warping distance and then sum everything.
 * The baseline warping is 4 time points (given that partials are 64 time points).
 *
 */
float          matrixDTW(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the overall Hellinger distance between two matrix
 */
float          matrixHellinger(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the overall Kullback-Leibler distance between two matrix
 */
float          matrixKullbackLeibler(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the overall correlation coefficient distance between two matrix
 */
float          matrixCorrelation(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the Orchidee-based temporal partials distance
 */
float          matrixPartials(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the row-wise Dynamic Time Warping (DTW) between two matrix
 *
 * For each of the row, we compute the time warping distance and then sum everything.
 * The baseline warping is 4 time points (given that partials are 64 time points).
 *
 */
float          vectorDTW(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the overall Hellinger distance between two vectors
 */
float          vectorHellinger(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the overall Kullback-Leibler distance between two vectors
 */
float          vectorKullbackLeibler(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the (inverse) correlation coefficient distance between two vectors
 */
float          vectorCorrelation(matrixPtr p1, matrixPtr p2);
/*
 * @brief      Computes the Orchidee-based partials distance
 */
float          vectorPartials(matrixPtr p1, matrixPtr p2);

#endif /* defined(__Orchids__distance__) */
