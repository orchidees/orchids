//
//  noteToHz.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

/*
 * == Parameters:
 *        +note+::        A string like: "<note>[sharpen|flatten][<octave>]" ->
 *                        "A1", "C#1", "Eb1", "A##3", "C"
 
 Use:
    for (i = 0; i<notes.size(); i++) {
        notes[i] = noteToHz(notes[i]);
    }
 
 */

#include "noteToHz.h"


float convert_note(int interval, int octave)
{
    float mul = powf(2, 1.0/12.0);
    float oct = (octave - 4) * 12;
//    if(octave > 1)
//        s *= (1 << (octave - 1));
    
    return 440.0f * powf(mul, (oct + interval));
}

float noteToHz(const char *note)
{
    int interval, octave;
    int decrease = 0, increase = 0;
    
    switch(toupper(*note)) {
        case 'A': interval = 0;  break;
        case 'B': interval = 2;  break;
        case 'C': interval = -9; break;
        case 'D': interval = -7; break;
        case 'E': interval = -5; break;
        case 'F': interval = -4; break;
        case 'G': interval = -2; break;
        default: return (float) -1;
    }
    
    if(!*(++note))        /* we'll assume the 4th octave */
        return convert_note(interval, 4);
    
    while(*note) {
        if(*note != '#' && *note != 'b') {
            note++;
            break;
        }
        
        interval += (*note == '#'? 1: -1);
/*        if(interval == -1) {
            decrease += 1;
            interval = 0x0b;
        } else if(interval == 0x0c) {
            increase += 1;
            interval = 0x00;
        }
*/
        //cout << "[I] Interval = " << interval << endl;
        note++;
    }
    /* this is ugly. */
    note--;
    
    octave = atoi(note);
    //if(octave == 0)
        //octave = 4;
    
    /* Push to A5 if G##4 was entered *
     * Drop to G4 if Abb5 was entered *
     * Do nothing if Ab# was entered */
    //octave += (increase + -decrease);
    
    /* boundries */
    if(octave < 0)
        octave = 0;
    
    if(octave > 8)
        octave = 8;
    
    return convert_note(interval, octave);
}

float noteToMidiCent(const char *note)
{
    float res = 12 * log(noteToHz(note) / 440) / log(2.0) + 69;
    return res * 100;
}