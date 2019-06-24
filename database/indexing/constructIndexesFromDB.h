//
//  constructIndexesFromDB.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__constructIndexesFromDB__
#define __Orchids__constructIndexesFromDB__

#include "Orchids.h"

#define indexTreePtr boost::shared_ptr<indexTree>


class KnowledgeBDB;
class indexTree;

/**
 *  @brief Creating indexes for a particular descriptors list
 *  @param knowledge Pointer to knowledge
 *  @param descName Name of the descriptor to index
 */
indexTreePtr constructIndexesFromDB(
                                    KnowledgeBDBPtr  knowledge,
                                    string           descName);

#endif /* defined(__Orchids__constructIndexesFromDB__) */
