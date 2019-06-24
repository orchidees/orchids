//
//  launchSoundAnalysis.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "launchSoundAnalysis.h"
#include "convert/noteToHz.h"

//Retrieve the descriptors from IRCAMDESCRIPTOR and partials from PM2 and store them in the target structure
void launchSoundAnalysis(
                         const char         *soundFile,
                         analysis_params    params,
                         sqlStruct          &sqlStruct)
{
    //Import signal from filename
    float  sr_hz;
    float  *data_v;
    int    data_v_size;
    if (!importSignal(soundFile, sr_hz, data_v, data_v_size))
        throw "ImportSignal::File contains data in an unknown format.";
    
    cout << "Anaysis" << endl;
    cout << "Analysis: IRCAMDescriptor" << endl;
    
    try {
        if (data_v_size < 8820)
        {
            const char *tmpFile = "/tmp/shorterSoundIDesc.wav";
            printf("Padding sound (%d samples) with %d samples to fit minimal window.\n", data_v_size, 16384 - data_v_size);
            padarray<float>(data_v, data_v_size, 8820 - data_v_size, 0, "post");
            wavWrite(data_v, 8820, sr_hz, 32, tmpFile);
            sqlStruct.descriptors = ircamDescriptors(tmpFile, params.irc_desc_params);
        }
        else
            sqlStruct.descriptors = ircamDescriptors(soundFile, params.irc_desc_params);
    } catch (const char * e) {
        throw e;
    }
    
    cout << "Computing ADSR..." << endl;
    sqlStruct.ADSR = computeADSR(soundFile, sqlStruct);
    
    cout << "Analysis : Main partials analysis" << endl;
    
    //NbSteps is NWindows of the descriptor analysis by ircamdescriptor
    int         nbSteps = 64;
    matrixPtr   partF(new matrix(25, nbSteps));
    matrixPtr   partA(new matrix(25, nbSteps));
    float       *partFM, *partAM;
    try {
        int fmin = round(noteToHz(sqlStruct.note.c_str()) * .9);
        partialsOptimized(soundFile, "/tmp/", params.pTrack_parameters, nbSteps, fmin, sqlStruct.ADSR, partF, partA, partFM, partAM);
    } catch (const char * e) {
        throw e;
    }

    sqlStruct.partials.partialsFrequency            = partF;
    sqlStruct.partials.partialsAmplitude            = partA;
    sqlStruct.partials.partialsMeanFrequency        = partFM;
    sqlStruct.partials.partialsMeanFrequencySize    = partF->rows();
    sqlStruct.partials.partialsMeanAmplitude        = partAM;
    sqlStruct.partials.partialsMeanAmplitudeSize    = partA->rows();
    
    cout << "Analysis : Mel band analysis" << endl;
    crs     *filtre_m = nullptr; //nullptr SUPER IMPORTANT!!!
    matrixPtr meln_s = melBandsSoundFix(data_v, data_v_size, sr_hz, filtre_m);
    crs_destroy(filtre_m);
    melBandsStruct(meln_s, sqlStruct, data_v_size);
    
    //If multiphonic sound or no note was found, keep FundamentalFrequencyMean
    size_t p = sqlStruct.note.find(",");
    if (p != string::npos || sqlStruct.note == "NA")
        //Note = mean of Fundamental Frequency
        sqlStruct.note = hzToNote( getMean(sqlStruct.descriptors[1].values->c_data(), sqlStruct.descriptors[1].values->size()) );
    
    cout << "End Analysis" << endl;
    free(data_v);
}