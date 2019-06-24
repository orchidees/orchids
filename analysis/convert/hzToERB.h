/**
 *
 *  @file			hzToERB.h
 *  @brief			Convert frequency to ERB
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__hzToERB__
#define __Orchids__hzToERB__

#include "Orchids.h"

/**
 *  @brief Convert frequency to ERB
 *
 *  @param f Frequency to convert
 *  @param formula Formula used for converting: 'glasberg90' or 'moore83'
 */
float hzToERB(
              float f,
              string formula);

#endif /* defined(__Orchids__hzToERB__) */
