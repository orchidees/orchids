/**
 *
 *  @file			ccs.h
 *  @brief          Compressed column storage matrixPtr class
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__ccs__
#define __Orchids__ccs__

#include "Orchids.h"

void    swap_col (int *a, int *b);
void    swap_row (int *a, int *b);
void    swap_mat (float *a, float *b);
void    quicksort3 (float *a, int *b, int *c, int beg, int end);

ccs     *crs2ccs (crs *cr);
crs     *ccs2crs (ccs *cc);

ccs     *ccs_create (int *row, int *col, float *val, int size);
void    ccs_print (ccs *c);
void    ccs_destroy(ccs *c);
void    ccs_mul (ccs *c, float *v, float *r);


#endif
