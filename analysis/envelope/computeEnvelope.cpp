//
//  computeEnvelope.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 05/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

/*
* Implementation of the automatic note segmentation given in:
*
*        Caetano, M., Burred, J. J., Rodet, X.
*        Automatic Segmentation of the Temporal Evolution of Isolated Acoustic
*        Musical Instrument Sounds Using Spectro-Temporal Cues.
*        In Proc. DAFx 2010
*
*    Returns a list of dictionaries (one for each note) with the following keys:
*    * attack, decay, sustain, release, offset
*/

#include "computeEnvelope.h"


envelope computeEnvelope(
                         vector<float>& audio,
                         int            sRate,
                         vector<float>& env)
{
    int i;
    // First we find the true onset of the sound (start of attack)
    int decay = computeOnset(audio, sRate);
    int onset = computePrevMinima(env, decay);
    // Initialize the boundaries
    envelope boundaries;
    boundaries.attack   = onset;
    boundaries.decay    = decay;
    boundaries.sustain  = onset;
    boundaries.release  = onset;
    boundaries.offset   = (int)audio.size();
    
    // -- Offset -- = Last point where TAE has same energy as onset
    int n   = 256;
    vector<float> copyEnv(env.begin() + _max(onset - (n / 2), 1), env.begin() + onset + (n / 2) + 1);
    for (i = 0; i < copyEnv.size(); i++)
        copyEnv[i] = pow(copyEnv[i], 2);
    float   onsetEnergy = getSum(&copyEnv[0], (int)copyEnv.size());
    int     audioPos = onset + (sRate * 200 / 1000);
    if (audioPos > audio.size())
        audioPos = (int)audio.size() - 1;
    while (audioPos <= (audio.size() - n))
    {
        vector<float> frame(env.begin() + audioPos - (n / 2), env.begin() + audioPos + (n / 2) + 1);
        for (i = 0; i < frame.size(); i++)
            frame[i] = pow(frame[i], 2);
        float energy = getSum(&frame[0], (int)frame.size());
        if (energy <= (onsetEnergy + std::numeric_limits<float>::epsilon()))
            break;
        audioPos += (n / 8);
    }
    boundaries.offset = audioPos;
    
    // -- Decay (end of attack) / Sustain -- using modified Peeters' efforts
    int     max_transient = _min(decay + sRate, (int)env.size() - 1 - decay);
    if (decay > max_transient)
        max_transient = decay + sRate;
    if (max_transient >= env.size())
        max_transient = (int)env.size() - 1;
    copyEnv.clear();
    copyEnv = vector<float>(env.begin() + decay, env.begin() + max_transient + 1);
    float   max_env;
    int     max_env_loc;
    getMax(&copyEnv[0], (int)copyEnv.size(), max_env, max_env_loc);
    max_env_loc += decay;
    copyEnv.clear();
    copyEnv = vector<float>(env.begin() + decay, env.begin() + max_env_loc + 1);
    vector<float>   effort_thresholds   = linspace<float>(0, max_env, 10);
    vector<int>     effort_times        = computeEffortsTimes(copyEnv, effort_thresholds);
    for (i = 0; i < effort_times.size(); i++)
        effort_times[i] += decay;
    vector<int> efforts = diff(effort_times);
    int w = getMean(&efforts[0], (int)efforts.size());
    int M = 0;
    for (i = 0; i < efforts.size(); i++)
        if (efforts[i] > w)
            M++;

    // -- Start of sustain -- = first point at which effort > M * w
    int sustain = max_env_loc;
    for (i = 0; i < efforts.size(); i++)
        if (efforts[i] > (M * w))
        {
            sustain = effort_times[i];
            break;
        }
    boundaries.sustain = sustain;
    
    // -- Decay (end of attack) -- = first local minima (in spectral centroid) between onset and sustain
    //int sc_minima       = computeNextMinima(sc, onset);
    //boundaries.decay    = sc_minima;
    
    // -- Start of release -- = modified Peeters' efforts method (reversed)
    copyEnv.clear();
    copyEnv = vector<float>(env.begin() + onset, env.begin() + boundaries.offset + 1);
    getMax(&copyEnv[0], (int)copyEnv.size(), max_env, max_env_loc);
    max_env_loc += onset;
    copyEnv.clear();
    for (i = boundaries.offset; i >= onset; i--)
        copyEnv.push_back(env[i]);
    effort_thresholds.clear();
    effort_times.clear();
    effort_thresholds   = linspace<float>(0, max_env, 10);
    effort_times        = computeEffortsTimes(copyEnv, effort_thresholds);
    efforts.clear();
    efforts = diff(effort_times);
    w = getMean(&efforts[0], (int)efforts.size());
    M = 0;
    for (i = 0; i < efforts.size(); i++)
        if (efforts[i] > w)
            M++;
    // First point at which effort > M * w
    int release = max_env_loc;
    for (i = 0; i < efforts.size(); i++)
        if (efforts[i] > (M * w))
        {
            release = boundaries.offset - effort_times[i];
            break;
        }
    boundaries.release = release;
    boundaries.bestSustainWindow1 = (sustain + release) / 2;
    boundaries.bestSustainWindow2 = boundaries.bestSustainWindow1 + 512;
    return boundaries;
}