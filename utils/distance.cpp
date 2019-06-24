//
//  distance.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 28/10/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "distance.h"

float sparseness_partials = 0;

/*
 * @brief      Computes the row-wise Dynamic Time Warping (DTW) between two matrix
 *
 * For each of the row, we compute the time warping distance and then sum everything.
 * The baseline warping is 4 time points (given that partials are 64 time points).
 *
 */
float          matrixDTW(matrixPtr p1, matrixPtr p2)
{
    float      **df, res;
    float      min1, min2;
    int        i, j, r, row, col, warp;
    
    row = p1->rows(); col = p1->cols(); warp = 4;
    df = (float **)calloc(col, sizeof(float *));
    for (i = 0; i < col; i++)
        df[i] = (float *)calloc(col, sizeof(float));
    for (res = 0, r = 0; r < row; r++)
    {
        df[0][0] = sqrt(pow(p1->operator()(r, 0) - p2->operator()(r, 0), 2));
        for (i = 1; i < col; i++)
            df[i][0] = df[i - 1][0] + sqrt(p1->operator()(r, i) - p2->operator()(r, 0));
        for (j = 1; j < col; j++)
            df[0][j] = df[0][j - 1] + sqrt(p1->operator()(r, 0) - p2->operator()(r, j));
        for (i = 1; i < col; i++)
            for (j = 1; j < col; j++)
            {
                if (abs(i - j) <= warp)
                {
                    min1 = min(df[i][j - 1], df[i - 1][j]);
                    min2 = min(min1, df[i - 1][j - 1]);
                    df[i][j] = min2 + pow(p1->operator()(r, i) - p2->operator()(r, j), 2);
                }
                else
                    df[i][j] = INT_MAX;
            }
        res += sqrt(df[col - 1][col - 1]);
    }
    for (i = 0; i < col; i++)
        free(df[i]);
    free(df);
    return res;
}

/*
 * @brief      Computes the overall Hellinger distance between two matrix
 */
float          matrixHellinger(matrixPtr p1, matrixPtr p2)
{
    float      dist;
    int        i, j, row = p1->rows(), col = p1->cols();
    
    for (dist = 0, i = 0; i < row; i++)
        for (j = 0; j < col; j++)
        {
            if (p1->operator()(i, j))
                dist += pow((sqrt(p1->operator()(i, j)) - sqrt(p2->operator()(i, j))), 2);
            else
                dist += (pow((sqrt(p1->operator()(j)) - sqrt(p2->operator()(j))), 2)) * 0.1;
        }
    return (sqrt(dist) / sqrt(2));
}

/*
 * @brief      Computes the overall Kullback-Leibler distance between two matrix
 */
float          matrixKullbackLeibler(matrixPtr p1, matrixPtr p2)
{
    float      sum1, sum2, dist;
    int        i, j;
    
    for (dist = 0, i = 0; i < p1->rows(); i++)
    {
        for (sum1 = 0, sum2 = 0, j = 0; j < p1->cols(); j++)
        {
            sum1 += (p1->operator()(i, j) > 0 ? p1->operator()(i, j) : 0);
            sum2 += (p2->operator()(i, j) > 0 ? p2->operator()(i, j) : 0);
        }
        sum1 = sum1 > 0 ? sum1 : 0;
        sum2 = sum2 > 0 ? sum2 : 0;
        for (j = 0; j < p1->cols(); j++)
            if (p1->operator()(i, j) > 0 && p2->operator()(i, j) > 0)
                dist += abs((p1->operator()(i, j) / sum1) * log((p1->operator()(i, j) / sum1) / (p2->operator()(i, j) / sum2)));
    }
    return dist;
}

/*
 * @brief      Computes the row-wise Dynamic Time Warping (DTW) between two matrix
 *
 * For each of the row, we compute the time warping distance and then sum everything.
 * The baseline warping is 4 time points (given that partials are 64 time points).
 *
 */
float          vectorDTW(matrixPtr p1, matrixPtr p2)
{
    float      **df, res;
    float      min1, min2;
    int        i, j, size, warp;
    
    size = p1->size(); warp = 4;
    df = (float **)calloc(size, sizeof(float *));
    for (i = 0; i < size; i++)
        df[i] = (float *)calloc(size, sizeof(float));
    df[0][0] = sqrt(pow(p1->operator()(0) - p2->operator()(0), 2));
    for (i = 1; i < size; i++)
        df[i][0] = df[i - 1][0] + sqrt(p1->operator()(i) - p2->operator()(0));
    for (j = 1; j < size; j++)
        df[0][j] = df[0][j - 1] + sqrt(p1->operator()(0) - p2->operator()(j));
    for (i = 1; i < size; i++)
        for (j = 1; j < size; j++)
        {
            if (abs(i - j) <= warp)
            {
                min1 = MIN(df[i][j - 1], df[i - 1][j]);
                min2 = MIN(min1, df[i - 1][j - 1]);
                df[i][j] = min2 + pow(p1->operator()(i) - p2->operator()(j), 2);
            }
            else
                df[i][j] = std::numeric_limits<float>::max();
        }
    res = sqrt(df[size - 1][size - 1]);
    for (i = 0; i < size; i++)
        free(df[i]);
    free(df);
    return res;
}

/*
 * @brief      Computes the overall Hellinger distance between two matrix
 */
