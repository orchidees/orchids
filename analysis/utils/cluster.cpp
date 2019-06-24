/**
 *
 *  @file			cluster.c
 *  @brief			Functions for clustering algorithms (single, average and complete linkage)
 *
 *	This file contains the functions for performing clustering based on a full pairwise distance matrix.
 *	All the algorithms are abstracted from the distance computation and works with the matrixPtr directly.
 *	The pairwise distance matrixPtr can be completely filled or only lower triangular.
 *	Elements in the diagonal are not taken in account.
 *
 *  @author			Philippe Esling
 *	@version		1.1
 *	@date			16-01-2013
 *  @copyright		UNIGE - GEN/EV (Pawlowski) - 2013
 *	@licence		MIT Media Licence
 *
 */

#include "cluster.h"

float		findClosestPair(int n, float **distmatrix, int *ip, int *jp)
{
	float	distance = distmatrix[1][0];
	float	temp;
	int		i, j;
	
	*ip = 1;
	*jp = 0;
	for (i = 1; i < n; i++)
	{
		for (j = 0; j < i; j++)
		{ 
			temp = distmatrix[i][j];
			if (temp < distance)
			{
				distance = temp;
				*ip = i;
				*jp = j;
			}
		}
	}
	return distance;
}

int			clustcompare(const void* a, const void* b)
{
	cluster	*clust1 = (cluster *)a;
	cluster	*clust2 = (cluster *)b;
	float	val1 = clust1->distance;
	float	val2 = clust2->distance;
	
	if (val1 < val2)
		return -1;
	if (val1 > val2)
		return +1;
	return 0;
}


cluster		*singleLinkageClustering(float **distances, int nelements)
{ 
	cluster	*result;
	float	*temp;
	int		i, j, k;
	int		nnodes = nelements - 1;
	int		*vector;
	int		*index;
	
	temp = (float *)malloc(nnodes * sizeof(float));
	index = (int *)malloc(nelements * sizeof(int));
	vector = (int *)malloc(nnodes * sizeof(int));
	result = (cluster *)malloc(nelements * sizeof(cluster));
	for (i = 0; i < nnodes; i++)
		vector[i] = i;
	for (i = 0; i < nelements; i++)
    { 
		result[i].distance = DBL_MAX;
		for (j = 0; j < i; j++)
			temp[j] = distances[i][j];
		for (j = 0; j < i; j++)
		{
			k = vector[j];
			if (result[j].distance >= temp[j])
			{
				if (result[j].distance < temp[k])
					temp[k] = result[j].distance;
				result[j].distance = temp[j];
				vector[j] = i;
			}
			else if (temp[j] < temp[k])
				temp[k] = temp[j];
		}
		for (j = 0; j < i; j++)
			if (result[j].distance >= result[vector[j]].distance)
				vector[j] = i;
    }
	free(temp);
	for (i = 0; i < nnodes; i++)
		result[i].left = i;
	qsort(result, nnodes, sizeof(cluster), clustcompare);
	for (i = 0; i < nelements; i++)
		index[i] = i;
	for (i = 0; i < nnodes; i++)
	{
		j = result[i].left;
		k = vector[j];
		result[i].left = index[j];
		result[i].right = index[k];
		index[k] = - i - 1;
	}
	free(vector);
	free(index);
	result = (cluster *)realloc(result, nnodes * sizeof(cluster));
	return result;
}

cluster		*completeLinkageClustering(float **distances, int nelements)
{
	cluster	*result;
	int		*clusterid;
	int		j, n, is, js;
	
	clusterid = (int *)malloc(nelements * sizeof(int));
	result = (cluster *)malloc((nelements - 1) * sizeof(cluster));
	/* Setup a list specifying to which cluster each element belongs */
	for (j = 0; j < nelements; j++)
		clusterid[j] = j;
	for (n = nelements; n > 1; n--)
	{
		result[nelements - n].distance = findClosestPair(n, distances, &is, &js);
		/* Fix the distances */
		for (j = 0; j < js; j++)
			distances[js][j] = max(distances[is][j], distances[js][j]);
		for (j = js+1; j < is; j++)
			distances[j][js] = max(distances[is][j], distances[j][js]);
		for (j = is+1; j < n; j++)
			distances[j][js] = max(distances[j][is], distances[j][js]);
		for (j = 0; j < is; j++)
			distances[is][j] = distances[n-1][j];
		for (j = is+1; j < n-1; j++)
			distances[j][is] = distances[n-1][j];
		/* Update clusterids */
		result[nelements-n].left = clusterid[is];
		result[nelements-n].right = clusterid[js];
		clusterid[js] = n - nelements - 1;
		clusterid[is] = clusterid[n - 1];
	}
	free(clusterid);
	return result;
}

