//
//  export.h
//  Orchids
//
//  Created by Antoine Bouchereau on 21/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__export__
#define __Orchids__export__

#include "Orchids.h"

class Session;
class Solution;

/**
 *  @class Export
 *
 *  @brief Abstract interface for exporting data
 */
class Export
{
protected:
    Session*    sSession;      ///< Current user session
    string      libraryPath;    ///< Path of sound library
    string      filename;       ///< Filename which will contains export
    
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param libPath Path of sound library
     */
    Export(Session* sessObj, string libPath): sSession(sessObj), libraryPath(libPath){};
    
    virtual void initializeExport() = 0;
    virtual void exportSingleSolution(SolutionPtr sol, string outName) = 0;
    virtual void exportSolutionSet(PopulationPtr solutionSet, string outName) = 0;
};

/**
 *  @class ExportSound
 *
 *  @brief This class allows to export solutions to a soundfile by adding the different components of any proposal
 */
class ExportSound: public Export
{
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param libPath Path of sound library
     */
    ExportSound(Session* sessObj, string libPath): Export(sessObj, libPath){};
    /**
     *  @brief Destructor
     */
    ~ExportSound(){};
    
    /**
     *  @brief Initialization
     */
    void initializeExport();
    /**
     *  @brief Export only one solution
     *  @param solution Pointer to the current solution
     */
    void exportSingleSolution(SolutionPtr sol, string outName);
    /**
     *  @brief Export all solutions contained in a Population
     *  @param solutionSet Pointer to the current population
     *  @param outName Output directory
     */
    void exportSolutionSet(PopulationPtr solutionSet, string outName);
    
    void exportMultiTargetSolution(PopulationPtr solutionSet, string outName, vector<int> segments);
};


/**
 *  @class ExportRaw
 *
 *  @brief The export follows a raw property format which allows to put every informations about a solution inside a text file
 */
class ExportRaw: public Export
{
public:
    /**
     *  @brief Constructor
     *  @param sessObj Pointer to the current session
     *  @param libPath Path of sound library
     */
    ExportRaw(Session* sessObj, string libPath): Export(sessObj, libPath){};
    /**
     *  @brief Destructor
     */
    ~ExportRaw(){};
    
    void initializeExport();
    void exportSingleSolution(SolutionPtr sol, string outName);
    void exportSolutionSet(PopulationPtr solutionSet, string outName);
    
    /**
     *  @brief Export all solutions contained in a Population
     *  @param solutionSet Pointer to the current population
     *  @param outName Output directory
     */
    void exportPopulation(Population *solutionSet, string outName);
};

#endif /* defined(__Orchids__export__) */
