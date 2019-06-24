//
//  knowledgeBDB.h
//  Orchids
//
//  Created by Antoine Bouchereau on 19/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__knowledgeBDB__
#define __Orchids__knowledgeBDB__

#include "knowledge.h"

class Knowledge;
class BDBConnector;
class BDBTxn;

class KnowledgeBDB : public Knowledge
{
protected:
    string          dbName;
    string          dbFile;
    BDBConnector    *bdb;
    BDBTxn          *sDatabase;
    
public:
    KnowledgeBDB(Session *sessObj, string dbName, string dbFile);
    ~KnowledgeBDB();
    
    BDBTxn* getBDBConnector();
    void buildKnowledge();
    void getFieldsInfo();
    void getFieldsValues();
    vector<string> getFieldsValueList(string fName);
    void computeAttributeDomains(string fName);
    void setFields();
};

#endif /* defined(__Orchids__knowledgeBDB__) */
