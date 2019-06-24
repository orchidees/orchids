/**
 *
 *  @file			melBands.h
 *  @brief          Compute mel bands
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__melBands__
#define __Orchids__melBands__

#include "Orchids.h"

/**
 *  @brief Launch the fft of the analysis sound and compute mel bands
 *
 *  @param snd_v Input signal to compute
 *  @param snd_vSize Signal size
 *  @param sr_hz Sample rate
 *  @param filtre_s Mel filter
 */
matrixPtr melBandsSoundFix(
                           float*    &snd_v,
                           int       &snd_vSize,
                           float     sr_hz,
                           crs*      &filtre_m);

/**
 *  @brief Retrieve mel bands after computation and fill the base structure
 *
 *  @param meln_s Mel amplitude spectrum
 */
void melBandsStruct(
                    matrixPtr   meln_s,
                    sqlStruct&  features,
                    int         signalSize);

/**
 *  @brief Determine matrixPtr for a mel-spaced filterbank
 *
 *  @param p Number of filters in filterbank
 *  @param n Length of fft
 *  @param fs Sample rate in Hz
 *  @param fl Low end of the lowest filter as a fraction of fs (default = 0)
 *  @param fh High end of highest filter as a fraction of fs (default = 0.5)
 *  @param w Any sensible combination of the following:
 *		      't'  triangular shaped filters in mel domain (default)
 *		      'n'  hanning shaped filters in mel domain
 *		      'm'  hamming shaped filters in mel domain
 *
 *		      'z'  highest and lowest filters taper down to zero (default)
 *		      'y'  lowest filter remains at 1 down to 0 frequency and
 *			   highest filter remains at 1 up to nyquist freqency
 *
 *		       If 'ty' or 'ny' is specified, the total power in the fft is preserved.
 *  @param x Sparse matrixPtr containing the filterbank amplitudes
 */
void melbankm(
              float         p,
              float         n,
              float         fs,
              float         fl,
              float         fh,
              const char    *w,
              crs*          &x);


#endif /* defined(__Orchids__melBands__) */
