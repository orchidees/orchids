//
//  getConfigAnalysisParams.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "getConfigAnalysisParams.h"

int checkConfigParams(analysis_params &params)
{
    //Parameters
    //; Internal sampling rate of the program [11025 22050 44100]
    if (params.irc_desc_params[0].fVal != 11025 && params.irc_desc_params[0].fVal != 22050 && params.irc_desc_params[0].fVal != 44100)
        return 0;
    
    //; Apply normalization to the input file [0 1]
    if (params.irc_desc_params[1].fVal != 0 && params.irc_desc_params[1].fVal != 1)
        return 1;
    
    //; The window applied to every frame [hanning blackman hamming hanning2]
    if (params.irc_desc_params[2].sVal.compare("hanning")
        && params.irc_desc_params[2].sVal.compare("blackman")
        && params.irc_desc_params[2].sVal.compare("hamming")
        && params.irc_desc_params[2].sVal.compare("hanning2"))
        return 2;
    
    //; Saves or not the short time temporal features [0 1]
    if (params.irc_desc_params[3].fVal != 0 && params.irc_desc_params[3].fVal != 1)
        return 3;
    
    //; Enables the DC offset removal frame by frame [0 1]
    if (params.irc_desc_params[4].fVal != 0 && params.irc_desc_params[4].fVal != 1)
        return 4;
    
    //; Max lag to compute the autocorrelation [1-N]
    if ((int)params.irc_desc_params[5].fVal < 1)
        return 5;
    
    //; Number of frequency bands used for Flatness and Crest [1-4]
    if (!IsInBounds((int)params.irc_desc_params[6].fVal, 1, 4))
        return 6;
    
    //; Number of Mel Bands [10-24]
    if (!IsInBounds((int)params.irc_desc_params[7].fVal, 10, 24))
        return 7;
    
    //; Number of MFCCs [1-N]
    if ((int)params.irc_desc_params[8].fVal < 1)
        return 8;
    
    //; Max number of harmonics for harmonic analysis [1-N]
    if ((int)params.irc_desc_params[9].fVal < 1)
        return 9;
    
    //; Cutoff frequency for F0 estimation [1 - ResamplingTo/2]
    if (!IsInBounds((int)params.irc_desc_params[10].fVal, 1, (int)params.irc_desc_params[0].fVal / 2))
        return 10;
    
    //; Minimum detected F0 frequency [1 - ResamplingTo/2]
    if (!IsInBounds((int)params.irc_desc_params[11].fVal, 1, (int)params.irc_desc_params[0].fVal / 2))
        return 11;
    
    //; Maximum detected F0 frequency [F0MinFrequency - ResamplingTo/2]
    if (!IsInBounds((int)params.irc_desc_params[12].fVal, (int)params.irc_desc_params[11].fVal, (int)params.irc_desc_params[0].fVal / 2))
        return 12;
    
    //; Thresholding of the spectrum in F0 detection [1-N]
    if ((int)params.irc_desc_params[13].fVal < 1)
        return 13;
    
    //; Trigger the computation of the F0 modulation descriptor [0 1]
    if (params.irc_desc_params[14].fVal != 0 && params.irc_desc_params[14].fVal != 1)
        return 14;
    
    //; The percentage of energy used by the rolloff descriptors [0.0-1.0]
    if (!IsInBounds(params.irc_desc_params[15].fVal, 0.0f, 1.0f))
        return 15;
    
    //; Max number of bands to use in the deviation [1-Harmonics/MFCC]
    if ((int)params.irc_desc_params[16].fVal < 1)
        return 16;
    
    //; Percentage of the maximum value of the loudness (or energy) [0.0-1.0]
    if (!IsInBounds(params.irc_desc_params[17].fVal, 0.0f, 1.0f))
        return 17;
    
    //; Percentage of the maximum value of the loudness (or energy) [0.0-1.0]
    if (!IsInBounds(params.irc_desc_params[18].fVal, 0.0f, 1.0f))
        return 18;
    
    //; The minimum F0 for chroma [1-ChromaFreqMax]
    if (!IsInBounds(params.irc_desc_params[19].fVal, 1.0f, params.irc_desc_params[20].fVal))
        return 19;
    
    //; Maximum F0 for Chroma [ChromaFreqMin-ResampleTo/2]
    if (!IsInBounds(params.irc_desc_params[20].fVal, params.irc_desc_params[19].fVal, params.irc_desc_params[0].fVal / 2))
        return 20;
    
    //; The resolution of Chroma in semitones [0.0001 - 12]
    if (!IsInBounds(params.irc_desc_params[21].fVal, 0.0001f, 12.0f))
        return 21;
    
    //; Normalize or not the chroma result [0 1]
    if (params.irc_desc_params[22].fVal != 0 && params.irc_desc_params[22].fVal != 1)
        return 22;
    
    //; Size of the median filter [1-N (odd)]
    if ((int)params.irc_desc_params[23].fVal < 1 && (int)params.irc_desc_params[23].fVal % 2 != 1)
        return 23;
    
    //; Normalization of the median filter [0 1]
    if (params.irc_desc_params[24].fVal != 0 && params.irc_desc_params[24].fVal != 1)
        return 24;
    
    //; Triggers the computation of attack, decrease ... based on Loudness [0 1]
    if (params.irc_desc_params[25].fVal != 0 && params.irc_desc_params[25].fVal != 1)
        return 25;
    
    //Standard
    //; Size of analysis window [seconds : 2/resampleTo - duration(signal)]
    if (params.irc_desc_params[26].fVal < 2 / params.irc_desc_params[0].fVal)
        return 26;
    
    //; Size of analysis step [2/resampleTo - windowSize]
    if (!IsInBounds(params.irc_desc_params[27].fVal, 2 / params.irc_desc_params[0].fVal, params.irc_desc_params[26].fVal))
        return 27;

    //; Compute temporal modelings every N frame, not on whole file [1-N]
    if ((int)params.irc_desc_params[28].fVal < 1 && (int)params.irc_desc_params[28].fVal != -1)
        return 28;

    //; Step size for texture windows in number of short time descriptors [1-N]
    if ((int)params.irc_desc_params[29].fVal < 1 && (int)params.irc_desc_params[29].fVal != -1)
        return 29;

    //Energy
    //; Size of analysis window [seconds : 2/resampleTo - duration(signal)]
    if (params.irc_desc_params[30].fVal < 2 / params.irc_desc_params[0].fVal)
        return 30;

    //; Size of analysis step [2/resampleTo - windowSize]
    if (!IsInBounds(params.irc_desc_params[31].fVal, 2 / params.irc_desc_params[0].fVal, params.irc_desc_params[26].fVal))
        return 31;
    
    //; Compute temporal modelings every N frame, not on whole file [1-N]
    if ((int)params.irc_desc_params[32].fVal < 1 && (int)params.irc_desc_params[32].fVal != -1)
        return 32;
    
    //; Step size for texture windows in number of short time descriptors [1-N]
    if ((int)params.irc_desc_params[33].fVal < 1 && (int)params.irc_desc_params[33].fVal != -1)
        return 33;

    //; Compute following temporal descriptors [0 1]
    if (params.irc_desc_params[34].fVal != 0 && params.irc_desc_params[34].fVal != 1)
        return 34;
    if (params.irc_desc_params[35].fVal != 0 && params.irc_desc_params[35].fVal != 1)
        return 35;
    if (params.irc_desc_params[36].fVal != 0 && params.irc_desc_params[36].fVal != 1)
        return 36;
    if (params.irc_desc_params[37].fVal != 0 && params.irc_desc_params[37].fVal != 1)
        return 37;
    if (params.irc_desc_params[38].fVal != 0 && params.irc_desc_params[38].fVal != 1)
        return 38;
    if (params.irc_desc_params[39].fVal != 0 && params.irc_desc_params[39].fVal != 1)
        return 39;
    if (params.irc_desc_params[40].fVal != 0 && params.irc_desc_params[40].fVal != 1)
        return 40;
    
    return -1;
}

