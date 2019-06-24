//
//  oscReply.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscReply.h"


void sendOSCMessageOK()
{
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    
    p << osc::BeginMessage( "/Orchids/reply/OK" ) << osc::EndMessage;
    
    transmitSocket.Send( p.Data(), p.Size() );
}

void sendOSCMessageError(const char* e)
{
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    
    p << osc::BeginMessage( "/Orchids/reply/Error" ) << e << osc::EndMessage;
    
    transmitSocket.Send( p.Data(), p.Size() );
}

void OSCProcessSubVec( const char* addr, vector<string>& subVec )
{
    int i;
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char buffer[2048];
    osc::OutboundPacketStream p( buffer, 2048 );
    p << osc::BeginMessage( addr );// << (int)subVec.size();
    
    for (i = 0; i < subVec.size(); i++)
        p << subVec[i].c_str();
    p << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void sendOSCSoundNames( const char* addr, vector<string>& soundNames )
{
    //Split soundNames into N sub-vectors of same size
    int offset = 0, i;
    vector<string> subVec;
    
    for (i = 0; i < soundNames.size(); i++)
    {
        offset += soundNames[i].size();
        if (offset < 1024)
            subVec.push_back(soundNames[i]);
        else
        {
            //Send sub-vector
            OSCProcessSubVec(addr, subVec);
            offset = 0;
            subVec.clear();
            i--;
        }
    }
    //Send last sub-vector
    OSCProcessSubVec(addr, subVec);
    
    //Send "END" message
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char buffer[2048];
    osc::OutboundPacketStream p( buffer, 2048 );
    p << osc::BeginMessage( addr ) << "END" << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void sendOSCMessage( const char* addr, string msg )
{
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char buffer[2048];
    osc::OutboundPacketStream p( buffer, 2048 );
    p << osc::BeginMessage( addr ) << msg.c_str() << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}