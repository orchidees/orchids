 //
//  chordSeqOptimized.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "chordSeqOptimized.h"


//Apply the mean function on a matrix
int partialsMeanStd (matrixPtr amp_m, float* &amp1_v)
{
    int i, j, k;
    int size = amp_m->cols();
    amp1_v = (float *)malloc(size * sizeof(float));
    for (i = 0; i < size; i++)
    {
        float *col = amp_m->getCol(i);
        float *colNZ = (float *)calloc(amp_m->rows(), sizeof(float));
        for (j = 0, k = 0; j < amp_m->rows(); j++)
            if (col[j] > 0 && col[j] <= 1.0)
                colNZ[k++] = col[j];
        colNZ = (float *)realloc(colNZ, k * sizeof(float));
        amp1_v[i] = getMean(colNZ, k);
        free(col);
        free(colNZ);
    }

    return size;
}

//Apply the mean function on a matrix
int partialsMeanStd (matrixPtr amp_m, float* &amp1_v, int maxR)
{
    int i, j, k;
    int size = amp_m->cols();
    amp1_v = (float *)malloc(size * sizeof(float));
    for (i = 0; i < size; i++)
    {
        float *col = amp_m->getCol(i);
        float *colNZ = (float *)calloc(amp_m->rows(), sizeof(float));
        for (j = 0, k = 0; j < amp_m->rows(); j++)
            if (col[j] > 0 && col[j] <= 2.0 && (col[j] == col[j]))
                colNZ[k++] = col[j];
        colNZ = (float *)realloc(colNZ, maxR * sizeof(float));
        amp1_v[i] = getMean(colNZ, maxR);
        free(col);
        free(colNZ);
    }
    
    return size;
}

//Apply the median function on a matrix
int partialsMedianStd( matrixPtr freq_m, float* &freq1_v)
{
    int i, j, k;
    int size = freq_m->cols();
    freq1_v = (float *)calloc(size, sizeof(float));
    for (i = 0; i < size; i++)
    {
        float *col = freq_m->getCol(i);
        float *colNZ = (float *)calloc(freq_m->rows(), sizeof(float));
        for (j = 0, k = 0; j < freq_m->rows(); j++)
            if (col[j] > 0)
                colNZ[k++] = col[j];
        colNZ = (float *)realloc(colNZ, k * sizeof(float));
        freq1_v[i] = getMedian(colNZ, k);
        if (freq1_v[i] != freq1_v[i])
            freq1_v[i] = 0;
        free(col);
        free(colNZ);
    }
    
    return size;
}

//Apply the median function on a matrix
int partialsMedianStd( matrixPtr freq_m, float* &freq1_v, int maxR)
{
    int i, j, k;
    int size = freq_m->cols();
    freq1_v = (float *)calloc(size, sizeof(float));
    for (i = 0; i < size; i++)
    {
        float *col = freq_m->getCol(i);
        float *colNZ = (float *)calloc(freq_m->rows(), sizeof(float));
        for (j = 0, k = 0; j < freq_m->rows(); j++)
            if (col[j] > 1 && (col[j] == col[j]))
                colNZ[k++] = col[j];
        colNZ = (float *)realloc(colNZ, maxR * sizeof(float));
        freq1_v[i] = getMedian(colNZ, maxR);
        free(col);
    }
    
    return size;
}

