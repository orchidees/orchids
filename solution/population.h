//
//  population.h
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__population__
#define __Orchids__population__

#include "Orchids.h"

class Solution;
class Individual;
class indexTree;

/**
 *  @class Population
 *
 *  @brief Class definition for a population of solutions
 *
 *  The population object encapsulates a whole set of solutions. It allows to perform group-based operations as well as modifications and verifications of structures
 */
class Population
{
protected:
    Session*                        sSession;       ///< Current orchestration session
    matrixPtr                       criteriaSet;    ///< Set of criteria for optimization objectives
    features                        featuresSet;    ///< Set of features for the solutions
    vector<SolutionPtr>             solutionSet;    ///< Set of found solutions
    matrixIPtr                      solutionIDs;    ///< Array of instrument ID
    
public:
    /**
     *  @brief Default Constructor
     */
    Population(){};
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current user session
     */
    Population(Session* sessObj);
    /**
     *  @brief Destructor
     */
    ~Population(){};
    /**
     *  @brief Set the current user session
     */
    void                setSession(Session* sessObj);
    /**
     *  @brief Add one solution to the solution set
     */
    void                setSolution(SolutionPtr sol);
    /**
     *  @brief Set the solutions contained in the population
     */
    void                setSolutions(vector<SolutionPtr>& solSet);
    /**
     *  @brief Set the solutions ID array
     */
    void                setSolutionsIDs(matrixIPtr solIDs);
    /**
     *  @brief Set the optimization criteria
     */
    void                setCriteria(matrixPtr critSet);
    /**
     *  @brief Set one feature
     */
    void                setFeature(string featName, matrixPtr featValue);
    /**
     *  @brief Set the features set
     */
    void                setFeatures(features& fSet);
    /**
     *  @brief Retrieve the number of solutions in the population
     */
    int                 getNbSolutions();
    /**
     *  @brief Retrieve the current solution set
     */
    vector<SolutionPtr> &getSolutions();
    /**
     *  @brief Retrieve the criteria set for current solutions
     */
    matrixPtr           getCriteria();
    /**
     *  @brief Retrieve the features of the solution set
     */
    features            &getFeatures();
    /**
     *  @brief Retrieve the solution IDs matrix
     */
    matrixIPtr          getSolutionsIDs();
    /**
     *  @brief Retrieve one solution at index id
     */
    SolutionPtr         getSolution(int id);
    
    void                createNeutralPop();
    /**
     *  @brief Extract a population given a set of indices
     */
    PopulationPtr       extractPopulation(vector<int> &loc);
    /**
     *  @brief Combine this population with another population
     */
    void                combinePopulations(PopulationPtr newPop);
    /**
     *  @brief Draw a random population where each line is an individual and each column an instrument of the orchestra
     */
    void                generateRandomPopulation(int popsize, float popSparse);
    /**
     *  @brief Draw a random population where each line is an individual and each column an instrument of the orchestra
     */
    void                generateConstrainedPopulation(int popsize, vector<matrixIPtr> &proposals, vector<int>& onsets);
    /**
     *  @brief Draw a random population where each line is an individual and each column an instrument of the orchestra
     */
    void                generateConstrainedPopulation(int popsize, vector<matrixIPtr> &proposals, vector<int>& onsets, vector<int>& durations, float popCons, float popSparse);
    /**
     *  @brief Update the current features of the population
     */
    void                updatePopulationFeatures();
    /**
     *  @brief Local solution-wise optimization of the (normally Pareto) population
     */
    void                localOptimization(sqlStruct& targetFeatures);
    /**
     *  @brief Perform a periodic harmonic fixing of the solutions
     */
    PopulationPtr       harmonicFix(sqlStruct& targetFeatures, int nbGen, int nbKept);
    /**
     *  @brief Adaptive optimization of the population
     */
    PopulationPtr       adaptiveOptimization(sqlStruct& targetFeatures, bool TSIndex, map<string, indexTreePtr>& indexes, int nbGen, int nbKept);
    /**
     *  @brief 1-point mutation operator. Input population is a double matrixPtr where each row is an individual and each column is an instrument of the orchestra. Ouput population has the size of the input population
     */
    void                mutation();
    /**
     *  @brief Optimized uniform crossover operator.
     */
    void                shuffleRows();
    /**
     *  @brief N-point mutation operator that takes into account a per-individual probability of mutation
     */
    void                mutationProbas(vector<float> &probas);
     /**
     *  @brief Selective orchestration-specific mutation operator. This ranks the solutions given the current optimization direction and then partition the space to select the mutation target based on loudness.
     */
    void                selectiveMutation(float *weights);
    /**
     *  @brief Uniform crossover operator. Input population is a double matrixPtr where each row is an individual and each column is an instrument of the orchestra. Ouput population has the size of the input population
     */
    void                crossover();
    /**
     *  @brief Optimized uniform crossover operator.
     */
    void                uniformCrossover();
    /**
     *  @brief Uniform crossover operator that takes probability into account.
     */
    void                crossoverProbas(vector<float> &probas);
    /**
     *  @brief Structural crossover operator over onsets only.
     */
    void                onsetsCrossover();
    /**
     *  @brief One-point crossover operator.
     */
    void                onePointCrossover();
    /**
     *  @brief Keep only the unique solutions in the solution set
     */
    matrixPtr           uniqueSolutions();
    /**
     *  @brief Function to preserve density and reduce size of the population
     */
    void                smoothPopulation(int popsize);
    /**
     *  @brief Remove from population every individual for which at least one criteria as a NaN or inf value
     */
    void                discardNaN();
    /**
     *  @brief Debug function to dump population to files
     */
    void                dumpPopulation(string dirname, int dump_all);
};

#endif /* defined(__Orchids__population__) */