cluster		*averageLinkageClustering(float **distances, int nelements)
{
	cluster	*result;
	int		*clusterid;
	int		*number;
	int		j, n, sum;
	int		is, js;
	
	clusterid = (int *)malloc(nelements * sizeof(int));
	number = (int *)malloc(nelements * sizeof(int));
	result = (cluster *)malloc((nelements - 1) * sizeof(cluster));
	/* Setup a list specifying to which cluster each element belongs and keep cluster cardinalities */
	for (j = 0; j < nelements; j++)
	{
		number[j] = 1;
		clusterid[j] = j;
	}
	for (n = nelements; n > 1; n--)
	{
		result[nelements - n].distance = findClosestPair(n, distances, &is, &js);
		/* Save result */
		result[nelements - n].left = clusterid[is];
		result[nelements - n].right = clusterid[js];
		/* Fix the distances */
		sum = number[is] + number[js];
		for (j = 0; j < js; j++)
		{
			distances[js][j] = distances[is][j]*number[is] + distances[js][j]*number[js];
			distances[js][j] /= sum;
		}
		for (j = js + 1; j < is; j++)
		{ 
			distances[j][js] = distances[is][j]*number[is] + distances[j][js]*number[js];
			distances[j][js] /= sum;
		}
		for (j = is + 1; j < n; j++)
		{
			distances[j][js] = distances[j][is]*number[is] + distances[j][js]*number[js];
			distances[j][js] /= sum;
		}
		for (j = 0; j < is; j++)
			distances[is][j] = distances[n - 1][j];
		for (j = is + 1; j < n - 1; j++)
			distances[j][is] = distances[n - 1][j];
		/* Update number of elements in the clusters */
		number[js] = sum;
		number[is] = number[n - 1];
		/* Update clusterids */
		clusterid[js] = n - nelements - 1;
		clusterid[is] = clusterid[n - 1];
	}
	free(clusterid);
	free(number);
	return result;
}

cluster		*centroidLinkageClustering(float **data, float **distances, int nelements, char distance)
{
	cluster	*result;
	float	**newdata = nullptr;
	int		i, inode;
	int		is, js;
	int		nnodes = nelements - 1;
	int		*distid;
	
	distid = (int *)malloc(nelements * sizeof(int));
	result = (cluster *)malloc(nnodes * sizeof(cluster));
	for (i = 0; i < nelements; i++)
		distid[i] = i;
	/* Storage for node data */
	for (i = 0; i < nelements; i++)
		memcpy(newdata[i], data[i], nelements * sizeof(float));
	data = newdata;
	for (inode = 0; inode < nnodes; inode++)
	{
		result[inode].distance = findClosestPair(nelements - inode, distances, &is, &js);
		result[inode].left = distid[js];
		result[inode].right = distid[is];
		/* Make node js the new node */
		for (i = 0; i < nelements; i++)
			data[js][i] = data[js][i] + data[is][i];
		free(data[is]);
		data[is] = data[nnodes - inode];
		/* Fix the distances */
		distid[is] = distid[nnodes - inode];
		for (i = 0; i < is; i++)
			distances[is][i] = distances[nnodes-inode][i];
		for (i = is + 1; i < nnodes-inode; i++)
			distances[i][is] = distances[nnodes-inode][i];
		distid[js] = -inode-1;
		/*
		 *
		 * Need to compute centroids for real here !
		 *
		 */
//		for (i = 0; i < js; i++)
//			distances[js][i] = metric(nelements,data,data,mask,mask,weight,js,i,0);
//		for (i = js + 1; i < nnodes-inode; i++)
//			distances[i][js] = metric(nelements,data,data,mask,mask,weight,js,i,0);
	}
	
	/* Free temporarily allocated space */
	free(data[0]);
	free(data);
	free(distid);
	return result;
}