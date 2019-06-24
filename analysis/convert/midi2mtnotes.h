//
//  midi2mtnotes.h
//  Orchids
//
//  Created by Antoine Bouchereau on 20/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__midi2mtnotes__
#define __Orchids__midi2mtnotes__

#include "Orchids.h"

float hz2midi(float hz);

float midi2hz(float midi);

vector<string> midi2mtnotes(vector<float> &mtmidi);

#endif /* defined(__Orchids__midi2mtnotes__) */
