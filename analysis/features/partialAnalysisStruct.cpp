//
//  partialAnalysisStruct.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "partialAnalysisStruct.h"


partials partialAnalysisStruct(
                               matrixPtr   partF,
                               matrixPtr   partA,
                               float    *partFM,
                               float    *partAM)
{
    struct partials Pa;
    Pa.partialsFrequency            = partF;
    Pa.partialsAmplitude            = partA;
    Pa.partialsMeanFrequency        = partFM;
    Pa.partialsMeanFrequencySize    = partF->rows();
    Pa.partialsMeanAmplitude        = partAM;
    Pa.partialsMeanAmplitudeSize    = partA->rows();

    return Pa;
}