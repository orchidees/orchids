/**
 *
 *  @file			dynamicsToLoudness.h
 *  @brief			Convert dynamics to loudness
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__dynamicsToLoudness__
#define __Orchids__dynamicsToLoudness__

#include "Orchids.h"

/**
 *  @brief Convert dynamics to loudness
 *
 *  @param dyn_c Dynamic to convert
 */
int dynamicsToLoudness(string dyn_c);

/**
 *  @brief Convert loudness to dynamics
 *
 *  @param loud Loudness to convert
 */
string loudnessToDynamics(float loud);


#endif /* defined(__Orchids__dynamicsToLoudness__) */
