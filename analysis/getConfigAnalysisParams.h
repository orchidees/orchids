/**
 *
 *  @file			getConfigAnalysisParams.h
 *  @brief			Read and parse 'Orchids.cfg', update and check parameters for analysis
 *  @author			Antoine Bouchereau
 *  @version        0.04
 *	@date			20-12-2013
 *
 */


#ifndef __Orchids__getConfigAnalysisParams__
#define __Orchids__getConfigAnalysisParams__

#include "Orchids.h"

/**
 *  @brief Check each parameter and send an error if its value is out of range 
 *  @param params Parameters structure for IRCAM Descriptor and PM2 analysis to check
 */
int checkConfigParams(analysis_params &params);

/**
 *  @brief Read 'Orchids.cfg' and update parameters for analysis
 *  @param params Parameters structure for IRCAM Descriptor and PM2 analysis to update
 */
void getConfigAnalysisParams(analysis_params &params, string configFile);


#endif
