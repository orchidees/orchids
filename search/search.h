//
//  search.h
//  Orchids
//
//  Created by Antoine Bouchereau on 17/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__search__
#define __Orchids__search__

#include "Orchids.h"
#include "multiobjective/density_PADE.h"
#include "multiobjective/draw_jaszkiewicz_weights.h"
#include "multiobjective/sample_criteria_space.h"
#include "multiobjective/paretofront.h"
#include "multiobjective/paretoGroup.h"
#include "multiobjective/preserveDiversity.h"
#include "multiobjective/extract_pareto_set.h"
#include "multiobjective/is_dominated_by.h"
#include "genetic/compute_features.h"
#include "genetic/compare_features.h"
#include "genetic/eval_population.h"
#include "genetic/scalarize.h"
#include "genetic/select_population.h"
#include "genetic/fitness_advance.h"
#include "segmentation/bottomUpSegmentation.h"
#include "segmentation/approximateEntropy.h"
#include "segmentation/tsMultiLevelSegment.h"

#define indexTreePtr boost::shared_ptr<indexTree>

class Session;
class Target;
class Solution;
class Population;

/**
 *  @class Search
 *
 *  @brief Abstract interface for search algorithm
 *  This class allows to define an abstract target
 */
class Search
{
protected:
    Session*            sSession;               ///< Current user session
    vector<SolutionPtr> solutionSet;            ///< Set of solutions
    vector<float>       operatorWiseProb;       ///< Operator-wise probabilities
    bool                selectiveMutation;      ///< Perform loudness-specific mutation
    //bool                oneCross;             ///< Perform 1-pt crossover
    //bool                uniformCross;         ///< Perform uniform crossover
    //bool                onsetCross;           ///< Perform onsets crossover
    bool                globalProba;            ///< Rely on globally adaptive probabilities
    bool                harmonicFix;            ///< Rely on a harmonic fix (if PartialsMeanAmplitude)
    int                 harmonicFixFrequency;   ///< Frequency of harmonic fix
    bool                localOptim;             ///< Perform local optimization of Pareto
    bool                adaptiveOptim;          ///< Perform adaptive optimization of Pareto
    int                 adaptiveOptimFrequency; ///< Frequency of adaptive optimization
    bool                TSIndex;                ///< Use TS index in adaptive optimization
    bool                optimFullPopulation;    ///< Rely on full population or Pareto one in optim
    int                 optimPopSubsampling;    ///< Number of individuals to optimize
    int                 optimGenIndividuals;    ///< Number of optimized per individual
    int                 optimGenSubsampling;    ///< Final number of optimized kept
    float               popConstrained;         ///< Amount of constraint on population
    float               popSparsity;            ///< Amount of sparsity on population
    
public:
    /**
     *  @brief Virtual constructor
     */
    virtual ~Search(){};
    
    /**
     *  @brief Initialize the search algorithm
     */
    virtual void                        initialize() = 0;
    /**
     *  @brief Launch the search algorithm and return the solution set
     */
    virtual PopulationPtr               launchSearch(sqlStruct& targetFeatures) = 0;
    /**
     *  @brief Set the parameters of the search to default values
     */
    virtual void                        setDefaultParameters() = 0;
    /**
     *  @brief Set the parameters of the search
     */
    virtual void                        setSearchParameters(map<string, boost::any> params) = 0;
    /**
     *  @brief Set one parameter of the search
     */
    virtual void                        setParameter(string name, float value) = 0;
    /**
     *  @brief Get parameters of the search
     */
    virtual map<string, boost::any>     getSearchParameters() = 0;
    
    /**
     *  @brief Available algorithms
     */
    vector<string> availableAlgorithms()
    {
        vector<string> algs;
        algs.push_back("Sub-space genetic");
        algs.push_back("Optimal warping");
        return algs;
    }
    
    /**
     *  @brief Set a vector of solutions
     */
    void                setSolutionSet(vector<SolutionPtr>& solSet);
    /**
     *  @brief Get solutions
     */
    vector<SolutionPtr> getSolutionSet();
    
