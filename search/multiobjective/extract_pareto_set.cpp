//
//  extract_pareto_set.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 12/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "extract_pareto_set.h"


matrixPtr extract_pareto_set(matrixPtr X)
{
    int         i, j;
    matrixPtr   paretoSet;
    int         Npoints = X->rows();
    
    if (Npoints > 0)
    {
        paretoSet = matrixPtr(new matrix(1, X->cols()));
        // Add first point to Pareto set
        for (i = 0; i < X->cols(); i++)
            paretoSet->operator()(0, i) = X->operator()(0, i);
        //vector<int> idSet;
        //idSet.push_back(1);
        for (i = 1; i < Npoints; i++)
        {
            // Remove from Pareto set all points dominated by new point
            vector<int> I = is_dominated_by(X->getRowVec(i), paretoSet, "max");
            for (j = 0; j < I.size(); j++)
                if (I[j] == 1)
                {
                    paretoSet->delRow(j);
                    //idSet.erase(idSet.begin() + j);
                }
            if (paretoSet->size() != 0)
            {
                // Add new point to Pareto set if it is not dominated by any Pareto vector
                I = is_dominated_by(X->getRowVec(i), paretoSet, "min");
                if (getMax(&I[0], (int)I.size()) == 0)
                {
                    paretoSet->quickmerge(X->getRow(i));
                    //idSet.push_back(i);
                }
            }
            else
            {
                matrixPtr tmpParetoSet(new matrix(1, X->cols()));
                for (j = 0; j < X->cols(); j++)
                    paretoSet->operator()(0, j) = X->operator()(i, j);
                paretoSet = tmpParetoSet;
                //idSet.clear();
                //idSet.push_back(i);
            }
        }
    }
    
    return paretoSet;
}