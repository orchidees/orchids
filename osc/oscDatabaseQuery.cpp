//
//  oscDatabaseQuery.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 24/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "oscDatabaseQuery.h"

char                *findCurrentLog()
{
    DIR             *dp;
    struct dirent   *entry;
    struct stat     statbuf;
    char            *ret = (char *)calloc(512, sizeof(char));
    
    if((dp = opendir(BDB_DIR)) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", BDB_DIR);
        return NULL;
    }
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);
        // Avoid directories
        if(S_ISDIR(statbuf.st_mode))
            continue;
        // Check if this is a log
        if (strncmp("log.", entry->d_name, 4) == 0)
            sprintf(ret, "%s", entry->d_name);
    }
    closedir(dp);
    return ret;
}

bool                checkLogExists(int nb)
{
    DIR             *dp;
    struct dirent   *entry;
    struct stat     statbuf;
    char            logStr[256];
    bool            found = false;
    
    sprintf(logStr, "bkp.log.%d", nb);
    if((dp = opendir(BDB_DIR)) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", BDB_DIR);
        return NULL;
    }
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);
        // Avoid directories
        if(S_ISDIR(statbuf.st_mode))
            continue;
        // Check if this is a log
        if (strcmp(logStr, entry->d_name) == 0)
        {
            found = true;
            break;
        }
    }
    closedir(dp);
    return found;
}

int                 backupLogIndex()
{
    DIR             *dp;
    struct dirent   *entry;
    struct stat     statbuf;
    int             val, maxVal = 0;
    
    if((dp = opendir(BDB_DIR)) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", BDB_DIR);
        return NULL;
    }
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);
        // Avoid directories
        if(S_ISDIR(statbuf.st_mode))
            continue;
        // Check if this is a log
        if (strncmp("bkp.log.", entry->d_name, 8) == 0)
        {
            sscanf(entry->d_name, "bkp.log.%d", &val);
            if (val > maxVal) {maxVal = val;}
        }
    }
    closedir(dp);
    return maxVal;
}

void addSoundDirectories(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    
    // First create a backup of the previous log
    char *logCmd = (char *)calloc(512, sizeof(char));
    char *lastLog = findCurrentLog();
    int logVal = backupLogIndex();
    sprintf(logCmd, "cp %s/%s %s/bkp.log.%d", BDB_DIR, lastLog, BDB_DIR, logVal);
    system(logCmd); free(lastLog); free(logCmd);
    
    bool success = false;
    while (arg != m.ArgumentsEnd())
    {
        const char* path    = (arg++)->AsString();
        const char* inst    = (arg++)->AsString();
        const char* instS   = (arg++)->AsString();
        const char* sourc   = (arg++)->AsString();
        const char* style   = (arg++)->AsString();
        int recurse         = (arg++)->AsInt32();
        string dpath        = path;
        string instr        = inst;
        if (recurse)
            success = txn->addSoundDirectory_OSC_recurse(dpath, sourc, instr, instS);
        else
            success = txn->addSoundDirectory_OSC_style(dpath, sourc, instr, instS, style);
        if (!success)
            break;
    }
    (success) ? sendOSCMessageOK() : sendOSCMessageError("Can't find directory");
}

void recoverDBBackup(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    bool success = false;
    while (arg != m.ArgumentsEnd())
    {
        //int index           = (arg++)->AsInt32();
        // First create a backup of the previous log
        char *logCmd = (char *)calloc(512, sizeof(char));
        char *lastLog = findCurrentLog();
        int logVal = backupLogIndex();
        sprintf(logCmd, "cp %s/bkp.log.%d %s/%s", BDB_DIR, logVal, BDB_DIR, lastLog);
        system(logCmd); free(lastLog); free(logCmd);
    }
    (success) ? sendOSCMessageOK() : sendOSCMessageError("Problem with backup recovery");
}

void removeSoundDirectory(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *path;
    args >> path >> osc::EndMessage;
    try {
        txn->removeSoundDirectory((char*)path);
        sendOSCMessageOK();
    } catch (DbException e) {
        cout << e.what() << endl;
        sendOSCMessageError(e.what());
    }
}

