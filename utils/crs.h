/**
 *
 *  @file			crs.h
 *  @brief          Compressed row storage matrixPtr class
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__crs__
#define __Orchids__crs__

#include "Orchids.h"

crs         *crs_create (int *row, int *col, float *val, int size, int nrows, int ncols);
void        crs_destroy (crs *c);
void        crs_print   (crs *c);
void        crs_mul     (crs *c, float *v, float* &r);

#endif
