//
//  ccs.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "ccs.h"

/**
 *  Swaps two elements of NNZ
 */
void swap_col (int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}


/**
 *  Swaps two elements of ROW
 */
void swap_row (int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}


/**
 *  Swaps two elements of MAT
 */
void swap_mat (float *a, float *b)
{
	float temp = *a;
	*a = *b;
	*b = temp;
}


/**
 *  Sorts three vectors (a, b, c) using the quicksort algorithm
 */
void quicksort3 (float *a, int *b, int *c, int beg, int end)
{
	if (end > beg + 1) {
        int piv = c[beg];
		int l = beg + 1;
		int r = end;
        while (l < r) {
			if (c[l] <= piv)
                ++l;
			else {
				--r;
				swap_col(&c[l], &c[r]);
				swap_mat(&a[l], &a[r]);
				swap_row(&b[l], &b[r]);
			}
		}
		--l;
		swap_col(&c[l], &c[beg]);
		swap_mat(&a[l], &a[beg]);
		swap_row(&b[l], &b[beg]);
		quicksort3(a, b, c, beg, l);
		quicksort3(a, b, c, r, end);
	}
}


/**
 *  Creates a CCS structure from a CRS structure. See crs.h for CRS structure
 */
ccs * crs2ccs (crs *cr)
{
	ccs *cc = (ccs *)malloc(sizeof(ccs));
	cc->val = (float *)malloc(cr->non_zero * sizeof(float));
	cc->col_ptr = (int *)malloc((cr->columns + 1) * sizeof(int));
	cc->row_ind = (int *)malloc(cr->non_zero * sizeof(int));
	
	cc->rows = cr->rows;
	cc->columns = cr->columns;
	cc->non_zero = cr->non_zero;
	
    int i, j;
	for (i=0; i < cc->non_zero; ++i)
		cc->val[i] = cr->val[i];
	for (i=0; i <= cc->columns; ++i)
		cc->col_ptr[i] = 0;
	for (i=0; i < cc->non_zero; ++i)
		cc->col_ptr[cr->col_ind[i] + 1] += 1;
	for (i=0; i < cc->columns; ++i)
		cc->col_ptr[i+1] += cc->col_ptr[i];
	for (i=0; i < cc->rows; ++i)
		for (j = cr->row_ptr[i]; j < cr->row_ptr[i+1]; ++j)
			cc->row_ind[j] = i;
	int *col_ind = (int *)malloc(cr->non_zero * sizeof(int));
	for(i=0; i < cr->non_zero; ++i)
		col_ind[i] = cr->col_ind[i];
	quicksort3(cc->val, cc->row_ind, col_ind, 0, cc->non_zero);
	free(col_ind);
	return cc;
}

/**
 *  Creates a CRS structure from a CCS structure. See crs.h for CRS structure
 */
crs * ccs2crs (ccs *cc)
{
	crs *cr = (crs *)malloc(sizeof(crs));
	cr->val = (float *)malloc(cc->non_zero * sizeof(float));
	cr->row_ptr = (int *)malloc((cc->rows + 1) * sizeof(int));
	cr->col_ind = (int *)malloc(cc->non_zero * sizeof(int));
	
	cr->rows = cc->rows;
	cr->columns = cc->columns;
	cr->non_zero = cc->non_zero;
	
    int i ,j;
	for (i=0; i < cr->non_zero; ++i)
		cr->val[i] = cc->val[i];
	for (i=0; i <= cr->rows; ++i)
		cr->row_ptr[i] = 0;
	for (i=0; i < cr->non_zero; ++i)
		cr->row_ptr[cc->row_ind[i] + 1] += 1;
	for (i=0; i < cr->rows; ++i)
		cr->row_ptr[i+1] += cr->row_ptr[i];
	for (i=0; i < cr->columns; ++i)
		for (j = cc->col_ptr[i]; j < cc->col_ptr[i+1]; ++j)
			cr->col_ind[j] = i;
	int *row_ind = (int *)malloc(cc->non_zero * sizeof(int));
	for(i=0; i < cc->non_zero; ++i)
		row_ind[i] = cc->row_ind[i];
	quicksort3(cr->val, cr->col_ind, row_ind, 0, cr->non_zero);
	free(row_ind);
	return cr;
}

/**
 *  Creates and initializes a CRS
 */
ccs * ccs_create (int *row, int *col, float *val, int size)
{
	crs *cr = crs_create(row, col, val, size, getMax(row, size), getMax(col, size));
	ccs *cc = crs2ccs(cr);
	crs_destroy(cr);
	return cc;
}

/**
 *  Prints a CCS structure (val, col_ptr, row_ind)
 */
void ccs_print (ccs *c)
{
    int i;
    cout << "Header" << endl;
	cout << c->rows << " ";
	cout << c->columns << " ";
	cout << c->non_zero << endl;
	cout << "Values - Row Number" << endl;
	for (i = 0; i < c->non_zero ; ++i)
    {
		cout << c->val[i] << " ";
		cout << c->row_ind[i] << endl;;
	}
    cout << endl;
    cout << "Column Pointer" << endl;
    for (i = 0; i < c->columns + 1; i++)
        cout << c->col_ptr[i] << " ";
    cout << endl;
}


/**
 *  Destroyes a CCS structure, freeing the memory allocated with ccs_create()
 */
void ccs_destroy(ccs *c)
{
	free(c->col_ptr);
	free(c->row_ind);
	free(c->val);
	free(c);
}


/**
 *  Multiplies a CCS structure with a vector
 */
void ccs_mul (ccs *c, float *v, float *r)
{
    int i, j;
	float *val = c->val;
	int *row_ind = c->row_ind;
	int *col_ptr = c->col_ptr;
    int rows = c->rows;
	int columns = c->columns;
	float v_tmp;
	
	for (i=0; i < rows; ++i)
		r[i] = 0;
	
    for (i=0; i < columns; ++i)
    {
        v_tmp = v[i];
        for (j = col_ptr[i]; j < col_ptr[i+1]; ++j)
            r[row_ind[j]] += (val[j] * v_tmp);
    }
}