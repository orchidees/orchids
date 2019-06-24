//
//  oscExport.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscExport.h"


void exportSolutions(const osc::ReceivedMessage& m, Session* sSession)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *libPath, *outputDir;
    args >> libPath >> outputDir >> osc::EndMessage;
    PopulationPtr solutionSet = sSession->getSolution();
    if (solutionSet->getSolutions().empty())
    {
        sendOSCMessageError("Export::getSolutions - No solutions");
        return;
    }

    vector<string> soundfiles;
    if (!sSession->getIsMultiTarget())
    {
        ExportSound e(sSession, libPath);
        try {
            e.exportSolutionSet(solutionSet, outputDir);
        } catch (const char* e) {
            sendOSCMessageError(e);
            return;
        }
        boost::filesystem::path output(outputDir);
        boost::filesystem::path dir = output.parent_path();
        for (int i = 0; i < solutionSet->getNbSolutions(); i++)
        {
            string name = dir.string() + "/" + to_string(i) + ".wav";
            soundfiles.push_back(name);
        }
    }
    else
    {
        ExportSound e(sSession, libPath);
        try {
            e.exportMultiTargetSolution(solutionSet, outputDir, boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegments());
        } catch (const char* e) {
            sendOSCMessageError(e);
            return;
        }
        boost::filesystem::path output(outputDir);
        boost::filesystem::path dir = output.parent_path();
        string name = dir.string() + "/MultiTarget.wav";
        soundfiles.push_back(name);
    }
    
    sendOSCSoundNames("/Orchids/reply/exportOneWavSolution", soundfiles);
}

void exportOneWavSolution ( const osc::ReceivedMessage& m, Session* sSession )
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *libPath, *outputFile;
    int         index;
    args >> libPath >> index >> outputFile >> osc::EndMessage;
    if (sSession->getSolution()->getSolutions().empty())
    {
        sendOSCMessageError("Export::getOneWavSolution - No solutions");
        return;
    }

    ExportSound     e(sSession, libPath);
    SolutionPtr     solution = sSession->getSolution()->getSolution(index);
    try {
        e.exportSingleSolution(solution, outputFile);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    sendOSCMessageOK();
}

