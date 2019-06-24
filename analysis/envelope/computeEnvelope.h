//
//  computeEnvelope.h
//  Orchids
//
//  Created by Antoine Bouchereau on 05/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__computeEnvelope__
#define __Orchids__computeEnvelope__

#include "Orchids.h"

envelope computeEnvelope(
                         vector<float>& signal,
                         int            sRate,
                         vector<float>& env);

#endif /* defined(__Orchids__computeEnvelope__) */
