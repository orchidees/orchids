/**
 *
 *  @file			launchSoundAnalysis.h
 *  @brief			Launch the analysis and retrieve data
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__launchSoundAnalysis__
#define __Orchids__launchSoundAnalysis__

#include "Orchids.h"


/**
 *  @brief Launch the analysis and retrieve data
 *
 *  Send the analysis file (.wav) to IRCAMDescriptor, PM2 and mel bands analysis.
 *  All features, partials and mel bands are stored in the sql structure.
 *
 *  @param soundfile Path of the sound file to analyze
 *  @param params Parameters structure for IRCAM Descriptor and PM2 analysis
 *  @param sqlStruct Base structure to fill
 */
void launchSoundAnalysis(
                         const char         *soundFile,
                         analysis_params    params,
                         sqlStruct          &sqlStruct);

#endif /* defined(__Orchids__launchSoundAnalysis__) */
