//
//  knowledge.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 19/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "knowledge.h"


bool noteCmp(string a, string b)
{
    return noteToMidiCent(a.c_str()) < noteToMidiCent(b.c_str());
}


Knowledge::Knowledge(Session* sessObj)
{
    sSession        = sessObj;
    neutralElement  = -1;
    time_t now      = time(0);
    tm *gmtm        = gmtime(&now);
    creationDate    = asctime(gmtm);
}

char* Knowledge::getCreationDate()
{
    return creationDate;
}

map<string, tpl> &Knowledge::getAttributeDomains()
{
    return attributeDomains;
}

KnowledgeBDB::KnowledgeBDB(Session* sessObj, string name, string file)
: Knowledge(sessObj)
{
    dbName      = name;
    dbFile      = file;
    bdb         = new BDBConnector(dbName, dbFile, sessObj);
    sDatabase   = new BDBTxn(*bdb);
    sDatabase->initCoreDB();
}

KnowledgeBDB::~KnowledgeBDB()
{
    sDatabase->close();
    
    free(sDatabase);
    delete bdb;
}

BDBTxn* KnowledgeBDB::getBDBConnector()
{
    return sDatabase;
}

void KnowledgeBDB::buildKnowledge()
{
    int i;
    knowledgeDesc.fields    = getTablesList();
    knowledgeDesc.types     = getTypesList();
    
    for (i = 0; i < knowledgeDesc.fields.size(); i++)
    {
        printf("Constructing knowledge : %s\n", knowledgeDesc.fields[i].c_str());
        if (knowledgeDesc.types[i] == "string")
        {
            if (knowledgeDesc.fields[i] != "name" && knowledgeDesc.fields[i] != "file")
                computeAttributeDomains(knowledgeDesc.fields[i]);
            knowledgeDesc.queryable.push_back(1);
        }
        else if (knowledgeDesc.types[i] == "float")
        {
            if (knowledgeDesc.fields[i] != "duration" && knowledgeDesc.fields[i] != "MIDIcent")
                allowedFeatures.push_back(knowledgeDesc.fields[i]);
            
            float min, max;
            sDatabase->getDescriptorMinMax(knowledgeDesc.fields[i], min, max);
            std::map<string, tpl>::iterator it = attributeDomains.find(knowledgeDesc.fields[i]);
            if (it != attributeDomains.end())
                it->second = make_tuple(min, max, vector<string>());
            //attributeDomains[knowledgeDesc.fields[i]] = make_tuple(min, max, vector<string>());
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
    cout << "Done" << endl;
}

knowledgeDescription &KnowledgeBDB::getFieldsList()
{
    if (knowledgeDesc.isEmpty())
        buildKnowledge();
    
    return knowledgeDesc;
}

vector<string> &KnowledgeBDB::getFieldsInfo()
{
    return knowledgeDesc.fields;
}

vector<string> &KnowledgeBDB::getTypesInfo()
{
    return knowledgeDesc.types;
}

vector<int> &KnowledgeBDB::getQueryablesInfo()
{
    return knowledgeDesc.queryable;
}

void KnowledgeBDB::getFieldsValues(string fName, vector<int> &fID, vector<string> &symValues, vector<float> &featValues)
{
    std::vector<string>::iterator it = std::find(knowledgeDesc.fields.begin(), knowledgeDesc.fields.end(), fName);
    if (it == knowledgeDesc.fields.end())
    {
        printf("KnowledgeSQL:getFieldValues:Unknown', %s ' : no such field in database\n", fName.c_str());
        return;
    }
    
    string type = knowledgeDesc.types[it - knowledgeDesc.fields.begin()];
    // Symbolics
    if (type == "string")
    {
        for (vector<int>::iterator it = fID.begin(); it != fID.end(); ++it)
        {
            Dbt key;
            init_dbt(&key, &*it, sizeof(int));
            symbolics s;
            sDatabase->getSymbolics("symbolics", &key, s);
            symValues.push_back(boost::any_cast<string>(s.getMember(fName)));
        }
    }
    else if (type == "float")
    {
        // Float symbolics
        if (fName == "duration" || fName == "MIDIcent")
        {
            for (vector<int>::iterator it = fID.begin(); it != fID.end(); ++it)
            {
                Dbt key;
                init_dbt(&key, &*it, sizeof(int));
                symbolics s;
                sDatabase->getSymbolics("symbolics", &key, s);
                featValues.push_back(boost::any_cast<float>(s.getMember(fName)));
            }
        }
        // Mel Bands
        else if (fName == "LoudnessFactor")
        {
            for (vector<int>::iterator it = fID.begin(); it != fID.end(); ++it)
            {
                Dbt key;
                init_dbt(&key, &*it, sizeof(int));
                melStruct m;
                sDatabase->getMelBands("melbands", &key, m);
                featValues.push_back(m.loudnessFactor);
            }
        }
        // Partials
        else if (fName == "PartialsMeanEnergy")
        {
            for (vector<int>::iterator it = fID.begin(); it != fID.end(); ++it)
            {
                Dbt key;
                init_dbt(&key, &*it, sizeof(int));
                partials p;
                sDatabase->getPartials("partials", &key, p);
                featValues.push_back(p.partialsMeanEnergy);
            }
        }
        // Descriptors
        else
        {
            for (vector<int>::iterator it = fID.begin(); it != fID.end(); ++it)
            {
                Dbt key;
                init_dbt(&key, &*it, sizeof(int));
                feature<float> f;
                sDatabase->getDescriptor(fName, &key, f);
                featValues.push_back(f.mean);
            }
        }
    }
}

vector<string> KnowledgeBDB::getFieldsValueList(string fName)
{
    vector<string> list;
    sDatabase->getSecondaryIndexList(fName, list);
    //If "note" then sort values by pitch
    if (fName == "note")
        std::sort(list.begin(), list.end(), noteCmp);

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
    attributeDomains[fName] = make_tuple(0, 0, listValues);
}

void KnowledgeBDB::setFields()
{
    
}

int KnowledgeBDB::getNbEntries()
{
    return sDatabase->get_current_index() - 1;
}

vector<string> &KnowledgeBDB::getCriteriaList()
{
    if (knowledgeDesc.isEmpty())
        buildKnowledge();

    return allowedFeatures;
}

int KnowledgeBDB::getNeutralID()
{
    if (neutralElement == -1)
        neutralElement = sDatabase->get_current_index();
    
    return neutralElement;
}

void KnowledgeBDB::computeQuery(vector<Query> &queries, vector<int>& soundIDs)
{
    sDatabase->getSoundsQuery(queries, soundIDs);
}

void KnowledgeBDB::printKnowledge()
{
    int i;
    map<string, tpl>::iterator it = attributeDomains.begin();
    for ( it = attributeDomains.begin(); it != attributeDomains.end(); ++it )
    {
        cout << it->first << endl;
        if (std::get<2>(it->second).size() == 0)
            cout << std::get<0>(it->second) << " " << std::get<1>(it->second) << endl;
        else
        {
            for (i = 0; i < std::get<2>(it->second).size(); i++)
                cout << std::get<2>(it->second)[i] << " ";
            cout << endl;
        }
        cout << endl;
    }
}
