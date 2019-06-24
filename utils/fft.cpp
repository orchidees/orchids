//
//  fft.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "fft.h"


// Create a window of windowLength ones in buffer
void rectwin(unsigned int windowLength, float *buffer)
{
    uninitialized_fill_n(buffer, windowLength, 1);
}

// Create a blackman window of windowLength samples in buffer
void blackman(unsigned int windowLength, float *buffer)
{
    int i;
    for (i = 0; i < windowLength; i++)
        buffer[i] = 0.42 - 0.5 * cos(2.0 * _pi * i / float(windowLength - 1)) + 0.08 * cos(4.0 * _pi * i / float(windowLength - 1));
}

//Perform Short Time Fourier Transform
float STFT(
            float              *signal,
            int                signalLength,
            int                windowSize,
            int                hopSize,
            string             windowType,
            int                fftSize,
            matrixPtr          values)
{
    fftw_complex    *data, *fft_result;
    fftw_plan       plan_forward;
    int             i;
    int             noverlap = windowSize - hopSize;
    // Determine the number of columns of the STFT output (equation used by the spectrogram Matlab function)
    int             ncol = (signalLength - noverlap) / (windowSize - noverlap);
    int             nrow = fftSize / 2;
    
    //values      = matrixPtr(new matrix(nrow, ncol));
    data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    memset(data, 0, sizeof(fftw_complex) * fftSize);
    fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * fftSize );
    memset(fft_result, 0, sizeof(fftw_complex) * fftSize);
    plan_forward = fftw_plan_dft_1d( fftSize, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
    
    // Create a window of appropriate length
    float window[windowSize];
    
    if (windowType == "rectwin")
        rectwin(windowSize, window);
    else if (windowType == "blackman")
        blackman(windowSize, window);
    else
        rectwin(windowSize, window);
    
    int chunkPosition = 0;
    int readIndex;
    
    // Should we stop reading in chunks?
    int bStop = 0;
    int numChunks = 0;
    
    // Process each chunk of the signal
    while(chunkPosition < signalLength && !bStop && numChunks < ncol)
    {
        // Copy the chunk into our buffer
        for(i = 0; i < windowSize; i++) {
            readIndex = chunkPosition + i;
            if(readIndex < signalLength) {
                // Note the windowing!
                data[i][0] = signal[readIndex] * window[i];
                data[i][1] = 0.0;
            } else {
                // we have read beyond the signal, so zero-pad it!
                data[i][0] = 0.0;
                data[i][1] = 0.0;
                bStop = 1;
            }
        }
        
        // Perform the FFT on our chunk
        fftw_execute( plan_forward );
        
        // Uncomment to see the raw-data output from the FFT calculation
        // std::cout << "Column: " << chunkPosition << std::endl;
        // for(i = 0 ; i < windowSize/2 + 1 ; i++ ) {
        //  fprintf( stdout, "fft_result[%d] = { %2.2f, %2.2f }\n",
        //  i, fft_result[i][0], fft_result[i][1] );
        // }
        
        // Copy the first (windowSize/2 + 1) data points into your spectrogram.
        // We do this because the FFT output is mirrored about the nyquist
        // frequency, so the second half of the data is redundant. This is how
        // Matlab's spectrogram routine works.
        for (i = 0; i < fftSize / 2; i++)
            values->operator()(i, numChunks) = absValue<float>(fft_result[i][0], fft_result[i][1]);
        
        chunkPosition += hopSize;
        numChunks++;
    }
    fftw_destroy_plan( plan_forward );
    fftw_free( data );
    fftw_free( fft_result );
    fftw_cleanup();
    
    return getSum(window, windowSize);
}