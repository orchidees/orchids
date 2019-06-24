//
//  oscSession.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscSession__
#define __Orchids__oscSession__

#include "Orchids.h"

class Session;

/**
 *  @brief Initialize a user session
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current session
 */
void initSession    ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set the orchestration search mode (Genetic or Optimal Warping)
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current session
 */
void setSearchMode  ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set an orchestra
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current session
 */
void setCriteria    ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Launch the orchestration
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current session
 */
void orchestrate    ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Launch the autoPilot
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current session
 */
void launchAutoPilot( const osc::ReceivedMessage& m, Session* sSession );

#endif /* defined(__Orchids__oscSession__) */