void getConfigAnalysisParams(analysis_params &params, string configFile)
{
    int i;
    ConfigFile cfg(configFile);
    string name;
    
    for (i = 0; i < params.irc_desc_params.size(); i++)
    {
        name = params.irc_desc_params[i].name;
        
        if (name.compare("WindowSize") == 0
            || name.compare("HopSize") == 0
            || name.compare("TextureWindowsFrames") == 0
            || name.compare("TextureWindowsHopFrames") == 0)
        {
            name = params.irc_desc_params[i].type + name;
        }

        if (cfg.keyExists(name))
        {
            if (name.compare("WindowType") == 0)
                params.irc_desc_params[i].sVal = cfg.getValueOfKey<string>(name);
            else
                params.irc_desc_params[i].fVal = cfg.getValueOfKey<float>(name);
        }
    }
    
    if (cfg.keyExists("windowFFTSize"))
        params.pTrack_parameters.windowFFTSize      = cfg.getValueOfKey<int>("windowFFTSize");
    if (cfg.keyExists("windowAnalysisSize"))
        params.pTrack_parameters.windowAnalysisSize = cfg.getValueOfKey<int>("windowAnalysisSize");
    if (cfg.keyExists("windowAnalysisStep"))
        params.pTrack_parameters.windowAnalysisStep = cfg.getValueOfKey<int>("windowAnalysisStep");
    if (cfg.keyExists("windowType"))
        params.pTrack_parameters.windowType         = cfg.getValueOfKey<string>("windowTypePM2");
    if (cfg.keyExists("nPartials"))
        params.pTrack_parameters.nPartials          = cfg.getValueOfKey<int>("nPartials");
    if (cfg.keyExists("minAmpAtt"))
        params.pTrack_parameters.minAmpAtt          = cfg.getValueOfKey<int>("minAmpAtt");
    //if (cfg.keyExists("smoothEnvAtt"))
        params.pTrack_parameters.smoothEnvAtt       = 0;
    //if (cfg.keyExists("smoothEnvRel"))
        params.pTrack_parameters.smoothEnvRel       = 0;
    if (cfg.keyExists("minPartialLength"))
        params.pTrack_parameters.minPartialLength   = cfg.getValueOfKey<float>("minPartialLength");
}