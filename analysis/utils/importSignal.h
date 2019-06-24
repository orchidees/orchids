/**
 *
 *  @file			importSignal.h
 *  @brief			Operations on sound files
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__importSignal__
#define __Orchids__importSignal__

#include "Orchids.h"

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */
struct buffer {
    unsigned char const *start;
    unsigned long length;
};

/**
 *  @brief Import a sound file.
 *
 *  Import a sound file into an array of float.
 *  If multichannels, the mean is computed.
 *
 *  @param fname Path of the sound file to analyze
 *  @param sr_hz Sample rate
 *  @param signal Array of signal datas
 *  @param signal_size Array size
 */
bool importSignal(
                  string        fname,
                  float         &sr_hz,
                  float*        &signal,
                  int           &signal_size);

/**
 *  @brief Convert a mp3 file to a wav file
 *
 *  @param input Input mp3 sound file name
 *  @param output Output wav sound file name
 */
int convertMp3ToWav(const char* input, const char* output);

/**
 *  @brief Write a wav file from an array of datas
 *
 *  @param data Array of signal datas
 *  @param size Array size
 *  @param sampleRate Sample rate
 *  @param outfilename Name of the output wav file
 */
bool wavWrite(
              float         *data,
              int           size,
              int           sampleRate,
              int           bytes,
              const char    *outfilename);

#endif /* defined(__Orchids__importSignal__) */
