/**
 *
 *  @file			fft.h
 *  @brief          Implementation of the fftw3 library
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__fft__
#define __Orchids__fft__

#include "Orchids.h"

/**
 *  @brief Create a rectangular window of length windowLength
 */
void    rectwin(
                unsigned int    windowLength,
                float           *buffer);

/**
 *  @brief Create a blackman window of length windowLength
 */
void    blackman(
                 unsigned int   windowLength,
                 float          *buffer);

/**
 *  @brief Launch Short Time Fourier Transform
 *
 *  @param signal Input signal
 *  @param signalLength Input signal size
 *  @param windowSize Length of the window
 *  @param hopSize Length of interval
 *  @param windowType Window type: 'rectwin' or 'blackman'
 *  @param fftSize Length of the fft
 *  @param values Output matrixPtr to fill with STFT results
 */
float  STFT(
             float      *signal,
             int        signalLength,
             int        windowSize,
             int        hopSize,
             string     windowType,
             int        fftSize,
             matrixPtr  values);


#endif
