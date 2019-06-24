//
//  knowledge.h
//  Orchids
//
//  Created by Antoine Bouchereau on 19/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__knowledge__
#define __Orchids__knowledge__

#include "Orchids.h"

class Session;

/**
 *  @class Knowledge
 *
 *  @brief Abstract interface for knowledge definition
 *  This class allows to define an abstract source of knowledge
 */
class Knowledge
{
protected:
    Session*                sSession;          ///< Pointer to current session
    char                    *creationDate;      ///< Date of creation for the knowledg
    knowledgeDescription    knowledgeDesc;      ///< Description of the knowledge source
    map<string, tpl>        attributeDomains;   ///< Possible values for symbolic attributes
    vector<string>          allowedFeatures;    ///< Set of allowed features
    int                     neutralElement;     ///< Neutral element of knowledge
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     */
    Knowledge(Session* sessObj);
    /**
     *  @brief Destructor
     */
    virtual ~Knowledge(){};
    
    /**
     *  @brief Return knowledge descriptor
     */
    virtual knowledgeDescription    &getFieldsList() = 0;
    /**
     *  @brief Get informations about a knowledge field
     */
    virtual vector<string>          &getFieldsInfo() = 0;
    /**
     *  @brief Get type of a knowledge field
     */
    virtual vector<string>          &getTypesInfo()  = 0;
    /**
     *  @brief Return the possibility of querying a knwoledge field
     */
    virtual vector<int>             &getQueryablesInfo() = 0;
    /**
     *  @brief Get values of a knowledge field for a specified ID list
     */
    virtual void                    getFieldsValues(string, vector<int>&, vector<string>&, vector<float>&) = 0;
    /**
     *  @brief Get all possible values for a knowledge field
     */
    virtual vector<string>          getFieldsValueList(string) = 0;
    
    /**
     *  @brief Build the knowledge structure from its source
     */
    virtual void buildKnowledge()                   = 0;
    /**
     *  @brief Compute domains for a specific attribute
     */
    virtual void computeAttributeDomains(string)    = 0;
    /**
     *  @brief Modify values of fields from the knowledge
     */
    virtual void setFields()                        = 0;
    
    /**
     *  @brief Return creation date
     */
    char*               getCreationDate();
    /**
     *  @brief Get the structure of attributes domain
     */
    map<string, tpl>    &getAttributeDomains();
};


class BDBConnector;
class BDBTxn;
class Query;

/**
 *  @class KnowledgeBDB
 *
 *  @brief Defining a knowledge based on a Berkeley DB database
 *  This class allows to define a source of knowledge based on a Berkeley DB database
 */
class KnowledgeBDB : public Knowledge
{
protected:
    string          dbName;         ///< Name of DB directory
    string          dbFile;         ///< Name of DB file
    BDBConnector    *bdb;           ///< Pointer to database connector
    BDBTxn          *sDatabase;     ///< Pointer to database transaction system
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param dbName Name of DB directory
     *  @param dbFile Name of DB file
     */
    KnowledgeBDB(Session* sessObj, string dbName, string dbFile);
    /**
     *  @brief Destructor
     */
    ~KnowledgeBDB();
    
    /**
     *  @brief Return pointer to the database connector
     */
    BDBTxn*                 getBDBConnector();
    /**
     *  @brief Return knowledge descriptor
     */
    knowledgeDescription    &getFieldsList();
    /**
     *  @brief Get informations about a knowledge field
     */
    vector<string>          &getFieldsInfo();
    /**
     *  @brief Get type of a knowledge field
     */
    vector<string>          &getTypesInfo();
    /**
     *  @brief Return the possibility of querying a knwoledge field
     */
    vector<int>             &getQueryablesInfo();
    /**
     *  @brief Get values of a knowledge field for a specified ID list
     */
    void                    getFieldsValues(string fName, vector<int>& fID, vector<string> &symValues, vector<float> &featValues);
    /**
     *  @brief Get all possible values for a knowledge field
     */
    vector<string>          getFieldsValueList(string fName);
    /**
     *  @brief Retrieve the number of sounds in the database
     */
    int                     getNbEntries();
    /**
     *  @brief Get possible list of optimization criteria
     */
    vector<string>          &getCriteriaList();
    /**
     *  @brief Get neutral element ID of knowledge
     */
    int                     getNeutralID();

    /**
     *  @brief Build the knowledge structure from its source.
     *  This function fills the description of knowledge :
     *  - Name of the existing fields
     *  - Types of each field
     *  - Possibility of querying
     */
    void buildKnowledge();
    /**
     *  @brief Compute domains for a specific attribute
     */
    void computeAttributeDomains(string fName);
    /**
     *  @brief Perform a query over the knowledge source
     */
    void computeQuery(vector<Query> &queries, vector<int>& soundIDs);

    /**
     *  @brief Modify values of fields from the knowledge
     */
    void setFields();
    void printKnowledge();
};


#endif /* defined(__Orchids__knowledge__) */
