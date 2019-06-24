//
//  paretoGroup.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "paretoGroup.h"


bool* paretoGroup(matrixPtr X)
{
    int i, j;
    int m = X->rows();
    int n = X->cols();
    int groupcut = floor(pow((float)2, (float)13) / n);
    int gRoup = _max(1, ceil(m / groupcut));
    bool* front = (bool *)malloc(m * sizeof(bool));
    for (i = 0; i < m; i++)
        front[i] = false;
    for (i = 0; i < gRoup; i++)
    {
        int z0 = i * groupcut;
        int limit = _min(z0 + groupcut, m);
        vector<int> z;
        for (j = z0; j < limit; j++)
            z.push_back(j);
        
        matrixPtr M = matrixPtr(new matrix(X));
        M->reorder(z);

        bool* resFront = (bool*)calloc(M->rows(), sizeof(bool));
        paretofront(resFront, M->c_data(), M->rows(), M->cols());
        for (j = 0; j < M->rows(); j++)
            front[z[j]] = resFront[j];
        
        free(resFront);
    }
    
    if (gRoup > 1)
    {
        matrixPtr M = matrixPtr(new matrix(X));
        vector<int> index;
        for (i = 0; i < m; i++)
            if (front[i])
                index.push_back(i);
        M->reorder(index);
        bool* resFront = (bool*)malloc(M->rows() * sizeof(bool));
        paretofront(resFront, M->c_data(), M->rows(), M->cols());
        for (i = 0; i < index.size(); i++)
            front[index[i]] = resFront[i];
        
        free(resFront);
    }
    
    return front;
}

