/**
 *
 *  @file			calcN6.h
 *  @brief          Compute loudness and energy of mel bands
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__FcalcN6__
#define __Orchids__FcalcN6__

#include "Orchids.h"

/**
 *  @brief Compute loudness and energy of mel bands
 *
 *  @param mel_s Mel bands to compute
 */
float calcN6(matrixPtr meln_s);

#endif /* defined(__Orchids__FcalcN6__) */
