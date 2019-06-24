/**
 *
 *  @file			analysis.h
 *  @brief			Header definition for analysis
 *  @author			Antoine Bouchereau
 *  @version        0.06
 *	@date			16-12-2013
 *
 */

#ifndef Orchids_analysis_include_h
#define Orchids_analysis_include_h

#include "convert/dynamicsToLoudness.h"
#include "convert/hzToERB.h"
#include "convert/noteToHz.h"
#include "convert/hzToNote.h"
#include "convert/midi2mtnotes.h"
#include "descriptor/ircamdescriptor.h"
#include "features/chordSeqOptimized.h"
#include "features/mips2.h"
#include "features/partialsOptimized.h"
#include "features/calcN6.h"
#include "features/melBands.h"
#include "partialAnalysisStruct.h"
#include "sax/min_distDTWBSF.h"
#include "sdif/sdif.h"
#include "utils/importSignal.h"
#include "utils/cluster.h"
#include "utils/printHelp.h"
#include "envelope/medflt.h"
#include "envelope/computeOnset.h"
#include "envelope/computeNextMinima.h"
#include "envelope/computePrevMinima.h"
#include "envelope/computeEffortsTimes.h"
#include "envelope/computeEnvelope.h"
#include "envelope/computeADSR.h"

#include "extractSymbolicInfos.h"
#include "defaultAnalysisParams.h"
#include "getConfigAnalysisParams.h"
#include "analyzeTarget.h"
#include "analyzeSound.h"
#include "launchSoundAnalysis.h"

/**
 *  @brief Launch analysis module
 */
int launchAnalysisModule(int argc, const char * argv[], sqlStruct& s);

#endif
