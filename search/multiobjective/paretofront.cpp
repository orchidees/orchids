//
//  paretofront.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 10/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "paretofront.h"


void paretofront(bool * front, float * M, unsigned int row, unsigned int col)
{
    unsigned int t,s,i,j,j1,j2;
    bool *checklist, coldominatedflag;
    
    checklist = (bool *)calloc(row, sizeof(bool));
    for(t = 0; t<row; t++) checklist[t] = true;
    for(s = 0; s<row; s++) {
        t=s;
        if (!checklist[t]) continue;
        checklist[t]=false;
        coldominatedflag=true;
        for(i=t+1;i<row;i++) {
            if (!checklist[i]) continue;
            checklist[i]=false;
            for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                if (M[j1] <= M[j2]) {
                    checklist[i]=true;
                    break;
                }
            }
            if (!checklist[i]) continue;
            coldominatedflag=false;
            for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                if (M[j1] > M[j2]) {
                    coldominatedflag=true;
                    break;
                }
            }
            if (!coldominatedflag) {     //swap active index continue checking
                front[t]=false;
                checklist[i]=false;
                coldominatedflag=true;
                t=i;
            }
        }
        front[t]=coldominatedflag;
        if (t>s) {
            for (i=s+1; i<t; i++) {
                if (!checklist[i]) continue;
                checklist[i]=false;
                for (j=0,j1=i,j2=t;j<col;j++,j1+=row,j2+=row) {
                    if (M[j1] <= M[j2]) {
                        checklist[i]=true;
                        break;
                    }
                }
            }
        }
    }
    free(checklist);
}