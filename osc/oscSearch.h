//
//  oscSearch.h
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__oscSearch__
#define __Orchids__oscSearch__

#include "Orchids.h"

class Search;

/**
 *  @brief Modify parameters of the current search algorithm
 *  @param m Received OSC message to parse
 *  @param sSearch Pointer to the current search
 */
void setSearchParameter( const osc::ReceivedMessage& m, SearchPtr sSearch );

#endif /* defined(__Orchids__oscSearch__) */
