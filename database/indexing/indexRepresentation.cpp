//
//  indexRepresentation.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "indexRepresentation.h"


matrix indexRepresentation(
                           vector<float>&  rawData,
                           int             winLen,
                           int             wordLen,
                           int             cardinality,
                           vector<float>&  cutPoints)
{
    int i, j, k;
    matrix binaryWord(wordLen, cardinality);
    if (cardinality > 16)
    {
        cout << "Cardinality cannot exceed 16" << endl;
        return binaryWord;
    }
    
    float winSize = (float)winLen / (float)wordLen;
    // Scan accross the time series extract sub sequences, and converting them to strings
    for (i = 0; i < rawData.size() - (winLen - 1); i++)
    {
        // Remove the current subsection
        vector<float> subSection;
        for (j = i; j < i + winLen; j++)
            subSection.push_back(rawData[j]);
        // Normalize it
        float mean = getMean(&subSection[0], (int)subSection.size());
        for (j = 0; j < subSection.size(); j++)
            subSection[j] = (subSection[j] - mean);
        float max = getMaxAbs(&subSection[0], (int)subSection.size());
        for (j = 0; j < subSection.size(); j++)
            subSection[j] = subSection[j] / max;
        // take care of the special case where there is no dimensionality reduction
        vector<float> PAA;
        if (winLen == wordLen)
            PAA = subSection;
        // Convert to PAA
        else
        {
            // winLen is not dividable by wordLen
            if (winSize - floor(winSize) > 0 )
            {
                SRC_DATA resample;
                float *signalf = (float *)malloc(subSection.size() * sizeof(float));
                std::copy(&subSection[0], &subSection[subSection.size() - 1], signalf);
                resample.data_in = signalf;
                resample.data_out = (float *)calloc(wordLen, sizeof(float));
                resample.input_frames = (float)subSection.size();
                resample.output_frames = (float)wordLen;
                resample.src_ratio = (float)wordLen / (float)winLen;
                src_simple(&resample, SRC_LINEAR, 1);
                float *expandedSubSection = (float *)malloc(wordLen * sizeof(float));
                std::copy(resample.data_out, resample.data_out + wordLen, expandedSubSection);
                free(resample.data_in);
                free(resample.data_out);
                for (j = 0; j < wordLen; j++)
                    PAA.push_back(expandedSubSection[j]);
            }
            else
            {
                for (j = 0; j < wordLen; j++)
                    PAA.push_back(subSection[j]);
            }
        }
        //binaryWord = matrix((int)PAA.size(), cardinality);
        for (j = 0; j < PAA.size(); j++)
        {
            int sum = 0;
            for (k = 0; k < cutPoints.size(); k++)
                if (cutPoints[k] <= PAA[j])
                    sum++;
            vector<int> bytes = toBytes(sum, cardinality);
            for (k = 0; k < cardinality; k++)
                binaryWord(j, k) = bytes[k];
        }
    }
    
    return binaryWord;
}

int getHash(vector<int>& binary)
{
    int i;
    vector<int> tmpHash;
    int val = (int)binary.size() - 1;
    while (val >= 0)
        tmpHash.push_back(pow(2, (float)val--));
    for (i = 0; i < tmpHash.size(); i++)
        tmpHash[i] *= binary[i];
    return getSum(&tmpHash[0], (int)tmpHash.size());
}