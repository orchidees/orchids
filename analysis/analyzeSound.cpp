//
//  analyzeSound.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "analyzeSound.h"


bool analyzeSound(
                  string           soundfile,
                  analysis_params  params,
                  sqlStruct        &sqlFeatures,
                  string           source)
{
    extractSymbolicInfos(soundfile, sqlFeatures, source);
    
    //Check if mp3, then convert to wav
    bool isMp3 = false;
    if ( soundfile.substr(soundfile.find_last_of(".") + 1) == "mp3"
        || soundfile.substr(soundfile.find_last_of(".") + 1) == "MP3" )
    {
        isMp3 = true;
        string mp3name = soundfile;
        soundfile = soundfile.substr(0, soundfile.size() - 3);
        soundfile += "wav";
        if (convertMp3ToWav(mp3name.c_str(), soundfile.c_str()) != 0)
        {
            printf("Error : Convert MP3 to WAV failed on sound %s", mp3name.c_str());
            return false;
        }
    }

    //Computing duration of the target signal
    float  sr_hz;
    float  *signal;
    int    signal_size;
    
    if (!importSignal(soundfile, sr_hz, signal, signal_size))
    {
        cout << "File contains data in an unknown format." << endl;
        return false;
    }
    
    if (isMp3) boost::filesystem::remove(soundfile.c_str());
    
    sqlFeatures.duration = signal_size / sr_hz;

    if (sr_hz < 11025)
    {
        cout << "Error : Sound sample rate is too small." << endl;
        return false;
    }
    
    // Create an UUID as a temporary soundfile name
    string tmpWavSound = "/tmp/analysisFile.wav"; //"/tmp/" + boost::lexical_cast<std::string>(boost::uuids::random_generator()()) + ".wav";
    if (!wavWrite(signal, signal_size, sr_hz, 32, tmpWavSound.c_str()))
    {
        cout << "Error while writing the analysis wav file" << endl;
        return false;
    }
    
    //Launch the analysis
    try {
        launchSoundAnalysis(tmpWavSound.c_str(), params, sqlFeatures);
    } catch (const char * e) {
        cout << e << endl;
        return false;
    }

    //Number of analysis steps used for resampling
    int             i, j, nbSteps = 128;
    cout << "Resampling features..." << endl;
    for (i = 0; i < sqlFeatures.descriptors.size(); i++)
    {
        if (sqlFeatures.descriptors[i].values->cols() > 1)
            sqlFeatures.descriptors[i].values->transpose();
        else if (sqlFeatures.descriptors[i].values->rows() > 1)
        {
            //printf("%d - %d - %d - %d\n", sqlFeatures.ADSR.attack, sqlFeatures.ADSR.decay, sqlFeatures.ADSR.sustain, sqlFeatures.ADSR.release);
            int sustain = floor(0.05 * (float)sqlFeatures.descriptors[i].values->size());
            int release = floor(0.3 * (float)sqlFeatures.descriptors[i].values->size());
            //cout << "**" << endl << "**" << sustain << endl << release << "**" << endl << "**" << endl;
            //int sustain = (int)(((0.05 * sr_hz) / ((float)signal_size)) * (float)sqlFeatures.descriptors[i].values->size());
            //int release = MIN((int)(((0.8 * sr_hz) / ((float)signal_size)) * (float)sqlFeatures.descriptors[i].values->size()), sqlFeatures.descriptors[i].values->size());
            //int sustain = (int)(0.05 * (float)sqlFeatures.descriptors[i].values->size());
            //int release = (int)(0.3 * (float)sqlFeatures.descriptors[i].values->size());
            //int sustain = 0;
            //int release = sqlFeatures.descriptors[i].values->size();
            if (release <= sustain)
            {
                sqlFeatures.descriptors[i].mean = getMean(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size());
                sqlFeatures.descriptors[i].stdDev = std_dev(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size(), sqlFeatures.descriptors[i].mean);
            }
            else
            {
                //Retrieve mean and standard deviation of data only between sustain and release
                sqlFeatures.descriptors[i].mean = getMean(sqlFeatures.descriptors[i].values->begin() + sustain, release - sustain + 1);
                sqlFeatures.descriptors[i].stdDev = std_dev(sqlFeatures.descriptors[i].values->begin() + sustain, release - sustain + 1, sqlFeatures.descriptors[i].mean);
            }
            if (sqlFeatures.descriptors[i].mean == 0)
            {
                sqlFeatures.descriptors[i].mean = getMean(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size());
                sqlFeatures.descriptors[i].stdDev = std_dev(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size(), sqlFeatures.descriptors[i].mean);
            }
            
            //Compute resample values
            float *data = (float *)malloc(nbSteps * sizeof(float));
            resampleData(sqlFeatures.descriptors[i].values->c_data(), sqlFeatures.descriptors[i].values->size(), data, nbSteps, SRC_SINC_BEST_QUALITY);
            matrixPtr resampled_val(new matrix(data, true, nbSteps, 1));
            resampled_val->normalize();
            sqlFeatures.descriptors[i].resampled_val = resampled_val;
            
            // Check for NaN or inf values
            for (j = 0; j < sqlFeatures.descriptors[i].resampled_val->size(); j++)
                if (isinf(sqlFeatures.descriptors[i].resampled_val->operator()(j) || isnan(sqlFeatures.descriptors[i].resampled_val->operator()(j))))
                    sqlFeatures.descriptors[i].resampled_val->operator()(j) = 0;
        }
    }
    free(signal);
    
    //If no dynamics, take Loudness (descriptor n°4) mean (computed with ADSR)
    if (sqlFeatures.dynamics == "NA")
        sqlFeatures.dynamics = sqlFeatures.descriptors[4].mean;
    
    // Check Fundamental Frequency (descriptor n°1)
    float freq      = noteToHz(sqlFeatures.note.c_str());
    float freqmin   = midi2hz(hz2midi(freq) - 11);
    float freqmax   = midi2hz(hz2midi(freq) + 11);
    if (sqlFeatures.descriptors[1].mean < freqmin || sqlFeatures.descriptors[1].mean > freqmax)
        sqlFeatures.descriptors[1].resampled_val->fill(freq);
    
    //boost::filesystem::remove(tmpWavSound);
    return true;
}