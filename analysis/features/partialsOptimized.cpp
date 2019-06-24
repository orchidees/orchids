//
//  partialsOptimized.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "partialsOptimized.h"


void partialsOptimized(
                       const char       *filename,
                       const char       *dir,
                       pTrack_params&   params,
                       int              nbSteps,
                       int              fmin,
                       envelope&        boundaries,
                       matrixPtr        freqMIPS,
                       matrixPtr        ampMIPS,
                       float*           &freqMEANs,
                       float*           &ampMEANs)
{
    //int       nbSteps = 64;
    int         i, j, k, l;
    float       sr_hz;
    float       *tmp_v;
    int         signalSize;
    
    if (!importSignal(filename, sr_hz, tmp_v, signalSize))
        return;
    free(tmp_v);
    
    float       t_debut     = 0.0;
    float       t_fin       = signalSize / sr_hz;
    
    params.nPartials = 75;
    matrixPtr freq_v(new matrix(params.nPartials, nbSteps));
    matrixPtr amp_v(new matrix(params.nPartials, nbSteps));
    try {
        chordSeqOptimized2(filename, dir, sr_hz, fmin, t_debut, t_fin, nbSteps, params, freq_v, amp_v);
    } catch (const char * e) {
        throw e;
    }
    
    for (i = 0; i < nbSteps; i++)
    {
        float *F_MIPs, *A_MIPs;
        vector<int> poso_v;
        mips2(freq_v->getCol(i), freq_v->rows(), amp_v->getCol(i), amp_v->rows(), 25, fmin, F_MIPs, A_MIPs, poso_v);
        for (j = 0; j < poso_v.size(); j++)
        {
            freqMIPS->operator()(j, i)  = F_MIPs[j];
            ampMIPS->operator()(j, i)   = A_MIPs[j];
        }
        if (!poso_v.empty())
        {
            free(F_MIPs);
            free(A_MIPs);
        }
    }
    
    // NOW DO IT FOR THE MEAN PARTIALS
//    params.nPartials = 100;
    freq_v = matrixPtr(new matrix(params.nPartials, 1));
    amp_v = matrixPtr(new matrix(params.nPartials, 1));
    try {
        chordSeqOptimized2(filename, dir, sr_hz, fmin, t_debut, t_fin, 1, params, freq_v, amp_v);
    } catch (const char * e) {
        throw e;
    }
    vector<int> poso_v;
    mips2(freq_v->getCol(0), freq_v->rows(), amp_v->getCol(0), amp_v->rows(), 25, fmin, freqMEANs, ampMEANs, poso_v);
    if (poso_v.size() == 0)
    {
        freqMEANs   = (float*)calloc(25, sizeof(float));
        ampMEANs    = (float*)calloc(25, sizeof(float));
        for (j = 0; j < 25; j++)
        {
            freqMEANs[j] = freqMIPS->operator()(j, 5);
            freqMEANs[j] = freqMIPS->operator()(j, 5);
        }
    }
    // If found partials are less than required, zero pad
    else if (poso_v.size() < 25)
    {
        freqMEANs   = (float*)realloc(freqMEANs, 25 * sizeof(float));
        ampMEANs    = (float*)realloc(ampMEANs, 25 * sizeof(float));
        for (i = (int)poso_v.size(); i < 25; i++)
        {
            freqMEANs[i]    = 0;
            ampMEANs[i]     = 0;
        }
    }
}