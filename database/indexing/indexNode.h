//
//  indexNode.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__indexNode__
#define __Orchids__indexNode__

#include "Orchids.h"

#define indexNodePtr boost::shared_ptr<indexNode>
#define indexNodeVec vector<indexNodePtr>

/**
 *  @class indexNode
 */
class indexNode
{
protected:
    vector<int>     data;
    float           dist;
    int             dataSize;
    //indexNodeVec    nearestNodes;
    //vector<float>   nearestDists;
    int             nbChildren;
    string          nodeType;
    bool            isInternal;
    bool            isTerminal;
    int             cardinality;
    int             visited;
    indexNodeVec    hashNodes;
    vector<matrix>  seriesList;
    vector<float>   featureList;
    int             nbSeries;
    vector<int>     idList;
    
public:
    indexNode();
    indexNode(string type, int card);
    indexNode(string type, int card, vector<int> data);
    ~indexNode(){};
    
    bool            getIsInternal()             { return isInternal; };
    bool            getIsTerminal()             { return isTerminal; };
    int             getCardinality()            { return cardinality; };
    int             getDataSize()               { return dataSize; };
    int             getNbChildren()             { return nbChildren; };
    string          getType()                   { return nodeType; };
    int             getNbSeries()               { return nbSeries; };
    indexNodePtr    getHashNode(int id)         { return hashNodes[id]; };
    vector<matrix>  getSeriesList()             { return seriesList; };
    vector<int>     getIdList()                 { return idList; };
    vector<float>   getFeatureList()            { return featureList; };
    
    void            setNbSeries(int nb)         { nbSeries = nb; };
    void            setNbChildren(int nb)       { nbChildren = nb; };
    
    void            pushId(int id)              { idList.push_back(id); };
    void            pushFeature(float feat)     { featureList.push_back(feat); };
    void            pushSeries(matrix series)   { seriesList.push_back(series); };

    void            setData(vector<int>& data);
    void            getLeaves();
    void            getNumberDescendants();
    void            addChild(indexNodePtr child);
    vector<int>     getData();
    void            addSeries(matrix series, int id, float feat);
    indexNodePtr    containsChild(vector<int>& binaryRep);
    void            addSoundID();
    void            numberOfSeries();
    void            removeNearestNeighbor();
    void            sortNearestNeighbors();
    void            initNearestNeighbors();
    void            addNearestNeighbor();
    void            approxDistToNode();
    void            bestTrueDistance();
    void            emptySeriesList();
    void            disconnect();
    void            kill();
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & data;
        ar & dist;
        ar & dataSize;
        //ar & nearestNodes;
        //ar & nearestDists;
        ar & nbChildren;
        ar & nodeType;
        ar & isInternal;
        ar & isTerminal;
        ar & cardinality;
        ar & visited;
        ar & hashNodes;
        ar & seriesList;
        ar & featureList;
        ar & nbSeries;
        ar & idList;
    }
};


#endif /* defined(__Orchids__indexNode__) */
