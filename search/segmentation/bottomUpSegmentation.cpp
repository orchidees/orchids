//
//  bottomUpSegmentation.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 11/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "bottomUpSegmentation.h"


vector<segment> bottomUpSegmentation(matrixPtr data, int num_segments)
{
    int i, j;
    vector<int> left_x, right_x;
    int val = 0;
    while (val < (data->size() - 1))
    {
        left_x.push_back(val);      // Find the left x values vector for the "fine segmented representation".
        right_x.push_back(++val);   // Find the right x values vector for the "fine segmented representation"
        val++;
    }
    // Calculate the number of segments in the initial "fine segmented representation".
    int             number_of_segments  = (int)left_x.size();
    matrixPtr       curReconstruct      = data;
    vector<float>   dataVec(data->begin(), data->end());
    
    // Initialize the segments in the "fine segmented representation".
    vector<segment> segments;
    for (i = 0; i < number_of_segments; i++)
    {
        segment s;
        s.lx = left_x[i];
        s.rx = right_x[i];
        s.mc = numeric_limits<float>::max();
        segments.push_back(s);
    }
    
    // Initialize the merge cost of the segments in the "fine segmented representation".
    for (i = 0; i < number_of_segments - 1; i++)
    {
        int size = segments[i + 1].rx - segments[i].lx + 1;
        
        vector<int>     segmented = linspace<int>(segments[i].lx, segments[i + 1].rx, size);
        vector<float>   datas;
        reorder(dataVec, segmented, datas);
        vector<float>   coef = polyfit(segmented, datas, 1);
        segments[i].best.clear();
        for (j = 0; j < size; j++)
        {
            segments[i].best.push_back((coef[0] * segmented[j]) + coef[1]);
            datas[j] = pow(datas[j] - segments[i].best[j], 2);
        }
        segments[i].mc = getSum(&datas[0], size);
    }
    
    // Keep merging the lowest cost segments until only 'num_segments' remain.
    float errRecon = 0;
    while (errRecon < 0.006 && segments.size() > num_segments)
    {
        // Find the location "i", of the cheapest merge.
        float min = segments[0].mc;
        int index = 0;
        for (i = 1; i < segments.size(); i++)
            if (segments[i].mc < min)
            {
                min = segments[i].mc;
                index = i;
            }

        // The typical case, neither of the two segments to be merged are end segments
        if (index > 0 && index < (segments.size() - 2))
        {
            int size1 = segments[index + 1].rx - segments[index].lx + 1;
            int size2 = segments[index + 2].rx - segments[index].lx + 1;
            
            vector<int>     segmented1 = linspace<int>(segments[index].lx, segments[index + 1].rx, size1);
            for (i = 0; i < size1; i++)
                curReconstruct->operator()(segmented1[i]) = segments[index].best[i];
            vector<int>     segmented2 = linspace<int>(segments[index].lx, segments[index + 2].rx, size2);
            vector<float>   datas;
            reorder(dataVec, segmented2, datas);
            vector<float>   coef = polyfit(segmented2, datas, 1);
            segments[index].best.clear();
            for (i = 0; i < size2; i++)
            {
                segments[index].best.push_back(coef[0] * segmented2[i] + coef[1]);
                datas[i] = pow(datas[i] - segments[index].best[i], 2);
            }
            segments[index].mc = getSum(&datas[0], size2);
            segments[index].rx = segments[index + 1].rx;
            segments.erase(segments.begin() + index + 1);
            
            index--;
            segmented1.clear();
            datas.clear();
            
            size1 = segments[index + 1].rx - segments[index].lx + 1;
            segmented1 = linspace<int>(segments[index].lx, segments[index + 1].rx, size1);
            reorder(dataVec, segmented1, datas);
            coef = polyfit(segmented1, datas, 1);
            segments[index].best.clear();
            for (i = 0; i < size1; i++)
            {
                segments[index].best.push_back(coef[0] * segmented1[i] + coef[1]);
                datas[i] = pow(datas[i] - segments[index].best[i], 2);
            }
            segments[index].mc = getSum(&datas[0], size1);
        }
        else if (index == 0)
        {
            // Special case: The leftmost segment must be merged.
            int size1 = segments[index + 1].rx - segments[index].lx + 1;
            int size2 = segments[index + 2].rx - segments[index].lx + 1;
            
            vector<int>     segmented1 = linspace<int>(segments[index].lx, segments[index + 1].rx, size1);
            for (i = 0; i < size1; i++)
                curReconstruct->operator()(segmented1[i]) = segments[index].best[i];
            vector<int>     segmented2 = linspace<int>(segments[index].lx, segments[index + 2].rx, size2);
            vector<float>   datas;
            reorder(dataVec, segmented2, datas);
            vector<float>   coef = polyfit(segmented2, datas, 1);
            segments[index].best.clear();
            for (i = 0; i < size2; i++)
            {
                segments[index].best.push_back(coef[0] * segmented2[i] + coef[1]);
                datas[i] = pow(datas[i] - segments[index].best[i], 2);
            }
            segments[index].mc = getSum(&datas[0], size2);
            segments[index].rx = segments[index + 1].rx;
            segments.erase(segments.begin() + index + 1);
        }
        else
        {
            // Special case: The rightmost segment must be merged.
            int size1 = segments[index + 1].rx - segments[index].lx + 1;

            vector<int> segmented = linspace<int>(segments[index].lx, segments[index + 1].rx, size1);
            for (i = 0; i < size1; i++)
                curReconstruct->operator()(segmented[i]) = segments[index].best[i];
            segments[index].rx = segments[index + 1].rx;
            segments[index].mc = numeric_limits<float>::max();
            segments.erase(segments.begin() + index + 1);
            
            index--;
            segmented.clear();

            size1 = segments[index + 1].rx - segments[index].lx + 1;
            segmented = linspace<int>(segments[index].lx, segments[index + 1].rx, size1);
            vector<float> datas;
            reorder(dataVec, segmented, datas);
            vector<float> coef = polyfit(segmented, datas, 1);
            segments[index].best.clear();
            for (i = 0; i < size1; i++)
            {
                segments[index].best.push_back(coef[0] * segmented[i] + coef[1]);
                datas[i] = pow(datas[i] - segments[index].best[i], 2);
            }
            segments[index].mc = getSum(&datas[0], size1);
        }
        matrixPtr copy(new matrix(data));
        copy->operator-(curReconstruct);
        copy->operator^(2);
        float sum   = getSum(copy->c_data(), copy->size());
        errRecon    = sqrt(sum) / copy->size();
    }
    
    //----------------------------------------------
    
    vector<float> residuals;
    for (i = 0; i < segments.size(); i++)
    {
        int size = segments[i].rx - segments[i].lx + 1;

        vector<int>     segmented = linspace<int>(segments[i].lx, segments[i].rx, size);
        vector<float>   datas;
        reorder(dataVec, segmented, datas);
        vector<float>   coef = polyfit(segmented, datas, 1);
        vector<float>   best;
        for (j = 0; j < size; j++)
        {
            best.push_back(coef[0] * segmented[j] + coef[1]);
            best[j] = pow(datas[j] - best[j], 2);
        }
        residuals.push_back(getSum(&best[0], size));
    }
    
    //----------------------------------------------

    for (i = 0; i < segments.size(); i++)
    {
        int size = segments[i].rx - segments[i].lx + 1;

        vector<int>     segmented = linspace<int>(segments[i].lx, segments[i].rx, size);
        vector<float>   datas;
        reorder(dataVec, segmented, datas);
        vector<float>   coef = polyfit(segmented, datas, 1);
        vector<float>   best;
        for (j = 0; j < size; j++)
        {
            best.push_back(coef[0] * segmented[j] + coef[1]);
            best[j] = pow(datas[j] - best[j], 2);
        }
        
        segments[i].ly = best[0];
        segments[i].ry = best[best.size() - 1];
        segments[i].mc = residuals[i];
    }
    
    return segments;
}


