//
//  computeNextMinima.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 05/09/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "computeNextMinima.h"


int computeNextMinima(vector<float>& a, int current)
{
    int             i, p, n = 2;
    vector<float>   aNext(a.begin() + current, a.end());
    float           nextMinVal;
    int             nextMin;
    getMin(&aNext[0], (int)aNext.size(), nextMinVal, nextMin);
    for (i = current + 1; i <= a.size() - 1; i++)
    {
        int p2;
        int forward_neighbours  = _min(n, (int)a.size() - (i + 1));
        int backward_neighbours = _min(n, i);
        int minima = 1;
        // Search all forward neighbours (up to a max of n) to see
        for (p = 1; p <= forward_neighbours; p++)
        {
            if (a[i] > a[i + p + 1])
            {
                minima = 0;
                break;
            }
            else if (a[i] == a[i + p + 1])
            {
                // See if the next change is up or down
                p2 = p;
                while (p2 < (a.size() - i - 2) && a[i] == a[i + p2 + 1])
                    p2++;
                if (a[i] > a[i + p2 + 1])
                {
                    minima = 0;
                    break;
                }
            }
        }
        // If it is greater than 1 of the forward neighbours, no need to check backwards
        if (minima != 0)
        {
            // Now test the backwards neighbours
            for (p = 1; p <= backward_neighbours; p++)
            {
                if (a[i] > a[_max(i - (p + 1), 1)])
                {
                    minima = 0;
                    break;
                }
                else if (a[i] == a[_max(i - (p + 1), 1)])
                {
                    // See if the next change is up or down
                    p2 = p;
                    while (i - (p2 + 2) > 0 && a[i] == a[_max(i - (p2 + 1), 1)])
                        p2++;
                    if (a[i] > a[_max(i - (p2 + 1), 1)])
                    {
                        minima = 0;
                        break;
                    }
                }
            }
        }
        if (minima == 1)
        {
            nextMin = i;
            break;
        }
    }
    return nextMin;
}