void exportOneSolution( const osc::ReceivedMessage& m, Session* sSession )
{
    if (sSession->getSolution()->getSolutions().empty())
    {
        sendOSCMessageError("Export::getOneSolution - No solutions");
        return;
    }
    
    int i, j;
    float resolution;
    switch (sSession->getProduction()->getResolution())
    {
        case 1:
            resolution = 0;
            break;
        case 2:
            resolution = 50;
            break;
        case 4:
            resolution = 25;
            break;
        case 8:
            resolution = 12;
            break;
    }
    if (!sSession->getIsMultiTarget())
    {
        osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
        int index;
        args >> index >> osc::EndMessage;
        
        // Send solution infos
        SolutionPtr                 curSol = sSession->getSolution()->getSolution(index);
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        p << osc::BeginMessage( "/Orchids/reply/getSolutionInfo" ) << index << curSol->getNbInstruments() << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
        
        // Send each individuals of the solution
        float tDuration = sSession->getTarget()->getFeaturesList().duration;
        for (i = 0; i < curSol->getNbInstruments(); i++)
        {
            p.Clear();
            p << osc::BeginMessage( "/Orchids/reply/getSolution" ) << i;
            
            IndividualPtr   ind             = curSol->getIndividual(i);
            int             id              = ind->getInstrument();
            int             microtonality   = ind->getMicrotonality();
            float           onset           = (float)ind->getOnset() * tDuration / 128.0f;
            float           duration        = (float)ind->getDuration() * tDuration / 128.0f;
            float           cut             = duration == 0 ? tDuration - onset : _min(tDuration - onset, onset + duration);
            float           lfactor         = ind->getOriginalFeature("LoudnessFactor")->operator()(0);
            
            // If neutral individual, send nothing
            if (id == sSession->getKnowledge()->getNeutralID())
                continue;
            else
            {
                Dbt key;
                init_dbt(&key, &id, sizeof(int));
                symbolics s;
                sSession->getKnowledge()->getBDBConnector()->getSymbolics("symbolics", &key, s);
                if (cut > s.duration) cut = s.duration;
                p << s.instrument << onset << s.MIDIcent + (microtonality * resolution) << s.playingStyle << s.dynamics << s.duration << s.file << microtonality * resolution << cut << lfactor << osc::EndMessage;
            }
            transmitSocket.Send( p.Data(), p.Size() );
        }
        // Finish by END
        sendOSCMessage("/Orchids/reply/getSolution", "END");
    }
    else
    {
        PopulationPtr   finalPop    = sSession->getSolution();
        vector<int>     events      = boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegments();
        float           tDuration   = sSession->getTarget()->getFeaturesList().duration;
        // Send solution infos, can be extended if multiple solutions in multi target mode
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        p << osc::BeginMessage( "/Orchids/reply/getSolutionInfo" ) << 1 << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
        // Send each segment 
        for (i = 0; i < finalPop->getNbSolutions(); i++)
        {
            SolutionPtr curSol      = finalPop->getSolution(i);
            float       duration    = boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegmentDuration(i);
            float       shift       = 0;
            if (i > 0)
                shift = events[i - 1] * tDuration / boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSignalSize();

            p.Clear();
            p << osc::BeginMessage( "/Orchids/reply/getSegment" ) << i << shift << shift + duration << osc::EndMessage;
            transmitSocket.Send( p.Data(), p.Size() );
            // Send each individuals of the segment
            for (j = 0; j < curSol->getNbInstruments(); j++)
            {
                p.Clear();
                p << osc::BeginMessage( "/Orchids/reply/getSolution" ) << j;
                
                IndividualPtr   ind             = curSol->getIndividual(j);
                int             id              = ind->getInstrument();
                int             microtonality   = ind->getMicrotonality();
                float           onset           = (float)ind->getOnset() * tDuration / 128.0f;
                float           duration        = (float)ind->getDuration() * tDuration / 128.0f;
                float           cut             = duration == 0 ? tDuration - onset : _min(tDuration - onset, onset + duration);
                
                // If neutral individual, send nothing
                if (id == sSession->getKnowledge()->getNeutralID())
                    continue;
                else
                {
                    Dbt key;
                    init_dbt(&key, &id, sizeof(int));
                    symbolics s;
                    sSession->getKnowledge()->getBDBConnector()->getSymbolics("symbolics", &key, s);
                    if (cut > s.duration) cut = s.duration;
                    p << s.instrument << shift + onset << s.MIDIcent + (microtonality * resolution) << s.playingStyle << s.dynamics << s.duration << s.file << microtonality * resolution << cut << osc::EndMessage;
                }
                transmitSocket.Send( p.Data(), p.Size() );
            }
        }
        // Finish by END
        sendOSCMessage("/Orchids/reply/getSolution", "END");
    }
}

void getAllSolutionsDistancesFromTarget ( const osc::ReceivedMessage& m, Session* sSession )
{
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    vector<string> descriptors;
    while (arg != m.ArgumentsEnd())
    {
        const char* desc = (arg++)->AsString();
        descriptors.push_back(desc);
    }

    if (sSession->getSolution()->getSolutions().empty())
    {
        sendOSCMessageError("getAllSolutionsDistancesFromTarget::Error - No solutions");
        return;
    }
    
    int             i, j;
    matrixPtr       distances = sSession->getSolution()->getCriteria();
    vector<string>  criterias = sSession->getCriteriaList();
    vector<int>     pos(descriptors.size(), -1);
    for (i = 0; i < descriptors.size(); i++)
        for (j = 0; j < criterias.size(); j++)
            if (criterias[j] == descriptors[i])
                pos[i] = j;

    for (i = 0; i < pos.size(); i++)
    {
        if (pos[i] == -1)
        {
            string err = "getAllSolutionsDistancesFromTarget::Error - Criteria " + descriptors[i] + " not found";
            sendOSCMessageError(err.c_str());
            return;
        }
    }
    
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    for (i = 0; i < pos.size(); i++)
    {
        float* col = distances->getCol(pos[i]);
        p << osc::BeginMessage( "/Orchids/reply/getAllSolutionsDistancesFromTarget" ) << descriptors[i].c_str();
        for (j = 0; j < distances->rows(); j++)
            p << col[j];
        p << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
        free(col);
        p.Clear();
    }
    sendOSCMessage("/Orchids/reply/getAllSolutionsDistancesFromTarget", "END");
}

