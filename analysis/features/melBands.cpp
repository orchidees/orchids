//
//  melBands.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "melBands.h"

matrixPtr melBandsSoundFix(
                           float*    &snd_v,
                           int       &snd_vSize,
                           float     sr_hz,
                           crs*      &filtre_m)
{
    int nbFiltre    = 70;
    int winsize_k   = 8100;
    int hopsize_k   = 4000;
    int fftsize_k   = 32768;
    int i, j;

    if (filtre_m == nullptr)
        melbankm(nbFiltre, fftsize_k - 1, sr_hz, 0, 0.5, "tz", filtre_m);
        
    if (snd_vSize < winsize_k)
    {
        snd_v = (float *)realloc(snd_v, winsize_k * sizeof(float));
        for (i = snd_vSize; i < winsize_k; i++)
            snd_v[i] = 0;
        snd_vSize = winsize_k;
    }
    
    matrixPtr amp_m(new matrix(fftsize_k / 2, (snd_vSize - winsize_k + hopsize_k) / hopsize_k));
    float fenetreSum = STFT(snd_v, snd_vSize, winsize_k, hopsize_k, "blackman", fftsize_k, amp_m);
    
    amp_m->operator/(fenetreSum);
    amp_m->operator*(2);

    int     nb_frame    = amp_m->cols();
    int     nb_filtre   = filtre_m->rows;
    float  *col;
    float  *r;
    
    matrixPtr ampbank_s(new matrix(nb_filtre, nb_frame));
    //Applying the filter
    for (j = 0; j < nb_frame; j++) //nb_frame
    {
        col = amp_m->getCol(j);
        r   = (float *) malloc(nb_filtre * sizeof(float));
        
        // Multiplication between the compressed row storage matrixPtr (filter)
        // and the resized column of amp_m
        crs_mul(filtre_m, col, r);
        
        for (i = 0; i < nb_filtre; i++)
            ampbank_s->operator()(i, j) = r[i];

        free(col);
        free(r);
    }
    return ampbank_s;
}

void melBandsStruct(matrixPtr meln_s, sqlStruct& features, int signalSize)
{
    int            i, j, k;
    float          loudnessExp      = 0.6;
//    printf("***\n***\n***\n%d - %d\n***\n***\n***\n", meln_s->rows(), meln_s->cols());
//    matrixPtr tmpMel = matrixPtr(new matrix(meln_s));
//    tmpMel->resizeMatrixByCol(0, 7);
    float          N6source = 0.0f;
    for (i = 0; i < features.descriptors.size(); i++)
        if (features.descriptors[i].name == "Loudness")
        {
            for (j = 0; j < features.descriptors[i].values->size(); j++)
                if (features.descriptors[i].values->operator()(j) > N6source)
                    N6source = features.descriptors[i].values->operator()(j);
            break;
        }
    float          N6cible          = dynamicsToLoudness(features.intentDynamic);
    float          L6source         = powf(2, N6source / 10.0);
    float          L6cible          = powf(2, N6cible / 10.0);
    float          Lfacteur         = MIN(powf(L6cible, 1.0 / loudnessExp) / powf(L6source, 1.0 / loudnessExp), 1);
//    meln_s->operator*(Lfacteur);
    float          ampMeanNorm      = pow(norm(features.partials.partialsMeanAmplitude, features.partials.partialsMeanAmplitudeSize), 2 );
    matrixPtr      curAmp           = features.partials.partialsAmplitude;
    float          *ampEner         = (float*)malloc(curAmp->cols() * sizeof(float));

    for (i = 0; i < curAmp->cols(); i++)
    {
        float *col = curAmp->getCol(i);
//        for (j = 0; j < curAmp->rows(); j++)
//            col[j] *= Lfacteur;
        ampEner[i] = pow(norm(col, curAmp->rows()), 2);
        free(col);
    }
    
    // Compute melMeanAmplitude with ADSR
    int begin   = floor((float)features.ADSR.bestSustainWindow1 * (float)meln_s->cols() / (float)signalSize);
    int end     = ceil((float)features.ADSR.bestSustainWindow2 * (float)meln_s->cols() / (float)signalSize);
    int step    = abs(end - begin);
    if (step < 1) step = 1;
    (begin - step * 5 < 1) ? begin = 1 : begin -= step * 5;
    end += step * 5;
    if (end >= meln_s->cols()) end = meln_s->cols();
        
    matrixPtr copy = matrixPtr(new matrix(meln_s));
    copy->resizeMatrixByCol(begin, end);
    float* meanAmps = copy->getMean(2);
    //BubbleSort(meanAmps, copy->rows());
    features.melbands.melMeanAmp = matrixPtr(new matrix(1, copy->rows()));
    for (i = 0; i < copy->rows(); i++)
        features.melbands.melMeanAmp->operator()(i) = meanAmps[i];
    free(meanAmps);
    
    // Resample meln_s
    matrixPtr resampleCurArray(new matrix(70, 64));
    for (k = 0; k < meln_s->rows(); k++)
    {
        float* curArrayRow  = meln_s->getRow(k);
        float* row          = new float[64];
        resampleData(curArrayRow, meln_s->cols(), row, 64, SRC_LINEAR);
        for (i = 0; i < 64; i++)
            resampleCurArray->operator()(k, i) = row[i];
        delete row;
    }
    
    // Mel Energy
    matrixPtr melEnergy(new matrix(1, resampleCurArray->cols()));
    for (i = 0; i < resampleCurArray->cols(); i++)
    {
        float *col = resampleCurArray->getCol(i);
        melEnergy->operator()(i) = pow(norm(col, resampleCurArray->rows()), 2);
        free(col);
    }
    
    // Fill structure
    features.melbands.loudnessFactor        = Lfacteur;
    features.melbands.melAmplitude          = resampleCurArray;
    float melnMeanNorm                      = resampleCurArray->norm();
    if (melnMeanNorm != 0)
        features.melbands.melAmplitude->operator/(melnMeanNorm);
    features.melbands.melEnergy             = melEnergy;
    features.melbands.melMeanEnergy         = pow(melnMeanNorm, 2);
    features.partials.partialsEnergy        = ampEner;
    features.partials.partialsMeanEnergy    = ampMeanNorm;
    features.partials.partialsEnergySize    = curAmp->cols();
}


