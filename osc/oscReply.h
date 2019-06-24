//
//  oscReply.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscReply__
#define __Orchids__oscReply__

#include "Orchids.h"

/**
 *  @brief Send to the client split vectors of strings
 *  @param addr Specify to which query we are replying to
 *  @param subVec List of strings
 */
void OSCProcessSubVec( const char* addr, const vector<string>& subVec );
/**
 *  @brief Send a "/Orchids/reply/OK" message to the client
 */
void sendOSCMessageOK();
/**
 *  @brief Send a "/Orchids/reply/Error" message to the client
 */
void sendOSCMessageError( const char* e );
/**
 *  @brief Send to the client the list of sound names found after a query
 *  @param addr Specify to which query we are replying to
 *  @param soundNames List of sound names
 */
void sendOSCSoundNames( const char* addr, vector<string>& soundNames );
/**
 *  @brief Send to the client a message
 *  @param addr Specify to which query we are replying to
 *  @param msg Message to send
 */
void sendOSCMessage( const char* addr, string msg );

#endif /* defined(__Orchids__oscReply__) */