    void setSelectiveMutation(bool choice)  { selectiveMutation = choice; };
    //void setOneCross(bool choice)           { oneCross = choice; };
    //void setUniformCross(bool choice)       { uniformCross = choice; };
    //void setOnsetCross(bool choice)         { onsetCross = choice; };
    void setGlobalProba(bool choice)        { globalProba = choice; };
    void setHarmonicFix(bool choice)        { harmonicFix = choice; };
    void setLocalOptim(bool choice)         { localOptim = choice; };
    void setAdaptiveOptim(bool choice)      { adaptiveOptim = choice; };
    void setTSIndex(bool choice)            { TSIndex = choice; };
    void setHarmonicFixFreq(int val)        { harmonicFixFrequency = val; };
    void setAdaptiveOptimFreq(int val)      { adaptiveOptimFrequency = val; };
    void setOptimFullPopulation(bool c)     { optimFullPopulation = c; };
    void setOptimPopSubsampling(int val)    { optimPopSubsampling = val; };
    void setOptimGenIndividuals(int val)    { optimGenIndividuals = val; };
    void setOptimGenSubsampling(int val)    { optimGenSubsampling = val; };
    void setPopConstrained(float val)       { popConstrained = val; };
    void setPopSparsity(float val)          { popSparsity = val; };
};

/**
 *  @class SearchGenetic
 *
 *  @brief Class for the multiobjective genetic search algorithm
 *  This class allows to define a multiobjective genetic search
 */
class SearchGenetic : public Search
{
protected:
    int initPopSize;         ///< Initial population size
    int maxPopSize;          ///< Maximal population size
    int matingPoolSize;      ///< Size of mating pool
    int paretoMaxSize;       ///< Maximum size of pareto front
    int nIter;               ///< Number of evolution iterations
    
public:
    /**
     *  @brief Constructor
     *  @param s Pointer to the current user session
     */
    SearchGenetic(Session* s);
    /**
     *  @brief Destructor
     */
    ~SearchGenetic(){};
    
    /**
     *  @brief Set the parameters of the search to default values
     */
    void                        setDefaultParameters();
    /**
     *  @brief Set one parameter of the search
     */
    void                        setParameter(string name, float value);
    /**
     *  @brief Get parameters of the search
     */
    map<string, boost::any>     getSearchParameters();
    /**
     *  @brief Set the parameters of the search
     */
    void                        setSearchParameters(map<string, boost::any> params);
    /**
     *  @brief Initialize the search algorithm
     */
    void                        initialize();
    /**
     *  @brief Launch the search algorithm and return the solution set
     */
    PopulationPtr               launchSearch(sqlStruct& targetFeatures);
};


/**
 *  @class Search
 *
 *  @brief Class for the multiobjective optimal warping
 *  This class allows to define a multiobjective optimal warping search
 */
class SearchOptimalWarping : public Search
{
protected:
    float   rndFactor;           ///< Percentage of randomness
    int     nbSegBase;           ///< Number of basis segments
    int     initPopSize;         ///< Initial population size
    int     maxPopSize;          ///< Maximal population size
    int     matingPoolSize;      ///< Size of the mating pool
    int     paretoMaxSize;       ///< Size of Pareto front
    int     descIndexes;         ///< Indexes to descriptor
    int     nIter;               ///< Number of evolution iterations
    
public:
    /**
     *  @brief Constructor
     *  @param s Pointer to the current user session
     */
    SearchOptimalWarping(Session* s);
    /**
     *  @brief Destructor
     */
    ~SearchOptimalWarping(){};
    
    /**
     *  @brief Set the parameters of the search to default values
     */
    void                        setDefaultParameters();
    /**
     *  @brief Set one parameter of the search
     */
    void                        setParameter(string name, float value);
    /**
     *  @brief Get parameters of the search
     */
    map<string, boost::any>     getSearchParameters();
    /**
     *  @brief Set the parameters of the search
     */
    void                        setSearchParameters(map<string, boost::any> params);
    /**
     *  @brief Initialize the search algorithm
     */
    void                        initialize();
    /**
     *  @brief Launch the search algorithm and return the solution set
     */
    PopulationPtr               launchSearch(sqlStruct& targetFeatures);
};


#endif
