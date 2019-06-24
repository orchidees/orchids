//
//  oscProduction.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscProduction.h"


void setResolution(const osc::ReceivedMessage& m, ProductionPtr sProduction)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    int resolution;
    args >> resolution >> osc::EndMessage;
    try {
        sProduction->setResolution(resolution);
    } catch (const char* e) {
        sendOSCMessageError(e);
    }
    sProduction->emptyVariableDomains();
    sProduction->needInit();
    sendOSCMessageOK();
}

void setOrchestra(const osc::ReceivedMessage& m, ProductionPtr sProduction)
{
    vector<string> instrus;
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    while (arg != m.ArgumentsEnd())
    {
        const char* instrument = (arg++)->AsString();
        string inst = instrument;
        instrus.push_back(inst);
    }
    sProduction->setInstruments(instrus);
    sProduction->emptyVariableDomains();
    sProduction->needInit();
    sendOSCMessageOK();
}

void isInBase ( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    int i;
    vector<string> result;
    vector<string> userInst = boost::dynamic_pointer_cast<ProductionOrchestra>(sProduction)->getUserInstruments();
    osc::ReceivedMessage::const_iterator    arg     = m.ArgumentsBegin();
    // Change filters
    while (arg != m.ArgumentsEnd())
    {
        const char* name        = (arg++)->AsString();
        
        for (i = 0; i < userInst.size(); i++)
            if (userInst[i] == name) {result.push_back("1"); break;}
        if (i == userInst.size())
            result.push_back("0");
    }
    sendOSCSoundNames("/Orchids/reply/isInBase", result);
}

void setFilters(const osc::ReceivedMessage& m, ProductionPtr sProduction)
{
    // Reset filters
    sProduction->emptyVariableDomains();
    sProduction->resetFilters();
    sProduction->constructBaseFilters();
    
    int i;
    try
    {
        // Get filters
        vector<boost::shared_ptr<Filters> >     filters = sProduction->getFilters();
        osc::ReceivedMessage::const_iterator    arg     = m.ArgumentsBegin();
        // Change filters
        while (arg != m.ArgumentsEnd())
        {
            const char* type        = (arg++)->AsString();
            const char* descriptor  = (arg++)->AsString();
            for (i = 0; i < filters.size(); i++)
            {
                if (strcmp(filters[i]->getName().c_str(), descriptor) == 0)
                {
                    if (filters[i]->getAttribute() == "string")
                    {
                        const char* value = (arg++)->AsString();
                        vector<string> vals;
                        vals.push_back(value);
                        boost::static_pointer_cast<FiltersSymbolic>(filters[i])->setValuesList(vals);
                        filters[i]->setMode("free");
                    }
                    else if (filters[i]->getAttribute() == "float")
                    {
                        float lower = (arg++)->AsFloat();
                        float upper = (arg++)->AsFloat();
                        boost::static_pointer_cast<FiltersSpectral>(filters[i])->setValuesRange(pair<float, float>(lower, upper));
                        filters[i]->setMode(type);
                    }
                    else// if (filters[i].getType() == "Temporal")
                    {
                        // Do nothing
                    }
                }
            }
        }
        // Apply filters
        sProduction->setFilters(filters);
        sProduction->needInit();
        sendOSCMessageOK();
    }
    catch( osc::Exception& e )
    {
        throw e;
    }
}

void getNumFiltered(const osc::ReceivedMessage& m, Session *sSession)
{
    ProductionPtr   sProduction = sSession->getProduction();
    BDBTxn          *txn = sSession->getKnowledge()->getBDBConnector();
    vector<Query>   queries;
    string          desc;
    int             i;
    
    try
    {
        // Get filters
        vector<boost::shared_ptr<Filters> >     filters = sProduction->getFilters();
        osc::ReceivedMessage::const_iterator    arg     = m.ArgumentsBegin();
        // Change filters
        while (arg != m.ArgumentsEnd())
        {
            const char* type        = (arg++)->AsString();
            const char* descriptor  = (arg++)->AsString();
            for (i = 0; i < filters.size(); i++)
            {
                if (strcmp(filters[i]->getName().c_str(), descriptor) == 0)
                {
                    if (filters[i]->getAttribute() == "string")
                    {
                        const char *value = (arg++)->AsString();
                        string val = value;
                        Query q("REGEXP", descriptor, val);
                        queries.push_back(q);
                    }
                    else if (filters[i]->getAttribute() == "float")
                    {
                        float lower = (arg++)->AsFloat();
                        float upper = (arg++)->AsFloat();
                        Query q("BETWEEN", descriptor, pair<float, float>(lower, upper));
                        queries.push_back(q);
                    }
                    else// if (filters[i].getType() == "Temporal")
                    {
                        // Do nothing
                    }
                }
            }
        }
        //Run getSoundsQuery
        vector<int>     soundIDs;
        vector<string>  soundNames;
        if (!txn->getSoundsQuery(queries, soundIDs))
        {
            sendOSCMessageError("Database error during querying");
            return;
        }
        sendOSCMessage("/Orchids/reply/getNumFiltered", " " + to_string(soundIDs.size()));
    }
    catch( osc::Exception& e )
    {
        throw e;
    }
}

void resetFilters( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    sProduction->emptyVariableDomains();
    sProduction->resetFilters();
    sProduction->constructBaseFilters();
    sProduction->needInit();
    sendOSCMessageOK();
}

void setHarmonicFiltering(const osc::ReceivedMessage& m, ProductionPtr sProduction)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    int harmFilt;
    args >> harmFilt >> osc::EndMessage;
    switch (harmFilt)
    {
        case 0:
            sProduction->setHarmonicFiltering(false);
            break;
            
        case 1:
            sProduction->setHarmonicFiltering(true);
            break;
            
        default:
            break;
    }
    sProduction->emptyVariableDomains();
    sProduction->needInit();
    sendOSCMessageOK();
}

void setStaticMode(const osc::ReceivedMessage& m, ProductionPtr sProduction)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    int sMode;
    args >> sMode >> osc::EndMessage;
    switch (sMode)
    {
        case 0:
            sProduction->setStaticMode(false);
            break;
            
        case 1:
            sProduction->setStaticMode(true);
            break;
            
        default:
            break;
    }
    sProduction->emptyVariableDomains();
    sProduction->needInit();
    sendOSCMessageOK();
}

void getScoreOrder( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    vector<string> score = sProduction->getScoreOrder();
    sendOSCSoundNames("/Orchids/reply/getScoreOrder", score);
}

void getUserInstrus ( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    vector<string> userInst = boost::dynamic_pointer_cast<ProductionOrchestra>(sProduction)->getUserInstruments();
    sendOSCSoundNames("/Orchids/reply/getInstruments", userInst);
}

void getUserPlayingStyles ( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    vector<string> userInst = boost::dynamic_pointer_cast<ProductionOrchestra>(sProduction)->getUserPlayingStyles();
    sendOSCSoundNames("/Orchids/reply/getPlayingStyles", userInst);
}

void getSources ( const osc::ReceivedMessage& m, ProductionPtr sProduction )
{
    vector<string> userInst = boost::dynamic_pointer_cast<ProductionOrchestra>(sProduction)->getSources();
    sendOSCSoundNames("/Orchids/reply/getSources", userInst);
}
