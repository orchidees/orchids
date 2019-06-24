/**
 *
 *  @file			chordSeqOptimized.h
 *  @brief			Launch the PM2 analysis
 *  @version        0.05
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__FChordSeqOptimized__
#define __Orchids__FChordSeqOptimized__

#include "Orchids.h"

/**
 *  @brief Compute the mean of a matrix
 *
 *  @param amp_m matrixPtr of partials amplitude we want to compute
 *  @param amp1_v Array of means to fill
 */
int partialsMeanStd(
                    matrixPtr amp_m,
                    float*    &amp1_v);


int partialsMeanStd(
                    matrixPtr amp_m,
                    float*    &amp1_v,
                    int maxR);

/**
 *  @brief Compute the median of a matrix
 *
 *  @param freq_m matrixPtr of partials frequencies we want to compute
 *  @param freq1_v Array of medians to fill
 */
int partialsMedianStd(
                             matrixPtr  freq_m,
                             float*     &freq1_v);

/**
 *  @brief Launch the PM2 analysis
 *
 *  Launch the PM2 analysis of the analysis file.
 *  Read the SDIF file and parse datas.
 *  Retrieve partials frequencies and amplitudes.
 *
 *  @param soundfile Path of the sound file to analyze
 *  @param analyzedir Path of the temporary analysis directory
 *  @param sr_hz Sample rate of the analysis file
 *  @param minf0 Minimal frequency for the partials analysis
 *  @param numpartialsin Number of partials to calculate
 *  @param debut Beginning time
 *  @param fin Ending time
 *  @param nbSteps Number of steps for computing the partials
 *  @param freqo_F matrixPtr of partials frequency to fill
 *  @param ampo_F matrixPtr of partials amplitude to fill
 */
void chordSeqOptimized2(
                       const char      *soundfile,
                       string          analysedir,
                       float           sr_hz,
                       int             f0min,
                       //int             numpartialsin,
                       float           debut,
                       float           fin,
                       float           nbSteps,
                       pTrack_params&  pm2,
                       matrixPtr       freqo_F,
                       matrixPtr       ampo_F);


#endif /* defined(__Orchids__FChordSeqOptimized__) */
