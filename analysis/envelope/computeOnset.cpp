//
//  computeOnset.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 05/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "computeOnset.h"
#define MAX_LENGTH 10000

void unwrap(float p[], int N)
// ported from matlab (Dec 2002)
{
    float dp[MAX_LENGTH];
    float dps[MAX_LENGTH];
    float dp_corr[MAX_LENGTH];
    float cumsum[MAX_LENGTH];
    float cutoff = M_PI;               /* default value in matlab */
    int j;
    
    assert(N <= MAX_LENGTH);
    
    // incremental phase variation
    // MATLAB: dp = diff(p, 1, 1);
    for (j = 0; j < N-1; j++)
        dp[j] = p[j+1] - p[j];
    
    // equivalent phase variation in [-pi, pi]
    // MATLAB: dps = mod(dp+dp,2*pi) - pi;
    for (j = 0; j < N-1; j++)
        dps[j] = (dp[j]+M_PI) - floor((dp[j]+M_PI) / (2*M_PI))*(2*M_PI) - M_PI;
    
    // preserve variation sign for +pi vs. -pi
    // MATLAB: dps(dps==pi & dp>0,:) = pi;
    for (j = 0; j < N-1; j++)
        if ((dps[j] == -M_PI) && (dp[j] > 0))
            dps[j] = M_PI;
    
    // incremental phase correction
    // MATLAB: dp_corr = dps - dp;
    for (j = 0; j < N-1; j++)
        dp_corr[j] = dps[j] - dp[j];
    
    // Ignore correction when incremental variation is smaller than cutoff
    // MATLAB: dp_corr(abs(dp)<cutoff,:) = 0;
    for (j = 0; j < N-1; j++)
        if (fabs(dp[j]) < cutoff)
            dp_corr[j] = 0;
    
    // Find cumulative sum of deltas
    // MATLAB: cumsum = cumsum(dp_corr, 1);
    cumsum[0] = dp_corr[0];
    for (j = 1; j < N-1; j++)
        cumsum[j] = cumsum[j-1] + dp_corr[j];
    
    // Integrate corrections and add to P to produce smoothed phase values
    // MATLAB: p(2:m,:) = p(2:m,:) + cumsum(dp_corr,1);
    for (j = 1; j < N; j++)
        p[j] += cumsum[j-1];
}

void max_heapify(float *a, int i, int n)
{
    int j;
    float temp;
    temp = a[i];
    j = 2 * i;
    while (j <= n)
    {
        if (j < n && a[j + 1] > a[j])
            j = j + 1;
        if (temp > a[j])
            break;
        else if (temp <= a[j])
        {
            a[j / 2] = a[j];
            j = 2 * j;
        }
    }
    a[j / 2] = temp;
    
    return;
}

void build_maxheap(float *a, int n)
{
    int i;
    for(i = n / 2; i >= 1; i--)
        max_heapify(a, i, n);
}

int computeOnset(vector<float>& audio, int sRate)
{
    /*
    // Apply median filtering in the feature sequences (twice), using 5 windows
    TMedianFilter1D<float> flt;
    flt.SetWindowSize(5);
    flt.Execute(env, true);
    vector<float> E = flt.getArray();
    float E_mean = getMean(&E[0], (int)E.size());
    float T_E = E_mean / 2;
    // Thresholding
    vector<int> flags;
    for (i = 0; i < E.size(); i++)
        if (E[i] >= T_E)
            return i;
    return 0;
    */
    int i, j, n;
    float fMin = 30;
    float fMax = 8000;
    float nfft = 8192;
    // Builds Signal STFT, puts it in values and returns it.
    matrixPtr values(new matrix(nfft / 2, ((int)audio.size() - 1024 + 512) / 512));
    STFT(&audio[0], (int)audio.size(), 1024, 512, "rectwin", nfft, values);
    // Compute corresponding fmin / fmax indexes
    int IFmin = _max(1, round(fMin / sRate * nfft));
    int IFmax = round(_min(fMax / 2, fMax / sRate * nfft));
    // Take only frame inside our range
    matrixPtr   SigFrame = matrixPtr(new matrix(values->resizeMatrixByRow(IFmin, IFmax)));
    int         LenWindow = SigFrame->rows();
    int         Nbre = SigFrame->cols();
    matrixPtr   SigOut(new matrix(Nbre, 1));
    matrixPtr   Phi(new matrix(LenWindow, Nbre));
    for (i = 0; i < Phi->cols(); i++)
    {
        float *col = SigFrame->getCol(i);
        unwrap(col, LenWindow);
        for (j = 0; j < Phi->rows(); j++)
            Phi->operator()(j, i) = col[j];
        free(col);
    }
    matrixPtr  DevPhi(new matrix(LenWindow, Nbre));
    matrixPtr  PhiBis(new matrix(Phi->rows(), Phi->cols() - 2));
    for (i = 0; i < PhiBis->cols(); i++)
        for (j = 0; j < PhiBis->rows(); j++)
            PhiBis->operator()(j, i) = Phi->operator()(j, i + 2) - 2 * Phi->operator()(j, i + 1) + Phi->operator()(j, i);
    for (i = 0; i < PhiBis->cols(); i++)
        for (j = 0; j < PhiBis->rows(); j++)
            DevPhi->operator()(j, i + 2) = PhiBis->operator()(j, i);

    for (n = 1; n < Nbre; n++)
    {
        vector<float> SigOutInst;
        for (i = 0; i < DevPhi->rows(); i++)
            SigOutInst.push_back(sqrt( pow(abs(SigFrame->operator()(i, n - 1)), 2) + pow(abs(SigFrame->operator()(i, n)), 2)  - 2 * abs(SigFrame->operator()(i, n)) * abs(SigFrame->operator()(i, n - 1)) * cos(DevPhi->operator()(i, n))));
        SigOut->operator()(n) = getSum(&SigOutInst[0], (int)SigOutInst.size());
    }
    
    float maxAbs = getMaxAbs(SigOut->c_data(), SigOut->size());
    SigOut->operator*(1 / maxAbs);
    float minPeakHeight = std_dev(SigOut->c_data(), SigOut->size(), getMean(SigOut->c_data(), SigOut->size())) * 0.75;
    for (i = 0; i < SigOut->size(); i++)
        if (SigOut->operator()(i) >= minPeakHeight)
            return (i + 1) * 512;
    
    return 0;
}


