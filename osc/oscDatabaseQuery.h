//
//  oscDatabaseQuery.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscDatabaseQuery__
#define __Orchids__oscDatabaseQuery__

#include "Orchids.h"

class BDBTxn;

/**
 *  @brief Add a sound directory (recursive) to the database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void addSoundDirectories    ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Recover a previous backup of the database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void recoverDBBackup        ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Remove a sound directory from the database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void removeSoundDirectory   ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Remove a sound directory from the database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void reanalyzeSoundDirectory    ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Refresh a sound directory in the database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void refreshSoundDirectories( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Get symbolic informations from a sound
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getSymbolics           ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Launch a query to perform on database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getSoundsQuery         ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Get min and maximum values of a descriptor
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getDescriptorMinMax    ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Get min and maximum values for multiple descriptors
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getMultipleDescriptorMinMax    ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Get one descriptor from one sound
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getSingle              ( const osc::ReceivedMessage& m, BDBTxn* txn );
/**
 *  @brief Return a list of all values for a field in database
 *  @param m Received OSC message to parse
 *  @param txn Pointer to the current database connector
 */
void getIndexList           ( const osc::ReceivedMessage& m, BDBTxn* txn );

#endif /* defined(__Orchids__oscDatabaseQuery__) */