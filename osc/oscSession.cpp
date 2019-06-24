//
//  oscSession.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscSession.h"


void initSession(const osc::ReceivedMessage& m, Session* sSession)
{
    try {
        sSession->constructDefaultSession();
        sendOSCMessageOK();
    } catch (DbException e) {
        cout << e.what() << endl;
        sendOSCMessageError(e.what());
    }
}

void setSearchMode ( const osc::ReceivedMessage& m, Session* sSession )
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* s;
    args >> s >> osc::EndMessage;
    string mode = s;
    
    map<string, boost::any> params = sSession->getSearch()->getSearchParameters();
    // Check and set search mode
    if (mode == "Genetic")
        sSession->setSearch(SearchPtr(new SearchGenetic(sSession)));
    else if (mode == "Optimal Warping")
        sSession->setSearch(SearchPtr(new SearchOptimalWarping(sSession)));
    else
    {
        string e = "Session::setSearchMode, mode '" + mode + "' is unknown";
        sendOSCMessageError(e.c_str());
        return;
    }
    sSession->getSearch()->setSearchParameters(params);
    sendOSCMessageOK();
}

void setCriteria(const osc::ReceivedMessage& m, Session* sSession)
{
    vector<string> critlist;
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    while (arg != m.ArgumentsEnd())
    {
        const char* criteria = (arg++)->AsString();
        string crit = criteria;
        if (crit != "null")
            critlist.push_back(crit);
    }
    try {
        sSession->setCriteriaList(critlist);
    } catch (const char* e) {
        cout << e << endl;
        sendOSCMessageError(e);
        return;
    }
    sendOSCMessageOK();
}

void orchestrate(const osc::ReceivedMessage& m, Session* sSession)
{
    try {
        sSession->launchSearch();
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        int nbSolutions;
        if (sSession->getIsMultiTarget())
            nbSolutions = 1;
        else
            nbSolutions = sSession->getSolution()->getNbSolutions();
        p << osc::BeginMessage( "/Orchids/reply/orchestrate" ) << nbSolutions << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
    } catch (const char* e) {
        cout << e << endl;
        sendOSCMessageError(e);
        return;
    }
}

void launchAutoPilot( const osc::ReceivedMessage& m, Session* sSession )
{
    int                         i;
    int                         res = sSession->autoPilot();
    vector<string>              desc = sSession->getCriteriaList();
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    switch (res) {
        case 0:
            sendOSCMessageError("AutoPilot::Error - Failed");
            break;
            
        case 1:
            p << osc::BeginMessage( "/Orchids/reply/autoPilot" );
            for (i = 0; i < desc.size(); i++)
                p << desc[i].c_str();
            p << "SearchGenetic" << 1 << osc::EndMessage;
            transmitSocket.Send( p.Data(), p.Size() );
            break;
            
        case 2:

            p << osc::BeginMessage( "/Orchids/reply/autoPilot" );
            for (i = 0; i < desc.size(); i++)
                p << desc[i].c_str();
            p << "SearchOptimalWarping" << 0 << osc::EndMessage;
            transmitSocket.Send( p.Data(), p.Size() );
            break;
            
        case 3:
            p << osc::BeginMessage( "/Orchids/reply/autoPilot" );
            for (i = 0; i < desc.size(); i++)
                p << desc[i].c_str();
            p << "SearchGenetic" << 0 << "MultiTarget";
            // Get and normalize events between 0 and 1
            vector<int> eventsTime = boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegments();
            int signalSize = sSession->getTarget()->getSoundSize();
            for (i = 0; i < eventsTime.size(); i++)
                p << (float)((float)eventsTime[i] / (float)signalSize);
            p << osc::EndMessage;
            transmitSocket.Send( p.Data(), p.Size() );
            break;
    }
}