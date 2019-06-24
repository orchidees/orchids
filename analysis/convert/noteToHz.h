/**
 *
 *  @file			noteToHz.h
 *  @brief			Convert note to frequency
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__noteToHz__
#define __Orchids__noteToHz__

#include "Orchids.h"

/**
 *  @brief Compute the frequency
 *
 *  @param interval
 *  @param octave
 */
float convert_note(
                   int interval,
                   int octave);

/**
 *  @brief Convert note to frequency
 *
 *  @param note Note to convert
 */
float noteToHz(const char *note);

float noteToMidiCent(const char *note);

#endif /* defined(__Orchids__noteToHz__) */
