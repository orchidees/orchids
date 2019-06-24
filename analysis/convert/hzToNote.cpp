//
//  hzToNote.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "hzToNote.h"

#define chroma_size 24


string hzToNote(float hz)
{
    string note;
    if (hz > 0)
    {
        string chromalist[chroma_size] = {"C", "C+", "C#", "C#+", "D", "D+", "D#", "D#+", "E", "E+", "F", "F+", "F#", "F#+", "G", "G+", "G#", "G#+", "A", "A+", "A#", "A#+", "B", "B+"};
        float C1 = 32.7032;
        float o = log2(hz / C1) + 1 / 48;
        int n = floor(o);
        int chroma = ceil((o - n) * 24);
        int oct = floor(o) + 1;
        if (chroma >= chroma_size)
            oct++;
        note = chromalist[chroma % chroma_size] + to_string(oct);
        size_t pos = note.find("+");
        if (pos != string::npos)
        {
            note.erase(note.begin() + pos);
            note += "+";
        }
    }else
        note = "NaN";
    
    return note;
}