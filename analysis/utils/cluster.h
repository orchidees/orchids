/**
 *
 *  @file			cluster.h
 *  @brief			Header definition for clustering algorithms (single, average and complete linkage)
 *
 *	This file contains the signatures and structures for performing clustering operations.
 *	All the algorithms are abstracted from the distance computation and works with the matrixPtr directly.
 *	The simple tree structure is defined to contain the result of hierarchical clustering algorithms.
 *
 *  @author			Philippe Esling
 *	@version		1.1
 *	@date			16-01-2013
 *  @copyright		UNIGE - GEN/EV (Pawlowski) - 2013
 *	@licence		MIT Media Licence
 *
 */

#ifndef CLUSTER_H_
# define CLUSTER_H_

#include "Orchids.h"

typedef struct cluster
{
	float		distance;
	int			left;
	int			right;
} cluster;

cluster			*singleLinkageClustering(float **distances, int nelements);
cluster			*averageLinkageClustering(float **distances, int nelements);
cluster			*completeLinkageClustering(float **distances, int nelements);
cluster			*centroidLinkageClustering(float **data, float **distances, int nelements, char distance);

#endif