void reanalyzeSoundDirectory(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *path, *source, *instru;
    args >> path >> source >> instru >> osc::EndMessage;
    try {
        txn->removeSoundDirectory((char*)path);
        txn->addSoundDirectory(path, source, instru);
        sendOSCMessageOK();
    } catch (DbException e) {
        cout << e.what() << endl;
        sendOSCMessageError(e.what());
    }
}

void refreshSoundDirectories(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    while (arg != m.ArgumentsEnd())
    {
        const char* path    = (arg++)->AsString();
        string dpath        = path;
        
        vector<string> filenames;
        if (txn->refreshSoundDirectory(dpath, getFileName<string>(dpath), filenames))
            txn->check_sounds_to_delete(getFileName<string>(dpath), filenames);
    }
    sendOSCMessageOK();
}

void getSymbolics(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *name;
    args >> name >> osc::EndMessage;
    
    int index = txn->getIndex(name);
    Dbt key;
    init_dbt(&key, &index, sizeof(int));
    
    symbolics   s;
    //Retrieve symbolics
    if (txn->getSymbolics("symbolics", &key, s))
    {
        //Send by OSC
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        p << osc::BeginMessage( "/Orchids/reply/getSymbolics" ) << s.playingStyle << s.dynamics << s.note << s.MIDIcent << s.duration << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
    }
    else
        sendOSCMessageError("Can't find sound in database");
}

void getSoundsQuery(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    vector<Query>   queries;
    int             nbQueries;
    //If quantiles are requested, keep descriptor name and N = number of intervals
    bool            needQuantiles = false;
    string          desc;
    int             N, i, j;
    
    args >> nbQueries;
    
    for (i = 0; i < nbQueries; i++)
    {
        const char *type, *descriptor;
        args >> type >> descriptor;
        
        if ( strcmp( type, "IS" ) == 0 || strcmp( type, "REGEXP" ) == 0 )
        {
            //Case: IS MUTED / IS UNMUTED
            if (strcmp(descriptor, "MUTED") == 0 || strcmp(descriptor, "UNMUTED") == 0)
            {
                Query q(type, descriptor);
                queries.push_back(q);
            }
            //Case: REGEXP {string} {string}
            else
            {
                const char *value;
                args >> value;
                string val = value;
                Query q(type, descriptor, val);
                queries.push_back(q);
            }
        }
        //Case: UNDER/OVER {string} {float}
        else if ( strcmp( type, "UNDER" ) == 0 || strcmp( type, "OVER" ) == 0 )
        {
            float val;
            args >> val;
            Query q(type, descriptor, pair<float, float>(val, 0));
            queries.push_back(q);
        }
        //Case: BETWEEN {string} {float} {float}
        else if ( strcmp( type, "BETWEEN" ) == 0 )
        {
            float start, end;
            args >> start >> end;
            Query q(type, descriptor, pair<float, float>(start, end));
            queries.push_back(q);
        }
        //Case: FOLLOWS {descriptor} {Complete/Fast} {Parameter} {float[128]}
        else if ( strcmp( type, "FOLLOWS" ) == 0 )
        {
            const char* indexing;
            float parameter;
            args >> indexing >> parameter;
            
            vector<float> values;
            if (strcmp(indexing, "Complete") == 0)
                values.push_back(0);
            else if (strcmp(indexing, "Fast") == 0)
                values.push_back(1);
            
            values.push_back(parameter);
            for (j = 0; j < 128; j++)
            {
                float val;
                args >> val;
                values.push_back(val);
            }
            Query q(type, descriptor, values);
            queries.push_back(q);
        }
        else if ( strcmp( type, "QUANTILES" ) == 0 )
        {
            needQuantiles = true;
            desc = descriptor;
            args >> N;
        }
    }
    args >> osc::EndMessage;
    
    //Run getSoundsQuery
    vector<int>     soundIDs;
    vector<string>  soundNames;
    if (!txn->getSoundsQuery(queries, soundIDs))
    {
        sendOSCMessageError("Database error during querying");
        return;
    }
    else if (soundIDs.size() == 0)
        soundNames.push_back("No Data");
    else
        soundNames = txn->getNames(soundIDs);
    sendOSCSoundNames("/Orchids/reply/getSoundsQuery", soundNames);
    
    //Compute quantiles if needed
    if (needQuantiles)
    {
        vector<int> freq;
        if (soundIDs.size() == 0)
            freq = getQuantiles(desc, N, txn);
        else
        {
            vector<float> means;
            txn->getSoundsDescriptorMean( desc, soundIDs, means );
            freq = histogram<float>(means, N);
        }
        desc += "Mean";
        float min, max;
        txn->getDescriptorMinMax(desc, min, max);
        
        //Send by OSC min, max and quantiles
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        p << osc::BeginMessage( "/Orchids/reply/getDescriptorFilteredMinMaxQuantiles" ) << min << max;
        for (j = 0; j < N; j++)
            p << freq[j];
        p << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
        p.Clear();
    }
}

