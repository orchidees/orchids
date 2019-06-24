//
//  oscProduction.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscProduction__
#define __Orchids__oscProduction__

#include "Orchids.h"

class Production;

/**
 *  @brief Set the microtonic resolution
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void setResolution          ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Set an orchestra
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void setOrchestra           ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Set filters to reduce search space
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void setFilters             ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Reset the filters
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void getNumFiltered         ( const osc::ReceivedMessage& m, Session *sSession );
/**
 *  @brief Reset the filters
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void resetFilters           ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Set harmonic filtering to true or false
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void setHarmonicFiltering   ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Set static mode to true or false
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void setStaticMode          ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Return the list of instruments in usual orchestral order
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void getScoreOrder          ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
/**
 *  @brief Return the list of instruments added by the user
 *  @param m Received OSC message to parse
 *  @param sProduction Pointer to the current production
 */
void getUserInstrus         ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
void getUserPlayingStyles   ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
void getSources             ( const osc::ReceivedMessage& m, ProductionPtr sProduction );
void isInBase               ( const osc::ReceivedMessage& m, ProductionPtr sProduction );


#endif /*  defined(__Orchids__oscProduction__) */
