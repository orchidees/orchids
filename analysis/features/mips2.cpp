//
//  Fmips2.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "mips2.h"


void mips2(
            float*          freq_v,        //Frequency vector (input)
            int             freq_vSize,
            float*          amp_v,         //Amplitude vector (input)
            int             amp_vSize,
            int             numpartialsin,  //Number of partials (input)
            int             fmin,           //Minimum frequency (input)
            float*          &freqo_v,       //Frequency vector (output)
            float*          &ampo_v,        //Amplitude vector (output)
            vector<int>     &poso_v)        //Index vector (output)
{
    //Resize output vectors
    //freqo_v =   (float *)malloc(numpartialsin * sizeof(float));
    //ampo_v =    (float *)malloc(numpartialsin * sizeof(float));
    int i, j;

    //Test frequency and amplitude size must be equal
    if (freq_vSize != amp_vSize)
    {
        cout << "Frequency and amplitude vectors are not the same size" << endl;
        int l  = min(freq_vSize, amp_vSize);
        freq_v = (float *)realloc(freq_v, l * sizeof(float));
        amp_v  = (float *)realloc(amp_v, l * sizeof(float));
        freq_vSize  = l;
        amp_vSize   = l;
    }
    
    //Retrieve index of values >= fmin
    vector<int> pos_v;
    for(i = 0; i < freq_vSize; i++)
        if (freq_v[i] >= fmin)
            pos_v.push_back(i);
    
    reorder(freq_v, freq_vSize, pos_v, freq_v);     //put values of freqi_v at index pos_v in freq_v
    reorder(amp_v, amp_vSize, pos_v, amp_v);        //put values of ampi_v at index pos_v in amp_v
    int freq_length = (int)pos_v.size();
    if (freq_length <= numpartialsin)
    {
        mySort(freq_v, freq_length, freqo_v, poso_v);
        if (freq_length == 0)
            return;
        reorder(amp_v, freq_length, poso_v, ampo_v);
        return;
    }

    //Apply HzToERB to freq_v
    float *freq_ERB = (float *)malloc(freq_length * sizeof(float));
    for (i = 0; i < freq_length; i++)
        freq_ERB[i] = hzToERB(freq_v[i], "");
    
    // We have to cluster a number of "mergeFlag" sequences
    int    mergeFlag   = freq_length;
    int    curSeq, lRead;
    float  **distances = (float **)malloc(mergeFlag * sizeof(float *));
    float  *mTable     = (float *)malloc(mergeFlag * sizeof(float));
    float  *repTable   = (float *)malloc(mergeFlag * sizeof(float));
    float  temp, comp, d;
    
    // Allocate the distance matrix
    for (curSeq = 0; curSeq	< mergeFlag; curSeq++)
        distances[curSeq] = (float *)malloc(mergeFlag * sizeof(float));
    // Here we compute the complete pairwise distance matrixPtr based on the Levenshtein distances
    for (curSeq = 0; curSeq < mergeFlag; curSeq++)
    {
        temp = freq_ERB[curSeq];
        // This is used for merging
        repTable[curSeq] = curSeq;
        mTable[curSeq] = 0;
        // Just for safety, zero in diagonal
        distances[curSeq][curSeq] = 0;
        // Iterate through all sequences remaining in the linked list
        for (lRead = (curSeq + 1); lRead < mergeFlag; lRead++)
        {
            comp = freq_ERB[lRead];
            d = abs(comp - temp);
            distances[lRead][curSeq] = d;
            distances[curSeq][lRead] = d;
        }
    }
    
    // Perform one of the clustering methods
    cluster *preclust = completeLinkageClustering(distances, mergeFlag);
    for (curSeq = 0; curSeq < mergeFlag; curSeq++)
        free(distances[curSeq]);
    free(distances);
    // Perform merging based on resulting hierarchical clustering until the number of merged cluster equals number of partials
    for (curSeq = 0; curSeq < (mergeFlag - 1) && (mergeFlag - curSeq) > (numpartialsin); curSeq++)
    {
        // Check if elements to merge are either sequences or nodes (if ID is negative, this means a node (group) to merge, otherwise it is a single element
        temp = (preclust[curSeq].left >= 0) ? repTable[preclust[curSeq].left] : mTable[-preclust[curSeq].left - 1];
        // Same check on the right element of current
        comp = (preclust[curSeq].right >= 0) ? repTable[preclust[curSeq].right] : mTable[-preclust[curSeq].right - 1];
        
        // USE AMPLITUDE TO CHOOSE BETWEEN "TEMP" and "COMP"
        /*
        int pos_temp, pos_comp;
        for (j = 0; j < freq_length; j++)
        {
            if (temp == freq_ERB[j])
                pos_temp = j;
            if (comp == freq_ERB[j])
                pos_comp = j;
        }
         */
        
        mTable[curSeq] = (amp_v[(int)temp] > amp_v[(int)comp]) ? temp : comp ;
        // Empty the previous list items
        if (preclust[curSeq].left < 0)
            mTable[-preclust[curSeq].left - 1] = 0;
        else
            repTable[preclust[curSeq].left] = 0;
        if (preclust[curSeq].right < 0)
            mTable[-preclust[curSeq].right - 1] = 0;
        else
            repTable[preclust[curSeq].right] = 0;
    }

    // Here need to check for "singleton" (in the clustering sense) sequences
    for (; curSeq < (mergeFlag - 1); curSeq++)
    {
        // Find empty position for singleton sequence on left
        if (preclust[curSeq].left >= 0)
            for (i = 0; i < mergeFlag; i++)
                if (mTable[i] == 0) { mTable[i] = repTable[preclust[curSeq].left]; break; }
        // Find empty position for singleton sequence on right
        if (preclust[curSeq].right >= 0)
            for (i = 0; i < mergeFlag; i++)
                if (mTable[i] == 0) { mTable[i] = repTable[preclust[curSeq].right]; break; }
    }
    free(preclust);
    free(repTable);
    
    //Resize the merge table
    float *mTablef = (float *)calloc(numpartialsin, sizeof(float));
    int index = 0;
    for (i = 0; i < mergeFlag; i++)
        if (mTable[i] != 0)
        {
            mTablef[index] = mTable[i];
            index++;
        }    

    //Retrieve indexes of merge table values in the frequency vector
    pos_v.clear();
    /*
    for (i = 0; i < numpartialsin; i++)
        for (j = 0; j < freq_length; j++)
            if (mTablef[i] == freq_ERB[j])
                pos_v.push_back(j);
     */
    for (i = 0; i < numpartialsin; i++)
        pos_v.push_back(mTablef[i]);
    //Reordering and sorting output vectors
    reorder (freq_v, freq_length, pos_v, freq_v);
    reorder (amp_v, freq_length, pos_v, amp_v);
    mySort  (freq_v, (int)pos_v.size(), freqo_v, poso_v);
    reorder (amp_v, (int)pos_v.size(), poso_v, ampo_v);

    if (poso_v.size() > numpartialsin)
    {
        freqo_v = (float *)realloc(freqo_v, numpartialsin * sizeof(float));
        ampo_v  = (float *)realloc(ampo_v, numpartialsin * sizeof(float));
        poso_v.resize(numpartialsin);
    }
    
    //Clean memory
    free(mTable);
    free(mTablef);
    free(freq_ERB);
}