/**
 *
 *  @file			mips2.h
 *  @brief          Compute the cluster linkage of partials
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__Fmips2__
#define __Orchids__Fmips2__

#include "Orchids.h"

/**
 *  @brief Compute the cluster linkage of partials
 *
 *  @param freqi_v Array of partials frequency
 *  @param freqi_vSize Size of the frequencies array
 *  @param ampi_v Array of partials amplitude
 *  @param ampi_vSize Size of the amplitudes array
 *  @param numpartialsin Number of partials to calculate
 *  @param fmin Minimal frequency for the partials analysis
 *  @param freqo_v Output frequencies array
 *  @param ampo_v Output amplitudes array
 *  @param poso_v Output positions vector (useful to retrieve size)
 */
void mips2(
            float           *freqi_v,        //Frequency vector (input)
            int             freqi_vSize,
            float           *ampi_v,         //Amplitude vector (input)
            int             ampi_vSize,
            int             numpartialsin,  //Number of partials (input)
            int             fmin,           //Minimum frequency (input)
            float*          &freqo_v,       //Frequency vector (output)
            float*          &ampo_v,        //Amplitude vector (output)
            vector<int>     &poso_v);       //Index vector (output)


#endif /* defined(__Orchids__Fmips2__) */
