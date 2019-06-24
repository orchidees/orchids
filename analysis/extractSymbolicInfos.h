/**
 *
 *  @file			extractSymbolicInfos.h
 *  @brief			Extract symbolic informations from a sound
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */


#ifndef __Orchids__extractSymbolicInfos__
#define __Orchids__extractSymbolicInfos__

#include "Orchids.h"

/**
 *  @brief Extract symbolic informations from the filename and stores them in the sql structure
 *
 *  @param filename Name of the sound to analyze
 *  @param sqlFeatures Base structure to fill with the symbolic informations
 */
void extractSymbolicInfos(string filename, sqlStruct &sqlFeatures, string source);


#endif
