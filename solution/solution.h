//
//  solution.h
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__solution__
#define __Orchids__solution__

#include "Orchids.h"

class Individual;

/**
 *  @class Solution
 *
 *  @brief Class definition for a single solution
 *
 *  A solution is defined as a mixture of individuals
 */
class Solution
{
protected:
    Session*                sSession;               ///< Current orchestration session
    vector<IndividualPtr>   individualsSet;         ///< Individuals that compose the mixture
    vector<int>             individualsID;          ///< Individuals IDs
    map<string, matrixPtr>  mixFeatures;            ///< Features for this solution
    matrixPtr               mixCriteria;            ///< Value of criteria for this solution
    bool                    isComputed;             ///< Boolean to check if solution has been computed
    bool                    isLocallyOptimized;     ///< Boolean to check if local optimization was performed
    
public:
    /**
     *  @brief Default Constructor
     */
    Solution(){};
    /**
     *  @brief Constructor
     *  @param size Number of solution
     */
    Solution(Session* sessObj, int size);
    /**
     *  @brief Copy constructor
     *  @param sol Solution to copy
     */
    Solution(const Solution& sol);
    /**
     *  @brief Destructor
     */
    ~Solution() {};
    
    /**
     *  @brief Modify one individual of the solution
     */
    void setIndividual      (int id, IndividualPtr iValue, int idInst);
    /**
     *  @brief Set all individuals of the solution
     */
    void setIndividuals     (const vector<IndividualPtr>& indivSet);
    /**
     *  @brief Set one feature of this solution
     */
    void setSolutionFeature (string fName, matrixPtr fValue);
    /**
     *  @brief Set all features of this solution
     */
    void setFeatures        (map<string, matrixPtr>& fSet);
    /**
     *  @brief Set the criteria values for this solution
     */
    void setCriteria        (matrixPtr cValue);
    /**
     *  @brief Set computed to true
     */
    void setComputed        ();
    /**
     *  @brief Set that this solution has been locally optimized
     */
    void setLocallyOptimized();
    
    /**
     *  @brief Empty all individuals of the solution
     */
    void                    emptyIndividuals();
    /**
     *  @brief Retrieve the number of individuals in the mixture (including neutral elements)
     */
    int                     getNbInstruments();
    /**
     *  @brief Get an individual at index id
     */
    IndividualPtr           getIndividual(int id);
    /**
     *  @brief Retrieve the individuals for this solution
     */
    vector<IndividualPtr>   &getIndividuals();
    /**
     *  @brief Get a list of individuals IDs
     */
    vector<int>             &getIndividualsID();
    /**
     *  @brief Get the individual feature
     */
    matrixPtr                  getIndividualFeature(string fName);
    /**
     *  @brief Retrieve the features of this solution
     */
    map<string, matrixPtr>     &getSolutionFeatures();
    /**
     *  @brief Retrieve the features of this solution
     */
    matrixPtr                  getSolutionFeature(string fName);
    /**
     *  @brief Retrieve the criteria values of this solution
     */
    matrixPtr                  getSolutionCriteria();
    /**
     *  @brief Check if solution has been computed
     */
    bool                    getIsComputed();
    /**
     *  @brief Check if solution has been computed
     */
    bool                    getLocallyOptimized();
    /**
     *  @brief Debug function to dump solution to files
     */
    void                    dumpSolution(string dirname);
};

#endif /* defined(__Orchids__solution__) */
