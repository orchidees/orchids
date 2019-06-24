//
//  oscTarget.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscTarget__
#define __Orchids__oscTarget__

#include "Orchids.h"

class Target;

/**
 *  @brief Load a new soundfile in the current target
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void setSoundfile           ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Choose to use default analysis parameters or the configuration file
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void setAnalysisParams      ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set an abstract target to the orchestration session
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void setAbstractTarget      ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set a multi target to the orchestration session
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void setMultiTarget         ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set the abstract target duration
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void setTargetDuration      ( const osc::ReceivedMessage& m, TargetPtr sTarget );
/**
 *  @brief Modify a target feature
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void setTargetFeature       ( const osc::ReceivedMessage& m, TargetPtr sTarget );
/**
 *  @brief Get one target feature
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void getTargetSingle        ( const osc::ReceivedMessage& m, TargetPtr sTarget );
/**
 *  @brief Get target symbolics informations
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void getTargetSymbolics     ( const osc::ReceivedMessage& m, TargetPtr sTarget );
/**
 *  @brief Freeze or unfreeze on target feature
 *  @param m Received OSC message to parse
 *  @param sTarget Pointer to the current target
 */
void freezeFeature          ( const osc::ReceivedMessage& m, TargetPtr sTarget );
/**
 *  @brief Export a solution to wav sound and set it as a target
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void setSolutionAsTarget    ( const osc::ReceivedMessage& m, Session* sSession );
/**
 *  @brief Set the target partial frequencies and amplitudes
 *  @param m Received OSC message to parse
 *  @param sSession Pointer to the current user session
 */
void setTargetPartials      ( const osc::ReceivedMessage& m, TargetPtr sTarget );
void setLoopRegion          ( const osc::ReceivedMessage& m, TargetPtr sTarget );
void setMarkers             ( const osc::ReceivedMessage& m, TargetPtr sTarget );


#endif /* defined(__Orchids__oscTarget__) */
