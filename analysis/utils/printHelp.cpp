//
//  printHelp.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "printHelp.h"


void printHelp()
{
    printf("Analysis Help (v0.06)\n");
    printf("---------------------\n");
    printf("./Analysis <Input File> [-c=<Config File>.cfg] [-Option] [-o=<Output File>.xml]\n");
    printf("-c\t\t\tUse a configuration file for the analysis\n");
    printf("-Option\t\t(default: -t)\n");
    printf("\t-t\t\tLaunch analysis on a target\n");
    printf("\t-s\t\tLaunch analysis on a SOL library sound (with the filename parser)\n");
    printf("-o\t\t\tDefine an XML output. Optional (default: /Current Directory/<Input File>.xml\n");
}