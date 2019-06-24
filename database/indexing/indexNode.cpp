//
//  indexNode.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "indexNode.h"

indexNode::indexNode()
{
    this->dataSize       = 0;
    this->nbChildren     = 0;
    this->nbSeries       = 0;
    this->cardinality    = 0;
    this->nodeType       = "";
    this->isTerminal     = false;
    this->isInternal     = false;
    this->visited        = 0;
    this->dist           = 0;
}

indexNode::indexNode(string type, int card)
{
    this->dataSize       = 0;
    this->nbChildren     = 0;
    this->nbSeries       = 0;
    this->cardinality    = card;
    this->nodeType       = type;
    this->isTerminal     = (this->nodeType == "terminal") ? true : false;
    this->isInternal     = (this->nodeType == "internal") ? true : false;
    this->hashNodes.resize(pow(2, (float)card - (float)this->dataSize), indexNodePtr(new indexNode()));
    this->visited        = 0;
    this->dist           = 0;
}

indexNode::indexNode(string type, int card, vector<int> data)
{
    this->data           = data;
    this->dataSize       = (int)data.size();
    this->nbChildren     = 0;
    this->nbSeries       = 0;
    this->cardinality    = card;
    this->nodeType       = type;
    this->isTerminal     = (this->nodeType == "terminal") ? true : false;
    this->isInternal     = (this->nodeType == "internal") ? true : false;
    this->hashNodes.resize(pow(2, (float)card - (float)this->dataSize), indexNodePtr(new indexNode()));
    this->visited        = 0;
    this->dist           = 0;
}

void indexNode::setData(vector<int>& data)
{
    this->data      = data;
    this->dataSize  = (int)data.size();
}

void indexNode::getLeaves()
{

}

void indexNode::getNumberDescendants()
{
    
}

void indexNode::addChild(indexNodePtr child)
{
    nbChildren++;
    vector<int> data = child->getData();
    vector<int> tmpHash(data.begin() + dataSize, data.end());
    hashNodes[getHash(tmpHash)] = child;
}

vector<int> indexNode::getData()
{
    return data;
}

void indexNode::addSeries(matrix series, int id, float feat)
{
    featureList.push_back(feat);
    seriesList.push_back(series);
    idList.push_back(id);
    nbSeries++;
}

indexNodePtr indexNode::containsChild(vector<int>& binaryRep)
{
    vector<int> tmpHash(binaryRep.begin() + dataSize + 1, binaryRep.end());
    return getHashNode(getHash(tmpHash));
}

void indexNode::addSoundID()
{
    
}

void indexNode::numberOfSeries()
{
    
}

void indexNode::removeNearestNeighbor()
{
    
}

void indexNode::sortNearestNeighbors()
{
    
}

void indexNode::initNearestNeighbors()
{
    
}

void indexNode::addNearestNeighbor()
{
    
}

void indexNode::approxDistToNode()
{
    
}

void indexNode::bestTrueDistance()
{
    
}

void indexNode::emptySeriesList()
{
    
}

void indexNode::disconnect()
{
    idList.clear();
    seriesList.clear();
    hashNodes.clear();
    featureList.clear();
}

void indexNode::kill()
{
    
}

BOOST_CLASS_EXPORT(indexNode);
