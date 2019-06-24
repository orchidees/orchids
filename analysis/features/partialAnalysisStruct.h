/**
 *
 *  @file			partialAnalysisStruct.h
 *  @brief          Fill partials structure with partials analysis results
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__partialAnalysisStruct__
#define __Orchids__partialAnalysisStruct__

#include "Orchids.h"

/**
 *  @brief Fill partials structure with partials analysis results and convert partials frequencies to symbolic notes
 *
 *  @param partF Partials frequencies
 *  @param partA Partials amplitudes
 *  @param partFM Partials mean frequencies
 *  @param partAM Partials mean amplitudes
 */
partials partialAnalysisStruct(
                               matrixPtr   partF,
                               matrixPtr   partA,
                               float    *partFM,
                               float    *partAM);

#endif /* defined(__Orchids__partialAnalysisStruct__) */
