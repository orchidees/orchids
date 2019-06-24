/**
 *
 *  @file			analyzeTarget.h
 *  @brief			Spectral analysis of a sound to add in the database
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */


#ifndef __Orchids__analyzeTarget__
#define __Orchids__analyzeTarget__

#include "Orchids.h"

/**
 *  @brief Spectral analysis of a sound to add in the database
 *
 *  The input file is specified as a complete pathname. This file is then
 *  analysed with the descriptors obtained using IRCAMDescriptor.
 *  Each descriptor is stored in a structure using its mean and standard
 *  deviation values as well as two temporal modelisations.
 *
 *  @param soundfile Path of the sound file to analyze
 *  @param params Parameters structure for IRCAM Descriptor and PM2 analysis
 *  @param sqlFeatures Base structure to fill
 */
bool analyzeTarget(
                   string           soundfile,
                   analysis_params  params,
                   sqlStruct        &sqlFeatures);


#endif /* defined(__Orchids__analyzeTarget__) */
