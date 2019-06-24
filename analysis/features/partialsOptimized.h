/**
 *
 *  @file			partialsOptimized.h
 *  @brief          Launch the partials analysis
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__partialsOptimized__
#define __Orchids__partialsOptimized__

#include "Orchids.h"

/**
 *  @brief Launch the partials analysis
 *
 *  Launch PM2 analysis and compute the cluster linkage algorithm on partials and means
 *
 *  @param filename Path of the sound file to analyze
 *  @param dir Path of the temporary analysis directory
 *  @param params Parameters structure for PM2 analysis
 *  @param nbSteps Number of steps for computing the partials
 *  @param freqMIPS Output frequencies matrix
 *  @param ampMIPS Output amplitudes matrix
 *  @param freqMEANs Output mean frequencies array
 *  @param ampMEANs Output mean amplitudes array
 */
void partialsOptimized(
                       const char       *filename,
                       const char       *dir,
                       pTrack_params&   params,
                       int              nbSteps,
                       int              fmin,
                       envelope&        boundaries,
                       matrixPtr        freqMIPS,
                       matrixPtr        ampMIPS,
                       float*           &freqMEANs,
                       float*           &ampMEANs);

#endif /* defined(__Orchids__partialsOptimized__) */
