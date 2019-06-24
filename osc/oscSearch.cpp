//
//  oscSearch.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscSearch.h"


void setSearchParameter(const osc::ReceivedMessage& m, SearchPtr sSearch)
{
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    while (arg != m.ArgumentsEnd())
    {
        string name = (arg++)->AsString();
        if (name == "selectiveMutation")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setSelectiveMutation(true) : sSearch->setSelectiveMutation(false);
        }
        else if (name == "globalProba")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setGlobalProba(true) : sSearch->setGlobalProba(false);
        }
        else if (name == "harmonicFix")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setHarmonicFix(true) : sSearch->setHarmonicFix(false);
        }
        else if (name == "localOptim")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setLocalOptim(true) : sSearch->setLocalOptim(false);
        }
        else if (name == "adaptiveOptim")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setAdaptiveOptim(true) : sSearch->setAdaptiveOptim(false);
        }
        else if (name == "TSIndex")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setTSIndex(true) : sSearch->setTSIndex(false);
        }
        else if (name == "optimFullPopulation")
        {
            int val = (arg++)->AsInt32();
            (val == 1) ? sSearch->setOptimFullPopulation(true) : sSearch->setOptimFullPopulation(false);
        }
        else if (name == "harmonicFixFrequency")
            sSearch->setHarmonicFixFreq((arg++)->AsInt32());
        else if (name == "adaptiveOptimFrequency")
            sSearch->setAdaptiveOptimFreq((arg++)->AsInt32());
        else if (name == "setSparsenessPartials")
             sparseness_partials = (arg++)->AsFloat();
        else if (name == "optimPopSubsampling")
            sSearch->setOptimPopSubsampling((arg++)->AsInt32());
        else if (name == "optimGenIndividuals")
            sSearch->setOptimGenIndividuals((arg++)->AsInt32());
        else if (name == "optimGenSubsampling")
            sSearch->setOptimGenSubsampling((arg++)->AsInt32());
        else if (name == "popConstrained")
            sSearch->setPopConstrained((arg++)->AsFloat());
        else if (name == "popSparsity")
            sSearch->setPopSparsity((arg++)->AsFloat());
        else
        {
            try {
                sSearch->setParameter(name, (arg++)->AsInt32());
            } catch (const char* e) {
                cout << e << endl;
            }
        }
    }
    sendOSCMessageOK();
}