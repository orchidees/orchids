//
//  knowledgeBDB.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 19/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "knowledgeBDB.h"


KnowledgeBDB::KnowledgeBDB(Session *sessObj, string name, string file)
: Knowledge(sessObj)
{
    dbName      = name;
    dbFile      = file;
    bdb         = new BDBConnector(dbName, dbFile);
    sDatabase   = new BDBTxn(*bdb);
}

KnowledgeBDB::~KnowledgeBDB()
{
    sDatabase->close();
    
    delete sDatabase;
    delete bdb;
}

BDBTxn* KnowledgeBDB::getBDBConnector()
{
    return sDatabase;
}

void KnowledgeBDB::buildKnowledge()
{
    knowledgeDesc.fields    = getFieldsList();
    knowledgeDesc.types     = getTypesList();
    
    for (i = 0; i < knowledgeDesc.fields.size(); i++)
    {
        if (knowledgeDesc.types[i] == "string")
        {
            if (knowledgeDesc.fields[i] != "name" && knowledgeDesc.fields[i] != "file")
                computeAttributeDomains(knowledgeDesc.fields[i]);
            knowledgeDesc.queryable.push_back(1);
        }
        else if (knowledgeDesc.types[i] == "float")
        {
            if (knowledgeDesc.fields[i] == "duration" || knowledgeDesc.fields[i] == "MIDICent")
                allowedFeatures.push_back(knowledgeDesc.fields[i]);

            float min, max;
            sDatabase->getDescriptorMinMax(knowledgeDesc.fields[i], min, max);
            tuple<float, float, vector<string>> values(min, max, vector<string>());
            attributeDomains.insert(pair<string, tuple<float, float, vector<string>>>(knowledgeDesc.fields[i], values));
            knowledgeDesc.queryable.push_back(1);
        }
        else if (knowledgeDesc.types[i] == "descriptor")
        {
            allowedFeatures.push_back(knowledgeDesc.fields[i]);
            knowledgeDesc.queryable.push_back(0);
        }
        else if (knowledgeDesc.types[i] == "array")
        {
            allowedFeatures.push_back(knowledgeDesc.fields[i]);
            knowledgeDesc.queryable.push_back(0);
        }
        else if (knowledgeDesc.types[i] == "array2D")
        {
            allowedFeatures.push_back(knowledgeDesc.fields[i]);
            knowledgeDesc.queryable.push_back(0);
        }
    }
}

void KnowledgeBDB::getFieldsInfo()
{
    
}

void KnowledgeBDB::getFieldsValues()
{
    
}

vector<string> KnowledgeBDB::getFieldsValueList(string fName)
{
    vector<string> list;
    sDatabase->getSecondaryIndexList(fName, list);
    //If "note" then sort values by pitch
    if (fName == "note")
    {
        vector<int> MIDICents;
        for (i = 0; i < list.size(); i++)
        {
            MIDICents[i] = noteToMidiCent(list[i].c_str());
            MedianHybridQuickSort(&MIDICents[0], (int)MIDICents.size());
            reorder(list, MIDICents, list);
        }
    }
    return list;
}

void KnowledgeBDB::computeAttributeDomains(string fName)
{
    if (std::find(knowledgeDesc.fields.begin(), knowledgeDesc.fields.end(), fName) == knowledgeDesc.fields.end())
    {
        printf("KnowledgeSQL:computeAttributeDomains:Unknown', %s 'is not a knowledge field\n", fName.c_str());
        return;
    }
    vector<string> listValues = getFieldsValueList(fName);
    tuple<float, float, vector<string>> values(0, 0, listValues);
    attributeDomains.insert(pair<string, tuple<float, float, vector<string>>>(fName, values));
}

void KnowledgeBDB::setFields()
{
    
}