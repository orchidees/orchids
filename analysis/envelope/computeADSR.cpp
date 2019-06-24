//
//  computeADSR.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "computeADSR.h"


envelope computeADSR(string soundFile, sqlStruct& feat)
{
    float           sRate;
    float*          signal;
    int             signal_size;
    importSignal(soundFile, sRate, signal, signal_size);
    vector<float>   audio(signal, signal + signal_size);
    int             resample_size = signal_size;
    free(signal);
    
    // Resampling EnergyEnvelope
    float* ener = new float[resample_size];
    resampleData(feat.descriptors[0].values->c_data(), feat.descriptors[0].values->size(), ener, resample_size, SRC_SINC_MEDIUM_QUALITY);
    vector<float> energy(ener, ener + resample_size);
    delete ener;

    return computeEnvelope(audio, sRate, energy);
}