void getDescriptorMinMax(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    int i;
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    vector<string> descriptors;
    while (arg != m.ArgumentsEnd())
    {
        const char* desc = (arg++)->AsString();
        strcat((char*)desc, "Mean");
        descriptors.push_back(desc);
    }
    
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    p << osc::BeginMessage( "/Orchids/reply/getDescriptorMinMax" );
    for (i = 0; i < descriptors.size(); i++)
    {
        float min, max;
        //Get minimum and maximum descriptor mean
        if (txn->getDescriptorMinMax(descriptors[i], min, max))
            p << min << max;
        else
            sendOSCMessageError("Can't find descriptor in database");
    }
    p << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void getMultipleDescriptorMinMax(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    int i;
    osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
    vector<string> descriptors;
    while (arg != m.ArgumentsEnd())
    {
        const char* desc = (arg++)->AsString();
        strcat((char*)desc, "Mean");
        descriptors.push_back(desc);
    }
    
    UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
    char                        buffer[MAX_SIZE_BUFFER];
    osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
    p << osc::BeginMessage( "/Orchids/reply/getDescriptorMinMax" );
    for (i = 0; i < descriptors.size(); i++)
    {
        float min, max;
        //Get minimum and maximum descriptor mean
        if (txn->getDescriptorMinMax(descriptors[i], min, max))
            p << min << max;
        else
            sendOSCMessageError("Can't find descriptor in database");
    }
    p << osc::EndMessage;
    transmitSocket.Send( p.Data(), p.Size() );
}

void getSingle(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    int i;
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char *sound, *desc;
    args >> sound >> desc >> osc::EndMessage;
    int index = txn->getIndex(sound);
    Dbt key;
    init_dbt(&key, &index, sizeof(int));
    
    //Retrieve the descriptor resampled value
    feature<float> f;
    if (txn->getDescriptor(desc, &key, f))
    {
        float min, max;
#ifndef DATABASE_LIGHT_VERSION
        min = getMin(f.values.c_data(), f.values.size());
        max = getMax(f.values.c_data(), f.values.size());
#else
        min = f.min;
        max = f.max;
#endif
        //Send by OSC
        UdpTransmitSocket           transmitSocket( IpEndpointName( ADDRESS, REMOTE_PORT ) );
        char                        buffer[MAX_SIZE_BUFFER];
        osc::OutboundPacketStream   p( buffer, MAX_SIZE_BUFFER );
        p << osc::BeginMessage( "/Orchids/reply/getSingle" ) << f.mean << f.stdDev;
        for (i = 0; i < f.resampled_val->size(); i++)
            p << (f.resampled_val->operator()(i) * (max - min) + min); //Dénormalisation
        p << osc::EndMessage;
        transmitSocket.Send( p.Data(), p.Size() );
    }
    else
        sendOSCMessageError("Can't find descriptor in database");
}

void getIndexList(const osc::ReceivedMessage& m, BDBTxn* txn)
{
    osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
    const char*     table;
    args >> table >> osc::EndMessage;
    
    vector<string>  list;
    if (txn->getSecondaryIndexList(table, list))
        sendOSCSoundNames("/Orchids/reply/getIndexList", list);
    else
        sendOSCMessageError("Can't find index in database");
}