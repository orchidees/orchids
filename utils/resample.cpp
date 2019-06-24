//
//  resample.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 17/10/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "resample.h"


void resampleData(float* toResample, int tRSize, float* resampled, int resampleSize, int mode)
{
    SRC_DATA resample;
    float *signalf          = (float *)malloc(tRSize * sizeof(float));
    std::copy(toResample, toResample + tRSize, signalf);
    resample.data_in        = signalf;
    resample.data_out       = (float *)calloc(resampleSize, sizeof(float));
    resample.input_frames   = (float)tRSize;
    resample.output_frames  = (float)resampleSize;
    resample.src_ratio      = (float)resampleSize / (float)tRSize;
    int err_code = src_simple(&resample, mode, 1);
    if (err_code > 0)
        cout << src_strerror(err_code) << endl;
    std::copy(resample.data_out, resample.data_out + resampleSize, resampled);
    //Clear memory
    free(resample.data_in);
    free(resample.data_out);
}