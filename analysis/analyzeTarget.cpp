//
//  analyzeTarget.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "analyzeTarget.h"


bool analyzeTarget(
                   string           soundfile,
                   analysis_params  params,
                   sqlStruct        &sqlFeatures)
{    
    if (sqlFeatures.note.compare("NA") == 0 && sqlFeatures.dynamics.compare("NA") == 0)
    {
        sqlFeatures.note = "A1";
        sqlFeatures.dynamics = "mf";
        sqlFeatures.name = getFileName<string>(soundfile);
        sqlFeatures.file = soundfile;
    }
    params.pTrack_parameters.meanStart = 2;
    params.pTrack_parameters.meanEnd = 1;
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
            string err = "Error : Convert MP3 to WAV failed on sound " + mp3name;
            throw err;
        }
    }
    
    //Computing duration of the target signal
    float  sr_hz;
    float  *signal;
    int    signal_size;
    
    if (!importSignal(soundfile, sr_hz, signal, signal_size))
        throw "File contains data in an unknown format";
    
    if (isMp3) boost::filesystem::remove(soundfile.c_str());

    sqlFeatures.duration = signal_size / sr_hz;

    if (sr_hz < 11025)
        throw "Error : Sound sample rate is too small.";
    
    string tmpWavSound = "/tmp/analysisTarget.wav";
    if (!wavWrite(signal, signal_size, sr_hz, 32, tmpWavSound.c_str()))
        throw "Error while writing the analysis wav file";

    //Launch the analysis
    try {
        launchSoundAnalysis(tmpWavSound.c_str(), params, sqlFeatures);
    } catch (const char * e) {
        throw e;
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
            int sustain = MIN(floor((float)sqlFeatures.ADSR.decay * (float)sqlFeatures.descriptors[i].values->size() / (float)signal_size), (int)(0.1 *  (float)sqlFeatures.descriptors[i].values->size()));
            int release = MAX(floor((float)sqlFeatures.ADSR.release * (float)sqlFeatures.descriptors[i].values->size() / (float)signal_size), (int)(0.6 * (float)sqlFeatures.descriptors[i].values->size()));
            //Retrieve mean and standard deviation of data only between sustain and release
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
            if (sqlFeatures.descriptors[i].mean == 0 || sqlFeatures.descriptors[i].stdDev)
            {
                sqlFeatures.descriptors[i].mean = getMean(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size());
                sqlFeatures.descriptors[i].stdDev = std_dev(sqlFeatures.descriptors[i].values->begin(), sqlFeatures.descriptors[i].values->size(), sqlFeatures.descriptors[i].mean);
            }
            sqlFeatures.descriptors[i].min = getMin(sqlFeatures.descriptors[i].values->c_data(), sqlFeatures.descriptors[i].values->size());
            sqlFeatures.descriptors[i].max = getMax(sqlFeatures.descriptors[i].values->c_data(), sqlFeatures.descriptors[i].values->size());

            //Compute resample values of data
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
    /*
    float freq      = noteToHz(sqlFeatures.note.c_str());
    float freqmin   = midi2hz(hz2midi(freq) - 11);
    float freqmax   = midi2hz(hz2midi(freq) + 11);
    if (sqlFeatures.descriptors[1].mean < freqmin || sqlFeatures.descriptors[1].mean > freqmax)
        sqlFeatures.descriptors[1].resampled_val->fill(freq);
    */
    
    // Launch partial tracking for interface
    /*
    string output = "/tmp/orchis_ptrack_temp.sdif";
    string command = (string)pm2Path + " -t -S" + tmpWavSound + " -Apar -N" + to_string(params.pTrack_parameters.windowFFTSize) + " -M" + to_string(params.pTrack_parameters.windowAnalysisSize) + " -I" + to_string(params.pTrack_parameters.windowAnalysisStep) + " -W" + params.pTrack_parameters.windowType + " -OS -p1 -mode=0 -q" + to_string(params.pTrack_parameters.nPartials) + " -m" + to_string(params.pTrack_parameters.minAmpAtt) + " -a" + to_string(params.pTrack_parameters.smoothEnvAtt) + " -r" + to_string(params.pTrack_parameters.smoothEnvRel) + " -Ct0.40000001 -Cf0.029302239 -devFR=0.011619449 -devFC=0 -devA=0.5 -devM=1 -devK=3 -L" + to_string(params.pTrack_parameters.minPartialLength) + " " + output;
    if (system(command.c_str()) > 0)
    {
        //boost::filesystem::remove(output);
        throw "AnalyzeTarget::Error during partials tracking computation";
    }
    */

    //boost::filesystem::remove(tmpWavSound);
    return true;
}