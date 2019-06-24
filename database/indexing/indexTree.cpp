//
//  indexTree.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "indexTree.h"


indexTree::indexTree(int wordLen, int cardinality, int maxN, int cardInc)
: root(new indexNode("internal", wordLen))
{
    wordLength      = wordLen;
    maxCardinality  = cardinality;
    cardIncrease    = cardInc;
    maxSeries       = maxN;
    int minCard     = -(pow(2, (float)cardinality) - 1);
    cutPoints.push_back(minCard);
    while(minCard < (pow(2, (float)cardinality) - 1))
    {
        minCard += 2;
        cutPoints.push_back((float)minCard / (pow(2, (float)cardinality)));
    }
    cutPoints[0] = -2;
    cutPoints[cutPoints.size() - 1] = 2;
}

indexNodePtr indexTree::getRoot()
{
    return root;
}

void indexTree::removeAllSeries()
{
    
}

void indexTree::constructNearestNeighbors()
{
    
}

void indexTree::updateNearestNeighbors(indexNodePtr newNode)
{
    terminalNodes.push_back(newNode);
}

void indexTree::removeTerminalNode(indexNodePtr oldNode)
{
    int i, rIndex = 0;
    for (i = 0; i < terminalNodes.size(); i++)
    {
        if (terminalNodes[i] == oldNode)
        {
            rIndex = i;
            break;
        }
    }
    terminalNodes.erase(terminalNodes.begin() + rIndex);
}

void indexTree::insertSeries(vector<float>& rawData, int serieID, float featList)
{
    int             i;
    matrix          binaryRep   = matrix(indexRepresentation(rawData, (int)rawData.size(), wordLength, maxCardinality, cutPoints));
    indexNodePtr    curNode     = root;
    while (curNode->getIsInternal())
    {
        curNode->pushId(serieID);
        curNode->pushFeature(featList);
        int curCard = curNode->getCardinality();
        vector<int> curBinary(binaryRep.begin(), binaryRep.begin() + curCard);
        vector<int> tmpHash(curBinary.begin() + curNode->getDataSize(), curBinary.end());
        indexNodePtr node = curNode->getHashNode(getHash(tmpHash));
        if (node->getType() != "")
        {
            if (node->getIsTerminal())
            {
                if (node->getNbSeries() < maxSeries)
                {
                    node->addSeries(matrix(binaryRep), serieID, featList);
                    break;
                }
                else
                {
                    int newCard = curCard + cardIncrease;
                    if (newCard > binaryRep.size())
                    {
                        node->addSeries(matrix(binaryRep), serieID, featList);
                        break;
                    }
                    removeTerminalNode(node);
                    indexNodePtr    newIntNode(new indexNode("internal", newCard, curBinary));
                    vector<matrix>  tmpSeries = node->getSeriesList();
                    vector<int>     tmpIDList = node->getIdList();
                    vector<float>   tmpFList  = node->getFeatureList();
                    for (i = 0; i < tmpIDList.size(); i++)
                    {
                        matrix          curSeries = tmpSeries[i];
                        vector<int>     tmpBinary(curSeries.begin(), curSeries.begin() + newCard);
                        vector<int>     tmpHash(tmpBinary.begin() + newIntNode->getDataSize(), tmpBinary.end());
                        indexNodePtr    tmpNode = newIntNode->getHashNode(getHash(tmpHash));
                        if (tmpNode->getType() != "")
                            tmpNode->addSeries(curSeries, tmpIDList[i], tmpFList[i]);
                        else
                        {
                            indexNodePtr newNode(new indexNode("terminal", newCard, tmpBinary));
                            newNode->addSeries(curSeries, tmpIDList[i], tmpFList[i]);
                            updateNearestNeighbors(newNode);
                            newIntNode->addChild(newNode);
                        }
                    }
                    tmpSeries.clear();
                    tmpIDList.clear();
                    curNode->setNbChildren(curNode->getNbChildren() - 1);
                    node->disconnect();
                    node.reset();
                    curNode->addChild(newIntNode);
                    curNode = newIntNode;
                }
            }
            else if (node->getIsInternal())
                curNode = node;
        }
        else
        {
            indexNodePtr newNode(new indexNode("terminal", curCard, curBinary));
            newNode->addSeries(matrix(binaryRep), serieID, featList);
            updateNearestNeighbors(newNode);
            curNode->addChild(newNode);
            break;
        }
    }
}

void indexTree::approximateQuery()
{
    
}

void indexTree::approximateQueryConstrained(
                                            vector<float>& rawData,
                                            vector<int>&   filterID,
                                            int            maxNum,
                                            float          duration,
                                            vector<int>&   results,
                                            indexNodePtr   resNode)
{
    int i;
    matrix binaryRep = indexRepresentation(rawData, (int)rawData.size(), wordLength, maxCardinality, cutPoints);
    vector<float> distList;
    resNode = root;
    float nbBound           = maxNum;// ???? pow((float)2, pow2roundup(maxNum));
    float durVariability    = 0.2 * duration;
    //printf("DATA ITSELF :\n");
    //for (i = 0; i < rawData.size(); i++)
        //printf("%f ", rawData[i]);
    //printf("\n");
    //printf("REPRESENTATION OF DATA :\n");
    /*
    for (i = 0; i < binaryRep.rows(); i++)
    {
        for (j = 0; j < binaryRep.cols(); j++)
            printf("%f ", binaryRep(i, j));
        printf("\n");
    }
     */
    //printf("ABOUT TO QUERY ...\n");
    while (resNode->getIsInternal())
    {
        //printf("GOING DOWN ON NODE ...\n");
        if (filterID.empty())
            results = resNode->getIdList();
        else
        {
            vector<int> index = isMember(filterID, resNode->getIdList());
            vector<int> idList = resNode->getIdList();
            reorder(idList, index, results);
        }
        //printf("RESULTS : %d\n", (int)results.size());
        if (results.size() < nbBound)
            break;
        int             curCard = resNode->getCardinality();
        vector<int>     curBinary(binaryRep.begin(), binaryRep.begin() + curCard);
        //for (i = 0; i < curBinary.size(); i++)
            //printf("%d ", curBinary[i]);
        //printf("\n");
        indexNodePtr    node = resNode->containsChild(curBinary);
        //printf("%s\n", node->getType().c_str());
        if (node->getType() != "")
        {
            vector<int> intersect;
            if (filterID.empty())
                intersect = node->getIdList();
            else
            {
                vector<int> index = isMember(node->getIdList(), filterID);
                reorder(filterID, index, intersect);
            }
            if (intersect.size() < nbBound)
                break;
            resNode = node;
        }
        else
            break;
    }
    
    vector<float>   features = resNode->getFeatureList();
    vector<bool>    dist;
    for (i = 0; i < features.size(); i++)
    {
        distList.push_back(abs(features[i] - duration));
        (distList[i] < durVariability) ? dist.push_back(true) : dist.push_back(false);
    }
    vector<int> tmpResults;
    for (i = 0; i < dist.size(); i++)
        if (dist[i])
            tmpResults.push_back(resNode->getIdList()[i]);
    if (filterID.empty())
        results = tmpResults;
    else
    {
        vector<int> index = isMember(tmpResults, filterID);
        reorder(filterID, index, results);
    }
    
    if (resNode == root)
        results.clear();
}

void indexTree::resetVisitedStatus()
{
    
}

void indexTree::exactQuery()
{
    
}

void indexTree::exactQueryTopDown()
{
    
}

void indexTree::exactQueryUpward()
{
    
}

void indexTree::dumpTree()
{
    
}

BOOST_CLASS_EXPORT(indexTree);