//
//  oscTarget.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscTarget.h"


void setSoundfile(const osc::ReceivedMessage& m, Session* sSession)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* filename;
    args >> filename >> osc::EndMessage;
    string file = filename;
    // Retrieve previous features
    sqlStruct oldFeatures = sSession->getTarget()->getFeaturesList();
    // Apply new sound file
    sSession->setTarget(TargetPtr(new TargetSound(sSession, file)));
    try {
        // Compute new target features and keep previous freezed descriptors
        sSession->computeTargetFeatures(oldFeatures);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    sSession->getProduction()->emptyVariableDomains();
    sSession->getProduction()->needInit();
    sendOSCMessageOK();
}

void setAnalysisParams(const osc::ReceivedMessage& m, Session* sSession)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* configFile;
    args >> configFile >> osc::EndMessage;
    string file = configFile;
    // Retrieve previous features
    sqlStruct oldFeatures = sSession->getTarget()->getFeaturesList();
    try {
        sSession->getTarget()->setAnalysisParameters(file);
        sSession->getTarget()->computeFeatures(oldFeatures);
    } catch (const char * e) {
        sendOSCMessageError(e);
        return;
    }
    sSession->getProduction()->emptyVariableDomains();
    sSession->getProduction()->needInit();
    sendOSCMessageOK();
}

void setAbstractTarget(const osc::ReceivedMessage& m, Session* sSession)
{
    // Retrieve previous features
    sqlStruct oldFeatures = sSession->getTarget()->getFeaturesList();
    // Apply new abstract target
    sSession->setTarget(TargetPtr(new TargetAbstract(sSession)));
    try {
        // Compute new target features and keep previous freezed descriptors
        sSession->getTarget()->copyFeatures(oldFeatures);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    sSession->getProduction()->emptyVariableDomains();
    sSession->getProduction()->needInit();
    sendOSCMessageOK();
}

void setMultiTarget(const osc::ReceivedMessage& m, Session* sSession)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* filename;
    args >> filename >> osc::EndMessage;
    string file = filename;
    // Retrieve previous features
    sqlStruct oldFeatures = sSession->getTarget()->getFeaturesList();
    // Apply new sound file
    sSession->setTarget(TargetPtr(new MultiTarget(sSession, file)));
    try {
        // Compute new target features and keep previous freezed descriptors
        sSession->computeTargetFeatures(oldFeatures);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    sSession->getProduction()->emptyVariableDomains();
    sSession->getProduction()->needInit();
    
    // Send multiTarget markers
    int                         signalSize = sSession->getTarget()->getSoundSize();
    vector<int>                 events = boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegments();
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    p << osc::BeginMessage( "/Orchids/reply/multiTargetMarkers" );
    for (int i = 0; i < events.size(); i++)
        p << (float)((float)events[i] / (float)signalSize);
    p << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void setTargetDuration(const osc::ReceivedMessage& m, TargetPtr sTarget)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    float duration;
    args >> duration >> osc::EndMessage;
    sTarget->setDuration(duration);
    sendOSCMessageOK();
}

void setTargetFeature(const osc::ReceivedMessage& m, TargetPtr sTarget)
{
    int j;
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* descriptor;
    args >> descriptor;
    string desc = descriptor;
    
    // Retrieve time serie
    matrixPtr values(new matrix(1, 128));
    for (j = 0; j < 128; j++)
    {
        float val;
        args >> val;
        values->operator()(j) = val;
    }
    args >> osc::EndMessage;
    
    try {
        sTarget->modifyFeature(descriptor, values);
    } catch (const char* e) {
        cout << e << endl;
        sendOSCMessageError(e);
        return;
    }
    sendOSCMessageOK();
}

void getTargetSingle ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    int i;
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* descriptor;
    args >> descriptor >> osc::EndMessage;
    string desc = descriptor;

    // Get descriptors list
    vector<feature<float> >     features = sTarget->getFeaturesList().descriptors;
    float                       mean = 0, stdDev = 0, max = 0, min = 0;
    matrixPtr                   values;
    for (i = 0; i < features.size(); i++)
    {
        // Descriptor found
        if (features[i].name == desc)
        {
            mean    = features[i].mean;
            stdDev  = features[i].stdDev;
            max     = features[i].max;
            min     = features[i].min;
            values  = matrixPtr(new matrix(*features[i].resampled_val));
            break;
        }
    }
    
    // Send denormalized values of descriptor
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    p << osc::BeginMessage( "/Orchids/reply/getTargetSingle" ) << mean << stdDev << sTarget->getFreeze(desc);
    for (i = 0; i < values->size(); i++)
        p << (values->operator()(i) * (max - min) + min); //Dénormalisation
    p << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void getTargetSymbolics ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    sqlStruct                   s = sTarget->getFeaturesList();
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    p << osc::BeginMessage( "/Orchids/reply/getTargetSymbolics" ) << s.dynamics.c_str() << s.note.c_str() << s.duration << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void freezeFeature ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char* feature;
    int freeze;
    args >> feature >> freeze >> osc::EndMessage;
    switch (freeze)
    {
        case 0:
            sTarget->freezeFeature(false, feature);
            break;
            
        case 1:
            sTarget->freezeFeature(true, feature);
            break;
            
        default:
            break;
    }
    sendOSCMessageOK();
}

