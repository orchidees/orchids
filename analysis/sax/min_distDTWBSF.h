/**
 *
 *  @file			min_distDTWBSF.h
 *  @brief			This function computes the minimum (lower-bounding) distance between two time series
 *  @author			Antoine Bouchereau
 *  @version        0.06
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__min_distDTWBSF__
#define __Orchids__min_distDTWBSF__

#include "Orchids.h"

/**
 *  @brief This function computes the minimum (lower-bounding) distance between two time series
 *  @param Tshape Time serie to analyze
 *  @param TshapeSize Size of the time serie
 *  @param U Upper bound
 *  @param L Lower bound
 */
float min_distDTWBSF(float *Tshape, int TshapeSize, float *U, float *L);

#endif