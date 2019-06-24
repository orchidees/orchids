//
//  preserveDiversity.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 16/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "preserveDiversity.h"


vector<int> preserveDiversity(matrixPtr C, int N, string mode)
{
    // Keep a backup of input set
    matrixPtr  Carchive = matrixPtr(new matrix(C));
    // Compute number of elements to remove
    int     drops = 0;
    int     Ndrop = C->rows() - N;
    int     i;
    
    //rng.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_real<float> u(0, 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > gen(rng, u);
    
    if (mode == "fitness")
    {
        
    }
    else if (mode == "pareto")
    {
        // While population is too large, remove an element in densest cell
        while (drops < Ndrop)
        {
            vector<int> keep;
            float       min;
            int         pos;
            // Keep best solutions for each criteria
            //for (i = 0; i < Carchive->cols(); i++)
            //{
            //    float* col = Carchive->getCol(i);
            //    getMin(col, Carchive->rows(), min, pos);
            //    free(col);
            //    keep.push_back(pos);
            //}
            //vecUnique(keep);
            
            // Compute PADE density
            vector<int> density, cell_indices;
            density_PADE(Carchive, density, cell_indices);
            // Find in densest cell pareto-dominated individuals
            int dmax, imax;
            getMax(&density[0], (int)density.size(), dmax, imax);
            vector<int> this_cell_I;
            for (i = 0; i < cell_indices.size(); i++)
                if (cell_indices[i] == imax)
                    this_cell_I.push_back(i);
            matrixPtr this_cell_C = Carchive->reorder2(this_cell_I);
            matrixPtr this_cell_P = extract_pareto_set(this_cell_C);
            // Pick at random a dominated individual
            int this_drop_I, random;
            if (this_cell_P->size() == this_cell_C->size())
            {
                //int limit = 0;
                //do {
                    random = floor(gen() * this_cell_I.size());
                    this_drop_I = this_cell_I[random];
                    //limit++;
                //} while (std::find(keep.begin(), keep.end(), this_drop_I) != keep.end() && limit < 10);
            }
            else
            {
                vector<int> in_pareto = this_cell_C->isMember(this_cell_P);
                vector<int> potential_drop;
                for (i = 0; i < in_pareto.size(); i++)
                    if (in_pareto[i] == 0)
                        potential_drop.push_back(this_cell_I[i]);
                //int limit = 0;
                //do {
                    random = floor(gen() * potential_drop.size());
                    this_drop_I = potential_drop[random];
                    //limit++;
                //} while (std::find(keep.begin(), keep.end(), this_drop_I) != keep.end() && limit < 10);
            }
            // Drop the picked individual
            drops++;
            Carchive->delRow(this_drop_I);
        }
    }
    else if (mode == "random")
    {
        // While population is too large, remove an element in densest cell
        while (drops < Ndrop)
        {
            vector<int> keep;
            float       min;
            int         pos;
            // Keep best solutions for each criteria
            //for (i = 0; i < Carchive->cols(); i++)
            //{
            //    float* col = Carchive->getCol(i);
            //    getMin(col, Carchive->rows(), min, pos);
            //    free(col);
            //    keep.push_back(pos);
            //}
            //vecUnique(keep);
            
            // Compute PADE density
            vector<int> density, cell_indices;
            density_PADE(Carchive, density, cell_indices);
            // Pick at random an individual in densest cell
            int dmax, imax;
            getMax(&density[0], (int)density.size(), dmax, imax);
            vector<int> this_cell_I;
            for (i = 0; i < cell_indices.size(); i++)
                if (cell_indices[i] == imax)
                    this_cell_I.push_back(i);
            int this_drop_I, random, limit = 0;
            //do {
                random = floor(gen() * this_cell_I.size());
                this_drop_I = this_cell_I[random];
            //    limit++;
            //} while (std::find(keep.begin(), keep.end(), this_drop_I) != keep.end() && limit < 10);
            // Drop the picked individual
            drops++;
            Carchive->delRow(this_drop_I);
        }
    }
    else if (mode == "norm")
    {
        
    }
    else
    {
        // Raise exception if unknown mode
        printf("*** %s : Unknown density preservation mode. ***", mode.c_str());
    }
    // Compute sampled population index vector
    return C->isMember(Carchive);
}