//Launch the PM2 analysis, retrieve the matrixPtr of frequencies and amplitudes
void chordSeqOptimized2(
                       const char      *soundfile,
                       string          analysedir,
                       float           sr_hz,
                       int             f0min,
                       //int             numpartialsin,
                       float           debut,
                       float           fin,
                       float           nbSteps,
                       pTrack_params&  pm2,
                       matrixPtr       freqo_F,
                       matrixPtr       ampo_F)
{
    float           res, maxR, tDeb, tFin, chorddur = ((nbSteps == 1) ? 0.01 : 0.7);
    float           stepDur, stepLen;
    int             i, j, k, sum, freq1_vSize, size;
    const char*     soundfileRep = strrep<const char *>(soundfile, " ", "\\ ");
    float           *freq1_v, *amp1_v;
    vector<int>     sp_v, pos_to_merge;
    frame_reader    reader;
    
    pm2.windowAnalysisSize = MAX(pow(2,ceil(log2f((5.0f * sr_hz)/(float)f0min))), 2048);
    pm2.windowAnalysisStep = pm2.windowAnalysisSize / 4;
    pm2.windowFFTSize = MIN(pow(2,ceil(log2(pm2.windowAnalysisSize))+2), 16384);
    string s = soundfile;
    string outSdifFile = "/tmp/orchis_ptrack_temp.sdif";
    if (nbSteps != 1)
    {
//    string command = (string)pm2Path + " -S" + soundfileRep + " -Apar -N" + to_string(pm2.windowFFTSize) + " -M" + to_string(pm2.windowAnalysisSize) + " -I" + to_string(pm2.windowAnalysisStep)  + " -m" + to_string(pm2.minAmpAtt) + " -W" + pm2.windowType + " -q" + to_string(pm2.nPartials) + " -a" + to_string(pm2.smoothEnvAtt) + " -r" + to_string(pm2.smoothEnvRel) + " -Ct0.4 -Cf" + to_string(powf(2, 70.0 / 1200.0)-1) + " --devFR=0.03526492 --devFC=0 --devM=1 --devK=3 -L0.05 " + " " + outSdifFile;
//        string command = (string)pm2Path + " -S" + soundfileRep + " -Apar -N" + to_string(pm2.windowFFTSize) + " -M" + to_string(pm2.windowAnalysisSize) + " -I" + to_string(pm2.windowAnalysisStep)  + " -m" + to_string(pm2.minAmpAtt) + " -W" + pm2.windowType + " -q" + to_string(pm2.nPartials) + " -a" + to_string(pm2.smoothEnvAtt) + " -r" + to_string(pm2.smoothEnvRel) + " -Ct0.40000001 -Cf0.029302239 -devFR=0.011619449 --devFC=0 --devM=1 --devK=3 -L0.05 " + " " + outSdifFile;
        string command = (string)pm2Path + " -S" + soundfileRep + " -Apar -N" + to_string(pm2.windowFFTSize) + " -M" + to_string(pm2.windowAnalysisSize) + " -I" + to_string(pm2.windowAnalysisStep)  + " -m" + to_string(pm2.minAmpAtt) + " -W" + pm2.windowType + " -q" + to_string(pm2.nPartials) + " -a" + to_string(pm2.smoothEnvAtt) + " -r" + to_string(pm2.smoothEnvRel) + " -Ct" + to_string(pm2.timeGapConnect) + " -Cf" + to_string(pm2.freqGapConnect) + " -devFR=" + to_string(pm2.relativeFreqDev) + " --devA=" + to_string(pm2.relativeAmpDev) + " --devFC=" + to_string(pm2.constantFreqDev) + " --devM=1 --devK=3 -L" + to_string(pm2.minPartialLength) + " " + outSdifFile;
        //+ " -B" + to_string(0) + " -E" + to_string(fin)
        if (system(command.c_str()) > 0)
            throw "Error during PM2 Partials execution";
    }
    //Read SDIF file, count frames and get max length
    SdifGenInitCond(NULL);
    int nbFrames = 0;
    size_t bytesread = SdifReadFile(outSdifFile.c_str(), NULL, MySdifCountFrames, NULL, NULL, NULL, &nbFrames);
    reader.bytesread = bytesread;
    int maxLength    = 0;
    //SdifReadSimple(outSdifFile.c_str(), MySdifGetMaxLength, &maxLength);

    stepLen = nbFrames / (float)nbSteps;
    stepDur = stepLen + 1;
     
    for (i = 1; i <= nbSteps; i++)
    {
        tDeb = floor(((i - 1) * stepLen) + 1);
        tFin = ceil(min(tDeb + stepDur, (float)nbFrames));
        if (nbSteps == 1)
        {
            if (pm2.meanStart >= 1) { tDeb = pm2.meanStart; } else { tDeb = (int)((float)tFin * pm2.meanStart); }
            if (pm2.meanEnd > 1) { tFin = pm2.meanEnd; } else { tFin = (int)((float)tFin * pm2.meanEnd); }
            reader.first_frame  = 0;
            reader.last_frame   = tDeb - 1;
            mySdifReadFile(outSdifFile.c_str(), NULL, NULL, NULL, MySdifQueryMatrixDataPM2Optimized, NULL, &reader);
        }
        
        int m_rows_deb = tDeb - 1;
        int m_rows_fin = tFin;
        int m_rows = m_rows_fin - m_rows_deb + 1;
        
        reader.first_frame  = m_rows_deb;
        reader.last_frame   = m_rows_fin;
        mySdifReadFile(outSdifFile.c_str(), NULL, NULL, NULL, MySdifQueryMatrixDataPM2Optimized, NULL, &reader);
        
        matrixPtr ampM  = trcToMat(reader.frames, maxLength, 3);
        matrixPtr freqM = trcToMat(reader.frames, maxLength, 2);
        
        if (!ampM)
            continue;
        
        // Happens sometimes when the sound duration is very small
        if (m_rows != freqM->rows())
            m_rows = freqM->rows();
        
        //Erase columns that don't verify this condition: sum(freq_m ~= 0,1)./size(freq_m,1) > chorddur
        for (j = 0, maxR = 0; j < freqM->cols(); j++)
        {
            sum = 0;
            res = 0;
            for (k = 0; k < m_rows; k++)
            {
                if (freqM->operator()(k, j) > 1 && ampM->operator()(k, j) > 1e-9)
                    sum++;
            }
            res = (float)sum / (float)m_rows;
            if (res > chorddur)
            {
                if (sum > maxR)
                    maxR = sum;
                //Keep index and count column to merge
                pos_to_merge.push_back(j);
            }
        }
        
        //Initialize the merged matrices
        matrixPtr freq_mRes(new matrix(m_rows, (int)pos_to_merge.size()));
        matrixPtr amp_mRes(new matrix(m_rows, (int)pos_to_merge.size()));
        
        
        for (j = 0; j < (int)pos_to_merge.size(); j++)
        {
            //Add the column to merge
            for (k = 0; k < freq_mRes->rows(); k++)
                freq_mRes->operator()(k, j) = freqM->operator()(k, pos_to_merge[j]);
            for (k = 0; k < amp_mRes->rows(); k++)
                amp_mRes->operator()(k, j) = ampM->operator()(k, pos_to_merge[j]);
        }
        
        //Calculate mean of amplitudes matrix
        partialsMeanStd(amp_mRes, amp1_v, maxR);
        //Calculate median of frequencies matrix
        freq1_vSize = partialsMedianStd(freq_mRes, freq1_v, maxR);
        
        mySort(freq1_v, freq1_vSize, freq1_v, sp_v);
        reorder(amp1_v, freq1_vSize, sp_v, amp1_v);
                
        //Resize output matrices, keep data
        if (freqo_F->rows() < freq1_vSize)
        {
            size = freqo_F->rows();
            freqo_F->resizeTo(freq1_vSize, nbSteps, true);
            ampo_F->resizeTo(freq1_vSize, nbSteps, true);
            
            for (j = size; j < freq1_vSize; j++)
                for (k = 0; k < nbSteps; k++)
                {
                    freqo_F->operator()(j, k)   = 0;
                    ampo_F->operator()(j, k)    = 0;
                }
        }
        
        //Fill output matrices
        for (j = 0; j < freq1_vSize; j++)
        {
            freqo_F->operator()(j, i - 1)   = freq1_v[j];
            ampo_F->operator()(j, i - 1)    = amp1_v[j];
        }
        
        //Clean memory
        pos_to_merge.clear();
        sp_v.clear();
        free(freq1_v);
        free(amp1_v);
        reader.frames.clear();
    }

    /* cleanup */
    SdifGenKill();
}