void melbankm(
              float         p,
              float         n,
              float         fs,
              float         fl,
              float         fh,
              const char    *w,
              crs*          &x)
{
    int    i, j;
    float  f0  = 700/fs;
    float  fn2 = floor(n/2);
    float  lr  = log((f0 + fh) / (f0 + fl)) / (p + 1);
    //Convert to fft bin numbers with 0 for DC term
    float  bl[4] = {0, 1, p, p+1};
    for (i = 0; i < 4; i++)
        bl[i] = n * ((f0 + fl) * exp(bl[i] * lr) - f0);
    
    float  b2 = ceil(bl[1]);
    float  b3 = floor(bl[2]);
    float  b1 = floor(bl[0]) + 1;
    float  b4 = min(fn2, ceil(bl[3])) - 1;
    float  *pf;
    float  *fp;
    int    *r;
    int    *c;
    float  *v;
    float  *pm = nullptr;
    int    size, mn, mx;
    
    
    if (strcmp(w, "y") == 0) {
        assert(b3 >= b2);
        int pfSize  = b3 - b2 + 1;
        pf          = (float *)malloc(pfSize * sizeof(float));
        fp          = (float *)malloc(pfSize * sizeof(float));
        j           = b2;
        for (i = 0; i < pfSize; i++)
        {
            pf[i] = log((f0 + (j) / n) / (f0 + fl)) / lr;
            fp[i] = floor(pf[i]);
            j++;
        }

        //Construct r = [ones(1,b2) fp fp+1 p*ones(1,fn2-b3)];
        size = (fn2 - b3 < 0) ? b2 + pfSize * 2 : b2 + pfSize * 2 + fn2 - b3;
        r = (int *)malloc(size * sizeof(int));
        for (i = 0; i < b2; i++)
            r[i] = 1;
        for (j = 0; j < pfSize; j++)
            r[i++] = fp[j];
        for (j = 0; j < pfSize; j++)
            r[i++] = fp[j] + 1;
        for (; i < size; i++)
            r[i] = p;

        //Construct c = [1:b3+1 b2+1:fn2+1];
        size = ((fn2 + 1) - (b2 + 1) + 1 < 0) ? b3 + 1 : b3 + 1 + (fn2 + 1) - (b2 + 1) + 1;
        c = (int *)malloc(size * sizeof(int));
        for (i = 0; i < b3 + 1; i++)
            c[i] = i + 1;
        for (j = b2 + 1; j <= fn2 + 1; j++)
            c[i++] = j;

        //Construct v = 2*[0.5 ones(1,b2-1) 1-pf+fp pf-fp ones(1,fn2-b3-1) 0.5];
        size = (fn2 - b3 - 1 < 0) ? b2 + pfSize * 2 + 1 : b2 + pfSize * 2 + fn2 - b3;
        v = (float *)malloc(size * sizeof(float));
        v[0] = 0.5;
        for (i = 1; i <= b2 - 1; i++)
            v[i] = 1;
        for (j = 0; j < pfSize; j++)
            v[i++] = 1 - pf[j] + fp[j];
        for (j = 0; j < pfSize; j++)
            v[i++] = pf[j] - fp[j];
        for (j = 0; j < fn2 - b3 - 1; j++)
            v[i++] = 1;
        v[size - 1] = 0.5;
        
        for (i = 0; i < size; i++)
            v[i] = 2 * v[i];

        mn = 1;
        mx = fn2 + 1;
        
    }else{        
        assert(b4 >= b1);
        size    = b4 - b1 + 1;
        pf      = (float *)malloc(size * sizeof(float));
        fp      = (float *)malloc(size * sizeof(float));
        pm      = (float *)malloc(size * sizeof(float));
        j       = b1;
        for (i = 0; i < size; i++)
        {
            pf[i] = log((f0 + (j) / n) / (f0 + fl)) / lr;
            fp[i] = floor(pf[i]);
            pm[i] = pf[i] - fp[i];
            j++;
        }
        
        float k2 = b2 - b1 + 1;
        float k3 = b3 - b1 + 1;
        float k4 = b4 - b1 + 1;
        
        //Construct r = [fp(k2:k4) 1+fp(1:k3)];
        size    = (k4 - k2 < 0) ? k3 : k4 - k2 + 1 + k3;
        r       = (int *)malloc(size * sizeof(int));
        i       = 0;
        for (j = k2 - 1; j <= k4 - 1; j++)
        {
            r[i] = fp[j];
            i++;
        }
        for (j = 0; j < k3; j++)
            r[i++] = 1 + fp[j];
                
        //Construct c = [k2:k4 1:k3];
        size    = (k4 - k2 < 0) ? k3 : k4 - k2 + 1 + k3;
        c       = (int *)malloc(size * sizeof(int));
        i       = 0;
        for (j = k2; j <= k4; j++)
        {
            c[i] = j;
            i++;
        }
        for (j = 1; j <= k3; j++)
            c[i++] = j;
        
        //Construct v = 2*[1-pm(k2:k4) pm(1:k3)];
        size    = (k4 - k2 < 0) ? k3 : k4 - k2 + 1 + k3;
        v       = (float *)malloc(size * sizeof(float));
        i       = 0;
        for (j = k2 - 1; j <= k4 - 1; j++)
        {
            v[i] = 1 - pm[j];
            i++;
        }
        for (j = 0; j < k3; j++)
            v[i++] = pm[j];
        for (i = 0; i < size; i++)
            v[i] = 2 * v[i];
        
        mn	= b1 + 1;
        mx	= b4 + 1;
    }
    
    if (strcmp(w, "n") == 0)
    {
        for (i = 0; i < size; i++)
            v[i] = 1 - cos(v[i] * _pi / 2);
    }else if (strcmp(w, "m") == 0){
        for (i = 0; i < size; i++)
            v[i] = 1 - 0.92 / 1.08 * cos(v[i] * _pi / 2);
    }
    
    //c = c + mn - 1
    for (i = 0; i < size; i++)
        c[i] = c[i] + mn - 1;
    
    //Sorting and ordering columns, rows and values vectors
    vector<int> pos_v;
    mySort(r, size, r, pos_v);
    reorder(c, (int)pos_v.size(), pos_v, c);
    reorder(v, (int)pos_v.size(), pos_v, v);
    //Create the compressed row storage matrixPtr of size p-by-(1+fn2)
    x = crs_create(r, c, v, size, p, 1 + fn2);
    
    //Clear memory
    free(pf);
    free(fp);
    free(pm);
    free(r);
    free(c);
    free(v);
}