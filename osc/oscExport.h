//
//  oscExport.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscExport__
#define __Orchids__oscExport__

#include "Orchids.h"

/**
 *  @brief Export all solutions into wav file
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void exportSolutions                    ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Export one solution into wav file
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void exportOneWavSolution               ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Export one solution symbolics
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void exportOneSolution                  ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Send distances for each solution from the target for two criterias
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void getAllSolutionsDistancesFromTarget ( const osc::ReceivedMessage& m, Session* sSession );

#endif /* defined(__Orchids__oscExport__) */
