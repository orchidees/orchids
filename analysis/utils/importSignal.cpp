//
//  importSignal.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "importSignal.h"


bool importSignal(
                  string        fname,
                  float         &sr_hz,
                  float*        &signal,
                  int           &signal_size)
{
    SndfileHandle myf   = SndfileHandle(fname);
    int channels        = myf.channels();
    int frames          = (int)myf.frames();
    sr_hz               = myf.samplerate();
    
    if (myf.error() == 0)
    {
        // Channel data
        uint bufferSize = frames * channels;
        signal_size = bufferSize / channels;
        // Initialize our read buffer
        //float *readbuffer = new float[bufferSize];
        float *readbuffer = (float *)malloc(bufferSize * sizeof(float));
        signal  = (float *)malloc(signal_size * sizeof(float));
        
        int readcount;
        int i, j;
        int readpointer = 0;
        
        if (channels > 1) {
            // DSPAudioBuffer converts all multichannel files to mono by averaging the channels
            // This is probably not an optimal way to convert to mono
            float monoAverage;
            
            readcount = (int)myf.read(readbuffer, bufferSize);
            readpointer = 0;
            for (i = 0; i < signal_size; i++) {
                // for each frame...
                monoAverage = 0;
                for(j = 0; j < channels; j++)
                {
                    monoAverage += readbuffer[readpointer + j];
                }
                monoAverage /= channels;
                readpointer += channels;
                // add the averaged sample to our vector of samples
                signal[i] = monoAverage;
            }
        }else
            myf.read(signal, myf.frames());
        
        free(readbuffer);
        return true;
        
    }else{
        printf ("    Error       : %s : %s\n", myf.strError(), fname.c_str());
        return false;
    }
}

void cleanup(mpg123_handle *mh)
{
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
}

int convertMp3ToWav(const char *input, const char *output)
{
    SNDFILE* sndfile = NULL;
	SF_INFO sfinfo;
	mpg123_handle *mh = NULL;
	unsigned char* buffer = NULL;
	size_t buffer_size = 0;
	size_t done = 0;
	int  channels = 0, encoding = 0;
	long rate = 0;
	int  err  = MPG123_OK;
	off_t samples = 0;
    
	printf( "Input file: %s\n", input);
	printf( "Output file: %s\n", output);
	
	err = mpg123_init();
	if(err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL)
	{
		fprintf(stderr, "Basic setup goes wrong: %s", mpg123_plain_strerror(err));
		cleanup(mh);
		return -1;
	}
    
	/* Simple hack to enable floating point output. */
	mpg123_param(mh, MPG123_ADD_FLAGS, MPG123_FORCE_FLOAT, 0.);
    
    /* Let mpg123 work with the file, that excludes MPG123_NEED_MORE messages. */
	if(    mpg123_open(mh, input) != MPG123_OK
       /* Peek into track and get first output format. */
       || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK )
	{
		fprintf( stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh) );
		cleanup(mh);
		return -1;
	}
    
	if(encoding != MPG123_ENC_SIGNED_16 && encoding != MPG123_ENC_FLOAT_32)
	{ /* Signed 16 is the default output format anyways; it would actually by only different if we forced it.
       So this check is here just for this explanation. */
		cleanup(mh);
		fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
		return -2;
	}
	/* Ensure that this output format will not change (it could, when we allow it). */
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);
    
	bzero(&sfinfo, sizeof(sfinfo) );
	sfinfo.samplerate = (int)rate;
	sfinfo.channels = channels;
	sfinfo.format = SF_FORMAT_WAV|(encoding == MPG123_ENC_SIGNED_16 ? SF_FORMAT_PCM_16 : SF_FORMAT_FLOAT);
	printf("Creating WAV with %i channels and %liHz.\n", channels, rate);
    
	sndfile = sf_open(output, SFM_WRITE, &sfinfo);
	if(sndfile == NULL){ fprintf(stderr, "Cannot open output file!\n"); cleanup(mh); return -2; }
    
	/* Buffer could be almost any size here, mpg123_outblock() is just some recommendation.
     Important, especially for sndfile writing, is that the size is a multiple of sample size. */
	buffer_size = mpg123_outblock(mh);
	buffer = (unsigned char*)malloc( buffer_size );
    
	do
	{
		sf_count_t more_samples;
		err = mpg123_read( mh, buffer, buffer_size, &done );
		more_samples = encoding == MPG123_ENC_SIGNED_16
        ? sf_write_short(sndfile, (short*)buffer, done/sizeof(short))
        : sf_write_float(sndfile, (float*)buffer, done/sizeof(float));
		if(more_samples < 0 || more_samples*mpg123_encsize(encoding) != done)
		{
			fprintf(stderr, "Warning: Written number of samples does not match the byte count we got from libmpg123: %li != %li\n", (long)(more_samples*mpg123_encsize(encoding)), (long)done);
		}
		samples += more_samples;
		/* We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
         We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK. */
	} while (err==MPG123_OK);
    
	if(err != MPG123_DONE)
        fprintf( stderr, "Warning: Decoding ended prematurely because: %s\n",
                err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err) );
    
	sf_close( sndfile );
    
	samples /= channels;
	printf("%li samples written.\n", (long)samples);
	cleanup(mh);
	return 0;
}

bool wavWrite(
              float         *data,
              int           size,
              int           sampleRate,
              int           bytes,
              const char    *outfilename)
{
    int format;
    switch (bytes)
    {
        case 16:
            format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
            break;
            
        case 24:
            format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
            break;
            
        case 32:
            format = SF_FORMAT_WAV | SF_FORMAT_PCM_32;
            break;
            
        default:
            format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
            break;
    }
    const int channels  = 1;
    
    SndfileHandle outfile(outfilename, SFM_WRITE, format, channels, sampleRate);
    if (not outfile) return false;

    outfile.write(data, size);
    return true;
}