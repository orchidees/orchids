//
//  density_PADE.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 02/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "density_PADE.h"

/*
void countAllValues(vector<int>& K, vector<int>& density, const int maxCount)
{
    int i, pos_R;
    int size = (int)K.size();
    
    // Initialize output matrix
    density.resize(maxCount, 0);
    
    // Count values
    for (i = 0; i < size; i++)
    {            
        if (K[i] > maxCount)
            printf("Error: Max count number has a too low value\n");
        pos_R = K[i] - 1;
        density[pos_R] = density[pos_R] + 1;
    }
}
*/

void countAllValues(vector<int>& K, vector<int>& density, const int maxCount)
{
    int i, pos_R;
    int size = (int)K.size();
    
    // Initialize output matrix
    density.resize(maxCount, 0);
    
    // Count values
    for (i = 0; i < size; i++)
    {
        if (K[i] > maxCount)
            printf("Error: Max count number has a too low value\n");
        pos_R = K[i] - 1;
        density[pos_R] = density[pos_R] + 1;
    }
}

void density_PADE(
                  matrixPtr         curC,
                  vector<int>&      density,
                  vector<int>&      cell_indices)
{
    int i, j;
    matrixPtr C(new matrix(curC));
    // Get number of criteria
    int nC = C->cols();
    // Get number of individuals in population
    int nK = C->rows();
    // Compute ideal point
    vector<float> ideal = C->Min(1);
    // Translate ideal point at origin
    for (i = 0; i < nK; i++)
        for (j = 0; j < nC; j++)
            C->operator()(i, j) = C->operator()(i, j) - ideal[j];
    // Compute nadir point
    vector<float> nadir = C->Max(1);
    // Compute (float) number of cells on each dimension
    float nCells = pow((float)nK, 1.0f / (float)nC);
    // Compute cell width on each dimension
    vector<float> cell_width;
    for (i = 0; i < nadir.size(); i++)
        cell_width.push_back(nadir[i] / nCells);
    // Shift population (avoid individuals on the axes)
    vector<float> shift;
    for (i = 0; i < nadir.size(); i++)
        shift.push_back(nadir[i] - floor(nCells) * cell_width[i]);
    for (i = 0; i < nK; i++)
        for (j = 0; j < nC; j++)
            C->operator()(i, j) = C->operator()(i, j) + shift[j];
    // Compute new nadir
    nadir.clear();
    nadir = C->Max(1);
    // Compute (integer) number of cells on each dimension
    int nbCells = ceil(nCells);
    // Rescale population to fit in grid
    for (i = 0; i < nK; i++)
        for (j = 0; j < nC; j++)
            C->operator()(i, j) = C->operator()(i, j) / nadir[j];
    C->operator*(nbCells);
    // Quantize population (project each individuals on the closest grid node)
    matrixIPtr grid(new matrixI(C->rows(), C->cols()));
    for (i = 0; i < grid->rows(); i++)
        for (j = 0; j < grid->cols(); j++)
        {
            grid->operator()(i, j) = ceil(C->operator()(i, j)) - 1;
            if (grid->operator()(i, j) < 0)
                grid->operator()(i, j) = 0;
        }
    // Assign a cell index to each individual
    vector<float> factor;
    float col = nC - 1;
    while (col >= 0)
        factor.push_back(powf((float)nbCells, (float)col--));
    for (i = 0; i < grid->rows(); i++)
        for (j = 0; j < grid->cols(); j++)
            grid->operator()(i, j) *= factor[j];
    cell_indices = grid->getSumVector(2);
    for (i = 0; i < cell_indices.size(); i++)
        cell_indices[i] += 1;
    // Compute total number of cells
    nbCells = pow((float)nbCells, (float)nC);
    // Compute density (count the number of occurences of
    // each cell index in the population (i.e. the density of
    // each individual is the number of individuals sharing the same cell)
    countAllValues(cell_indices, density, nbCells);
    for (i = 0; i < cell_indices.size(); i++)
        cell_indices[i] -= 1;
}



