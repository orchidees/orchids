/**
 *
 *  @file			oscServer.h
 *  @brief			Interface to handle an OSC communication with a client
 *  @version        0.14
 *  @author			Antoine Bouchereau
 *	@date			25-02-2014
 *
 */

#ifndef __Orchids__oscServer__
#define __Orchids__oscServer__

#include "Orchids.h"
#include "oscReply.h"
#include "oscSession.h"
#include "oscDatabaseQuery.h"
#include "oscProduction.h"
#include "oscTarget.h"
#include "oscSearch.h"
#include "oscExport.h"

#define ADDRESS             "127.0.0.1"
#define LOCAL_PORT          3333
#define REMOTE_PORT         3334


class Session;

/**
 *  @class OSCListener
 *
 *  @brief Class that handles the OSC listener thread
 */
class OSCListener : public osc::OscPacketListener
{
public:
    /**
     *  @brief Constructor that creates the OSC listener socket
     *  @param sessObj Pointer to the current session
     */
    OSCListener (Session* sessObj);
    ~OSCListener();
    
    /**
     *  @brief Launch the boost::thread
     */
    void startThread();
    /**
     *  @brief Run the OSC listener
     */
    void Run();
    /**
     *  @brief Stop the OSC listener
     */
    void Break();
    /**
     *  @brief Exit OSC server
     */
    void Stop();
    /**
     *  @brief Main thread joins the OSC thread
     */
    void join();
    
protected:
    UdpListeningReceiveSocket   *s;
    boost::thread               m_Thread;
    Session*                    sSession;
    //boost::thread               *orchestrate_process;
    
    // Implement the required ProcessMessage()
    /**
     *  @brief Virtual function that implements the oscpack library ProcessMessage function. Parse the received OSC messages
     *  @param m Structure that contains the received OSC messages
     *  @param remoteEndPoint Structure that contains network informations about the received OSC messages
     */
    virtual void ProcessMessage( const osc::ReceivedMessage& m,
                                const IpEndpointName& remoteEndpoint );
};

#endif