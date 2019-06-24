//
//  crs.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "crs.h"

/**
 *  Initializes a CRS
 */
crs * crs_create (int *row, int *col, float *val, int size, int nrows, int ncols)
{
    int i, current = 0, previous = 0, count = 0, prev_tmp;
	crs *c = (crs *)malloc(sizeof(crs));
	
	c->rows = nrows;
	c->columns = ncols;
	c->non_zero = size;
	c->val = (float *)malloc(c->non_zero * sizeof(float));
	c->col_ind = (int *)malloc(c->non_zero * sizeof(int));
	c->row_ptr = (int *)malloc((c->rows + 1) * sizeof(int));
	for (i = 0; i < c->rows + 1; ++i)
		c->row_ptr[i] = 0;
	
	for (i = 0; i < c->non_zero; ++i) {
		current = row[i];
		if (count < c->rows) {
			if (current - previous == 1) {
				c->row_ptr[count] = i;
				count++;
				previous = current;
			} else if (current != previous) {
				prev_tmp = previous;
				previous = current;
				while (current - prev_tmp > 0) {
					c->row_ptr[count] = i;
					count++;
					current--;
				}
			}
		}
		c->col_ind[i] = col[i] - 1;
		c->val[i] = val[i];
	}
	c->row_ptr[count] = c->non_zero;
    
    return c;
}

/**
 *  Destroyes a CRS structure, freeing the memory allocated with crs_create()
 */
void crs_destroy (crs *c)
{
    free(c->col_ind);
    free(c->row_ptr);
	free(c->val);
	free(c);
}

/**
 *  Prints a CRS structure (val, col_ind, row_ptr)
 */
void crs_print (crs *c)
{
    int i;
	cout << "Header" << endl;
	cout << c->rows << " ";
	cout << c->columns << " ";
	cout << c->non_zero << endl;
	cout << "Values - Column Number" << endl;
	for (i = 0; i < c->non_zero ; ++i)
    {
		cout << c->val[i] << " ";
		cout << c->col_ind[i] << endl;;
	}
    cout << endl;
    cout << "Row Pointer" << endl;
    for (i = 0; i < c->rows + 1; i++)
        cout << c->row_ptr[i] << " ";
    cout << endl;
}

/**
 *  Multiplies a CRS with a vector
 */
void crs_mul (crs *c, float *v, float* &r)
{
	int i, j;
	int rows = c->rows;
	float *val = c->val;
	int *row_ptr = c->row_ptr;
	int *col_ind = c->col_ind;
	
	for (i = 0; i < rows; ++i)
		r[i] = 0;
    for (i = 0; i < rows; ++i)
        for (j = row_ptr[i]; j < row_ptr[i + 1]; ++j)
            r[i] += (val[j] * v[col_ind[j]]);
}