float          vectorHellinger(matrixPtr p1, matrixPtr p2)
{
    float      dist;
    int        j;
    
    for (dist = 0, j = 0; j < p1->size(); j++)
    {
        if (p1->operator()(j))
            dist += pow((sqrt(p1->operator()(j)) - sqrt(p2->operator()(j))), 2);
        else
            dist += (pow((sqrt(p1->operator()(j)) - sqrt(p2->operator()(j))), 2) * 0.1);
    }
    return (sqrt(dist) / sqrt(2));
}

/*
 * @brief      Computes the overall Kullback-Leibler distance between two matrix
 */
float          vectorKullbackLeibler(matrixPtr p1, matrixPtr p2)
{
    float      sum1, sum2, dist = 0;
    int        j;
    
    for (sum1 = 0, sum2 = 0, j = 0; j < p1->size(); j++)
    {
        sum1 += (p1->operator()(0, j) > 0 ? p1->operator()(0, j) : 0);
        sum2 += (p2->operator()(0, j) > 0 ? p2->operator()(0, j) : 0);
    }
    sum1 = sum1 > 0 ? sum1 : 0;
    sum2 = sum2 > 0 ? sum2 : 0;
    for (j = 0; j < p1->size(); j++)
        if (p1->operator()(0, j) > 0 && p2->operator()(0, j) > 0)
            dist += abs((p1->operator()(0, j) / sum1) * log((p1->operator()(0, j) / sum1) / (p2->operator()(0, j) / sum2)));
    return dist;
}

/*
 * @brief           Computes the overall Hellinger distance between two matrix
 */
float               vectorCorrelation(matrixPtr p1, matrixPtr p2)
{
    float           ax = 0, ay = 0, res, xv, yv, sxx = 0, syy = 0, sxy = 0, dist = 0;
    float           epsi = 1.0e-10;
    int             j;
    
    for (j = 0; j < p1->size(); j++)
    {
        if (sparseness_partials > 0 && p1->operator()(j) == 0)
            continue;
        ax += p1->operator()(j);
        ay += p2->operator()(j);
    }
    ax /= p1->size();
    ay /= p1->size();
    for (j = 0; j < p1->size(); j++)
    {
        if (sparseness_partials > 0 && p1->operator()(j) == 0)
            continue;
        xv = (p1->operator()(j) - ax);
        yv = (p2->operator()(j) - ay);
        sxx += xv * xv;
        syy += yv * yv;
        sxy += xv * yv;
        dist += pow(p1->operator()(j) - p2->operator()(j), 2);
    }
    res = sxy / (sqrt(sxx * syy) + epsi);
    return (res < 1e-5 ? (sqrt(dist) + 1.0) : ((1.0 - (res > 0 ? res : 0)) + epsi));
}


/*
 * @brief           Computes the overall Partials distance between two vectors
 */
float               vectorPartials(matrixPtr p1, matrixPtr p2)
{
    float           c = 0, a1 = 0, a2 = 0;
    float           epsi = 1.0e-10;
    int             j;
    
    for (j = 0; j < p1->size(); j++)
    {
        c += p1->operator()(j) * p2->operator()(j);
        a1 += p1->operator()(j) * p1->operator()(j);
        a2 += p2->operator()(j) * p2->operator()(j);
    }
    c /= sqrt(a1);
    c /= sqrt(a2);
    return 1 - c;
}


/*
 * @brief           Computes the overall Partials distance between two vectors
 */
float               matrixPartials(matrixPtr p1, matrixPtr p2)
{
    float           c = 0, cC = 0, a1 = 0, a2 = 0;
    int             i, j;
    
    for (i = 0; i < p1->cols(); i++)
    {
        for (j = 0, cC = 0; j < p1->rows(); j++)
        {
            cC += p1->operator()(j, i) * p2->operator()(j, i);
            a1 += p1->operator()(j, i) * p1->operator()(j, i);
            a2 += p2->operator()(j, i) * p2->operator()(j, i);
        }
        cC /= sqrt(a1);
        cC /= sqrt(a2);
        if (cC != cC)
            cC = 0;
        c += cC;
    }
    c /= p1->cols();
    return (1 - c);
}

/*
 * @brief           Computes the overall Hellinger distance between two matrix
 */
float               matrixCorrelation(matrixPtr p1, matrixPtr p2)
{
    float           ax = 0, ay = 0, res, xv, yv, sxx = 0, syy = 0, sxy = 0, dist = 0;
    float           epsi = 1.0e-8;
    int             i, j;
    
    for (i = 0; i < p1->rows(); i++)
        for (j = 0; j < p1->cols(); j++)
        {
            //if (sparseness_partials > 0 && p1->operator()(i, j) == 0)
            //    continue;
            ax += p1->operator()(i, j);
            ay += p2->operator()(i, j);
        }
    ax /= (p1->rows() * p1->cols());
    ay /= (p1->rows() * p1->cols());
    for (i = 0; i < p1->rows(); i++)
        for (j = 0; j < p1->cols(); j++)
        {
            //if (sparseness_partials > 0 && p1->operator()(i, j) == 0)
            //    continue;
            xv = (p1->operator()(i, j) - ax);
            yv = (p2->operator()(i, j) - ay);
            sxx += (xv * xv);
            syy += (yv * yv);
            sxy += (xv * yv);
            //dist += pow(p1->operator()(i, j) - p2->operator()(i, j), 2);
        }
    res = sxy / (sqrt(sxx * syy) + epsi);
    return res;
}