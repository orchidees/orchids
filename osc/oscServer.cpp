//
//  oscServer.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 16/04/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscServer.h"


OSCListener::OSCListener(Session* sessObj)
{
    try
    {
        s           = new UdpListeningReceiveSocket( IpEndpointName(IpEndpointName::ANY_ADDRESS, LOCAL_PORT), this);
        sSession    = sessObj;
    }
    catch (std::runtime_error& e)
    {
        printf("OSC error: %s\n", e.what());
        exit(1);
    }
}

OSCListener::~OSCListener()
{
    delete s;
}

void OSCListener::startThread()
{
    m_Thread = boost::thread(&OSCListener::Run, this);
}

void OSCListener::Run()
{
    printf("Running OSC server\n");
    s->Run();
}

void OSCListener::Break()
{
    printf("Exit OSC server\n");
    s->AsynchronousBreak();
}

void OSCListener::Stop()
{
    Break();
	sleep(0.5);
    sendOSCMessage("/Orchids/reply", "exit");
}

void OSCListener::join()
{
    m_Thread.join();
}

void OSCListener::ProcessMessage(const osc::ReceivedMessage&    m,
                                 const IpEndpointName&          remoteEndpoint )
{
    try{
        //---------------------ABORT---------------------
        /*
        if( strcmp( m.AddressPattern(), "/Orchids/abort" ) == 0 )
        {
            // Kill orchesration process
            orchestrate_process->interrupt();
        }
        */
        //---------------------INIT---------------------
        if( strcmp( m.AddressPattern(), "/Orchids/init" ) == 0 )
        {
            //initSession(m, sSession);
        }
        //---------------------STOP---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/stop" ) == 0 )
        {
            sSession->exitSession();
        }
        //---------------------SET CRITERIA---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/setCriteria" ) == 0 )
        {
            setCriteria(m, sSession);
        }
        //---------------------SET SEARCH MODE---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/setSearchMode" ) == 0 )
        {
            setSearchMode(m, sSession);
        }
        //---------------------ORCHESTRATE---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/orchestrate" ) == 0 )
        {
            //orchestrate_process = new boost::thread(orchestrate, m, sSession);
            orchestrate(m, sSession);
        }
        //---------------------AUTOPILOT---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/autoPilot" ) == 0 )
        {
            launchAutoPilot(m, sSession);
        }
        //---------------------ADD SOUND DIRECTORIES---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/addSoundDirectories" ) == 0 )
        {
            addSoundDirectories(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------RECOVER DATABASE BACKUP---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/recoverDBBackup" ) == 0 )
        {
            addSoundDirectories(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------REMOVE SOUND DIRECTORY---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/removeSoundDirectory" ) == 0 )
        {
            removeSoundDirectory(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------REFRESH SOUND DIRECTORIES---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/refreshSoundDirectories" ) == 0 )
        {
            refreshSoundDirectories(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------REFRESH SOUND DIRECTORIES---------------------
        else if( strcmp( m.AddressPattern(), "/Orchids/reanalyzeSoundDirectory" ) == 0 )
        {
            reanalyzeSoundDirectory(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET SYMBOLICS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getSymbolics" ) == 0 )
        {
            getSymbolics(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET SOUNDS QUERY---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getSoundsQuery" ) == 0 )
        {
            getSoundsQuery(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET DESCRIPTOR MIN MAX---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getDescriptorMinMax" ) == 0 )
        {
            getDescriptorMinMax(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET MULTIPLE DESCRIPTOR MIN MAX---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getMultipleDescriptorMinMax" ) == 0 )
        {
            getMultipleDescriptorMinMax(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET SINGLE---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getSingle" ) == 0 )
        {
            getSingle(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------GET INDEX LIST---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getIndexList" ) == 0 )
        {
            getIndexList(m, sSession->getKnowledge()->getBDBConnector());
        }
        //---------------------SET RESOLUTION---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setResolution" ) == 0 )
        {
            setResolution(m, sSession->getProduction());
        }
        //---------------------SET ORCHESTRA---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setOrchestra" ) == 0 )
        {
            setOrchestra(m, sSession->getProduction());
        }
        //---------------------SET FILTERS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setFilters" ) == 0 )
        {
            setFilters(m, sSession->getProduction());
        }
        //---------------------SET FILTERS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getNumFiltered" ) == 0 )
        {
            getNumFiltered(m, sSession);
        }
        //---------------------RESET FILTERS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/resetFilters" ) == 0 )
        {
            resetFilters(m, sSession->getProduction());
        }
        //---------------------SET HARMONIC FILTERING---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setHarmonicFiltering" ) == 0 )
        {
            setHarmonicFiltering(m, sSession->getProduction());
        }
        //---------------------SET STATIC MODE---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setStaticMode" ) == 0 )
        {
            setStaticMode(m, sSession->getProduction());
        }
        //---------------------GET SCORE ORDER---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getScoreOrder" ) == 0 )
        {
            getScoreOrder(m, sSession->getProduction());
        }
        //---------------------GET USER INSTRUS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getInstruments" ) == 0 )
        {
            getUserInstrus(m, sSession->getProduction());
        }
        //---------------------GET USER PLAYING STYLES---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getPlayingStyles" ) == 0 )
        {
            getUserPlayingStyles(m, sSession->getProduction());
        }
        //---------------------GET SOURCES---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getSources" ) == 0 )
        {
            getSources(m, sSession->getProduction());
        }
        //---------------------SET SOUNDFILE---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setSoundfile" ) == 0 )
        {
            setSoundfile(m, sSession);
        }
        //---------------------SET ANALYSIS PARAMETERS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setAnalysisParameters" ) == 0 )
        {
            setAnalysisParams(m, sSession);
        }
        //---------------------SET ABSTRACT TARGET---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setAbstractTarget" ) == 0 )
        {
            setAbstractTarget(m, sSession);
        }
        //---------------------SET MULTI TARGET---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setMultiTarget" ) == 0 )
        {
            setMultiTarget(m, sSession);
        }
        //---------------------SET MULTI TARGET---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/isInBase" ) == 0 )
        {
            isInBase(m, sSession->getProduction());
        }
        //---------------------SET TARGET SELECTION---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setTargetSelection" ) == 0 )
        {
            setLoopRegion(m, sSession->getTarget());
        }
        //---------------------SET MULTI TARGET MARKERS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setMarkers" ) == 0 )
        {
            setMarkers(m, sSession->getTarget());
        }
        //---------------------SET TARGET DURATION---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setTargetDuration" ) == 0 )
        {
            setTargetDuration(m, sSession->getTarget());
        }
        //---------------------SET TARGET FEATURE---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setTargetFeature" ) == 0 )
        {
            setTargetFeature(m, sSession->getTarget());
        }
        //---------------------GET TARGET FEATURE---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getTargetSingle" ) == 0 )
        {
            getTargetSingle(m, sSession->getTarget());
        }
        //---------------------GET TARGET SYMBOLICS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getTargetSymbolics" ) == 0 )
        {
            getTargetSymbolics(m, sSession->getTarget());
        }
        //---------------------GET TARGET SYMBOLICS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/freezeTargetFeature" ) == 0 )
        {
            freezeFeature(m, sSession->getTarget());
        }
        //---------------------SET TARGET PARTIALS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setTargetPartials" ) == 0 )
        {
            setTargetPartials(m, sSession->getTarget());
        }
        //---------------------SET SEARCH PARAMETER---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setSearchParameter" ) == 0 )
        {
            setSearchParameter(m, sSession->getSearch());
        }
        //---------------------SET SOLUTION AS TARGET---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/setSolutionAsTarget" ) == 0 )
        {
            setSolutionAsTarget(m, sSession);
        }
        //---------------------EXPORT SOUND SOLUTIONS---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/exportSoundSolutions" ) == 0 )
        {
            exportSolutions(m, sSession);
        }
        //---------------------EXPORT ONE WAV SOLUTION---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/exportOneWavSolution" ) == 0 )
        {
            exportOneWavSolution(m, sSession);
        }
        //---------------------EXPORT ONE SOLUTION---------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getSolution" ) == 0 )
        {
            exportOneSolution(m, sSession);
        }
        //---------------------GET DISTANCES----------------------------
        else if ( strcmp( m.AddressPattern(), "/Orchids/getAllSolutionsDistancesFromTarget" ) == 0 )
        {
            getAllSolutionsDistancesFromTarget(m, sSession);
        }
    }
    catch( osc::Exception& e )
    {
        // any parsing errors such as unexpected argument types, or
        // missing arguments get thrown as exceptions.
        std::cout << "error while parsing message: " << m.AddressPattern() << ": " << e.what() << "\n";
        sendOSCMessageError(e.what());
    }
}