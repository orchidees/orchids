//
//  individual.h
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__individual__
#define __Orchids__individual__

#include "Orchids.h"

/**
 *  @class Individual
 *
 *  @brief Class definition for an individual
 *
 *  An individual is a single component of a solution. It defines all properties of an instrument or synthesizer with every possible attribute that will modify its rendering
 */
class Individual
{
protected:
    //Session*              sSession;                   ///< Pointer to the current session
    int                     sInstrument;                ///< Index of the related instrument
    map<string, matrixPtr>  originalFeatures;           ///< Original features value (unshifted)
    map<string, matrixPtr>  finalFeatures;              ///< Shifted features values
    //symbolics             symbolicfeatures;           ///< Symbolic features of individual
    bool                    isComputed;                 ///< Does the features have been computed ?
    bool                    isFilled;                   ///< Does the features have been filled ?
    int                     sOnset;                     ///< Onset value for individual
    int                     sDuration;                  ///< Duration value for individual
    int                     sMicrotonality;             ///< Detune value for individual
    
public:
    /**
     *  @brief Default constructor
     */
    Individual();
    /**
     *  @brief Copy constructor
     */
    Individual(Individual &orig);
    /**
     *  @brief Constructor
     *  @param id ID of the instrument
     *  @param maxOn Maximum onset
     */
    Individual(int id, int maxOn);
    /**
     *  @brief Constructor
     *  @param id       ID of the instrument
     *  @param maxOn    Maximum onset
     *  @param maxDur   Maximum duration
     */
    Individual(int id, int maxOn, int maxDur);
    /**
     *  @brief Destructor
     */
    ~Individual() {};
    
    /**
     *  @brief Set the current instrument
     */
    void setInstrument          (int idInstru);
    /**
     *  @brief Set the current onset
     */
    void setOnset               (int onset);
    /**
     *  @brief Set the current duration
     */
    void setDuration            (int dur);
    /**
     *  @brief Set the current microtonality
     */
    void setMicrotonality       (int mic);
    /**
     *  @brief Compute the final version of the features
     */
    void computeFinalFeatures   ();
    /**
     *  @brief Set one of the feature from knowledge
     */
    void setFeature             (string fName, matrixPtr fValue);
    /**
     *  @brief Set the original features drawn from knowledge
     */
    void setFeatures            (map<string, matrixPtr>& featSet);
    /**
     *  @brief Set this individual as computed and shifted
     */
    void setComputed            ();

    /**
     *  @brief Get one feature from the final set
     */
    matrixPtr      getFeature(string fName);
    /**
     *  @brief Get one feature from the final set
     */
    map<string, matrixPtr> &getFeatures();
    /**
     *  @brief Get one original unmodified feature
     */
    matrixPtr      getOriginalFeature(string fName);
    /**
     *  @brief Get the original unmodified features
     */
    map<string, matrixPtr> &getOriginalFeatures();
    
    //symbolics   getSymbolicFeatures();
    
    /**
     *  @brief Check if the individual features are filled
     */
    bool        getIsFilled();
    /**
     *  @brief Check if the individual features are computed
     */
    bool        getIsComputed();
    /**
     *  @brief Get current onset
     */
    int         getOnset();
    /**
     *  @brief Get current duration
     */
    int         getDuration();
    /**
     *  @brief Get current microtonality
     */
    int         getMicrotonality();
    /**
     *  @brief Get current instrument
     */
    const int   getInstrument() const { return sInstrument; };
    /**
     *  @brief Debug function to dump individual to files
     */
    void        dumpIndividual(string dirname);
};

#endif /* defined(__Orchids__individual__) */
