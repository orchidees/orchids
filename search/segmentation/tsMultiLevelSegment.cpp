//
//  tsMultiLevelSegment.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 08/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "tsMultiLevelSegment.h"

void tsMultiLevelSegment(
                         matrixPtr                  tmpSeries,
                         int                        nbSegments,
                         vector<vector<float> >&    segmentSeries,
                         vector<int>&               tmpOn,
                         vector<int>&               durations)
{
    int i;
    vector<float> dataVec(tmpSeries->begin(), tmpSeries->end());
    // Perform piecewise linear approximation on the series
    vector<segment> segments = bottomUpSegmentation(tmpSeries, nbSegments);
    // Compute entropy for each base segment
    float std = std_dev(&dataVec[0], (int)dataVec.size(), getMean(&dataVec[0], (int)dataVec.size()));
    for (i = 0; i < segments.size(); i++)
    {
        vector<int>     segmented = linspace<int>(segments[i].lx, segments[i].rx, segments[i].rx - segments[i].lx + 1);
        vector<float>   segVal;
        reorder(dataVec, segmented, segVal);
        segments[i].entropy = approximateEntropy(1, 0.2, std, segVal);
        segments[i].deltaEnt = numeric_limits<float>::max();
    }
    vector<segment> tmpQueue            = segments;
    vector<segment> hierarchicalSegment = segments;
    segment         curSegment          = segments[0];
    while (!(curSegment.lx == 0 && curSegment.rx == (tmpSeries->size() - 1)))
    {
        vector<float> deltaEntropy;
        for (i = 0; i < tmpQueue.size() - 1; i++)
        {
            if (tmpQueue[i].deltaEnt == numeric_limits<float>::max())
            {
                vector<float> segVal = linspace<float>(tmpQueue[i].lx, tmpQueue[i + 1].rx, tmpQueue[i + 1].rx - tmpQueue[i].lx + 1);
                float tmpEntropy = approximateEntropy(1, 0.2, std, segVal);
                tmpQueue[i].deltaEnt = abs(tmpEntropy - (tmpQueue[i].entropy + tmpQueue[i + 1].entropy));
            }
            deltaEntropy.push_back(tmpQueue[i].deltaEnt);
        }
        float   max;
        int     pos;
        getMax(&deltaEntropy[0], (int)deltaEntropy.size(), max, pos);
        curSegment.lx = tmpQueue[pos].lx;
        curSegment.ly = tmpQueue[pos].ly;
        curSegment.rx = tmpQueue[pos + 1].rx;
        curSegment.ry = tmpQueue[pos + 1].ry;
        curSegment.mc = 0;
        curSegment.best.clear();
        vector<int> segVal = linspace<int>(tmpQueue[pos].lx, tmpQueue[pos + 1].rx, tmpQueue[pos + 1].rx - tmpQueue[pos].lx + 1);
        vector<float> data;
        reorder(dataVec, segVal, data);
        curSegment.entropy = approximateEntropy(2, 0.2, std, data);
        curSegment.deltaEnt = numeric_limits<float>::max();
        hierarchicalSegment.insert(hierarchicalSegment.begin(), curSegment);
        vector<segment> newQueue;
        if (pos > 0)
            newQueue.insert(newQueue.begin(), tmpQueue.begin(), tmpQueue.begin() + pos);
        newQueue.push_back(curSegment);
        if (pos < tmpQueue.size() - 2)
            newQueue.insert(newQueue.end(), tmpQueue.begin() + pos + 2, tmpQueue.end());
        tmpQueue = newQueue;
    }
    
    for (i = 1; i < hierarchicalSegment.size(); i++)
    {
        int tmpDur = hierarchicalSegment[i].rx - hierarchicalSegment[i].lx + 1;
        vector<int> segInd = linspace<int>(hierarchicalSegment[i].lx, hierarchicalSegment[i].rx, tmpDur);
        vector<float> segVal;
        reorder(dataVec, segInd, segVal);
        segmentSeries.push_back(segVal);
        tmpOn.push_back(hierarchicalSegment[i].lx);
        durations.push_back(tmpDur);
    }
}