void setSolutionAsTarget ( const osc::ReceivedMessage& m, Session* sSession)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *libPath;
    int         index;
    args >> libPath >> index >> osc::EndMessage;
    if (sSession->getSolution()->getSolutions().empty())
    {
        sendOSCMessageError("Export::getOneWavSolution - No solutions");
        return;
    }
    
    // Export solution to wav
    string          outputFile = "/tmp/tmpSolutionTarget.wav";
    ExportSound     e(sSession, libPath);
    SolutionPtr     solution = sSession->getSolution()->getSolution(index);
    try {
        e.exportSingleSolution(solution, outputFile);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    /*
    // Apply new target and keep previous freezed descriptors
    sqlStruct oldFeatures = sSession->getTarget()->getFeaturesList();
    sSession->setTarget(TargetPtr(new TargetSound(sSession, outputFile)));
    try {
        sSession->computeTargetFeatures(oldFeatures);
    } catch (const char* e) {
        sendOSCMessageError(e);
        return;
    }
    */
    sendOSCMessageOK();
}

void setTargetPartials ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    int i, j;
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *file;
    args >> file >> osc::EndMessage;
    
    matrixPtr partialsFreq(new matrix(25, 64));
    matrixPtr partialsAmp(new matrix(25, 64));
    ifstream f(file);
    if (!f)
    {
        cout << "Cannot open file" << endl;
        sendOSCMessageError("Cannot open file");
    }
    for (i = 0; i < partialsFreq->rows(); i++)
        for (j = 0; j < partialsFreq->cols(); j++)
        {
            f >> partialsFreq->operator()(i, j);
            partialsAmp->operator()(i, j) = 0.5;
        }
    for (i = 0; i < partialsAmp->rows(); i++)
        for (j = 0; j < partialsAmp->cols(); j++)
            f >> partialsAmp->operator()(i, j);
    f.close();
    
    printf("*** IMPORTED PARTIALS ***\n");
    for (i = 0; i < partialsFreq->rows(); i++)
    {
        for (j = 0; j < partialsFreq->cols(); j++)
            printf("%f (%f) ", partialsFreq->operator()(i, j), partialsAmp->operator()(i, j));
        printf("\n");
    }
    
    sTarget->setPartialsFrequency(partialsFreq);
    sTarget->setPartialsAmplitude(partialsAmp);
    sTarget->emptyHarmonicFilters();
    sTarget->getHarmonicFilters();
    sTarget->getFeaturesList().targetSpec.reset();
    sendOSCMessageOK();
}

void setLoopRegion ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    float begin, end;
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    args >> begin >> end >> osc::EndMessage;
    int signalSize = sTarget->getSoundSize();
    begin   *= (float)signalSize;
    end     *= (float)signalSize;
    int b = floor(begin);
    int e = floor(end);
    if (!sTarget->getComputed())
        sendOSCMessageError("Error::Target Selection - Target is not computed");
    else
    {
        try {
            sTarget->setLoopRegion(b, e);
        } catch (const char* e)
        {
            sendOSCMessageError(e);
        }
        sendOSCMessageOK();
    }
}

void setMarkers ( const osc::ReceivedMessage& m, TargetPtr sTarget )
{
    vector<int> markers;
    int signalSize = sTarget->getSoundSize();
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    if (arg->IsString())
    {
        // Cas setMarkers "none"
        boost::dynamic_pointer_cast<MultiTarget>(sTarget)->setSegments(markers);
        sendOSCMessageOK();
    }
    else
    {
        while (arg != m.ArgumentsEnd())
            markers.push_back((int)((arg++)->AsFloat() * (float)signalSize));
        boost::dynamic_pointer_cast<MultiTarget>(sTarget)->setSegments(markers);
        sendOSCMessageOK();
    }
}

