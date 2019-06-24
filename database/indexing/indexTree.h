//
//  indexTree.h
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__indexTree__
#define __Orchids__indexTree__

#include "Orchids.h"

class indexNode;

#define indexNodePtr boost::shared_ptr<indexNode>
#define indexNodeVec vector<indexNodePtr>

/**
 *  @class indexTree
 */
class indexTree
{
protected:
    indexNodePtr    root;
    int             wordLength;
    int             maxCardinality;
    indexNodeVec    terminalNodes;
    float           cardIncrease;
    int             maxSeries;
    vector<float>   cutPoints;
    
public:
    indexTree(){};
    indexTree(int wordLen, int cardinality, int maxN, int cardInc);
    ~indexTree(){};
    
    indexNodePtr    getRoot();
    void            removeAllSeries();
    void            constructNearestNeighbors();
    void            updateNearestNeighbors(indexNodePtr newNode);
    void            removeTerminalNode(indexNodePtr oldNode);
    void            insertSeries(vector<float>& rawData, int serieID, float featList);
    void            approximateQuery();
    void            approximateQueryConstrained(
                                                vector<float>& rawData,
                                                vector<int>&   filterID,
                                                int            maxNum,
                                                float          duration,
                                                vector<int>&   results,
                                                indexNodePtr   resNode);
    void            resetVisitedStatus();
    void            exactQuery();
    void            exactQueryTopDown();
    void            exactQueryUpward();
    void            dumpTree();
    
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & root;
        ar & wordLength;
        ar & maxCardinality;
        //ar & terminalNodes;
        ar & cardIncrease;
        ar & maxSeries;
        ar & cutPoints;
    }
};

#endif /* defined(__Orchids__indexTree__) */
