//
//  export.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 21/07/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "export.h"


void ExportSound::initializeExport()
{
    
}

void ExportSound::exportSingleSolution(SolutionPtr sol, string outName)
{    
    boost::filesystem::path output(outName);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int                 i, s, cutP;
    vector<int>::iterator it;
    vector<int>         variableTable       = sol->getIndividualsID();
    vector<bool>        isSOL;
    vector<string>      values              = sSession->getKnowledge()->getBDBConnector()->getPaths(variableTable, isSOL);
    float               tDuration           = sSession->getTarget()->getFeaturesList().duration;
    int                 targetSignalSize    = sSession->getTarget()->getSoundSize();
    int                 neutralElement      = sSession->getKnowledge()->getNeutralID();
    
    float*                  resultSignal        = (float*)calloc(tDuration * 44100, sizeof(float));
    int                     resultSignal_size   = tDuration * 44100;
    vector<IndividualPtr>   individuals         = sol->getIndividuals();
    IndividualPtr           curIndiv;
    int sID, signal_size;
    string instruPath;
    float sr_hz;
    
    for (s = 0; s < individuals.size(); s++)
    {
        curIndiv    = individuals[s];
        it          = std::find(variableTable.begin(), variableTable.end(), curIndiv->getInstrument());
        if (it != variableTable.end() && curIndiv->getInstrument() != neutralElement)
        {
            sID         = (int)(it - variableTable.begin());
            instruPath  = (isSOL[sID]) ? libraryPath + "/" + values[sID] : values[sID];
            float*  instruSignal;
            if (importSignal(instruPath, sr_hz, instruSignal, signal_size))
            {
                //float maxAbs = getMaxAbs(instruSignal, signal_size);
                //for (i = 0; i < signal_size; i++)
                //    instruSignal[i] = instruSignal[i] / maxAbs;
                int pad_size = floor(curIndiv->getOnset() * (tDuration / 128) * 44100);
                if (curIndiv->getDuration() != 0)
                    cutP = floor(curIndiv->getDuration() * (tDuration / 128) * 44100);
                for (i = cutP; i < signal_size; i++)
                    instruSignal[i] = 0;
                padarray<float>(instruSignal, signal_size, pad_size, 0, "pre");
                signal_size += pad_size;
                int lenDiff = abs(signal_size - resultSignal_size);
                if (signal_size < resultSignal_size)
                {
                    padarray<float>(instruSignal, signal_size, lenDiff, 0, "post");
                    signal_size += lenDiff;
                }
                /*
                else
                {
                    padarray<float>(resultSignal, resultSignal_size, lenDiff, 0, "post");
                    resultSignal_size += lenDiff;
                }
                 */
//                wavWrite(instruSignal, resultSignal_size, 44100, 32, (output.string() + "_ins" + boost::lexical_cast<std::string>(s) + ".wav").c_str());
                for (i = 0; i < resultSignal_size; i++)
                    resultSignal[i] += instruSignal[i];
                free(instruSignal);
            }
            else
            {
                printf("ExportWav::Error - Can't find sound file %s", instruPath.c_str());
            }
        }
    }
    // Fade out and cut
    /*
    resultSignal_size       = floor((tDuration + 0.2) * (float)targetSignalSize / tDuration);
    resultSignal            = (float*)realloc(resultSignal, resultSignal_size * sizeof(float));
    float curEnergyFactor   = 1;
    for (i = targetSignalSize; i < resultSignal_size; i++)
    {
        resultSignal[i] *= curEnergyFactor;
        curEnergyFactor -= (1 / ((float)resultSignal_size - (float)targetSignalSize));
    }
    */
    float maxAbs = getMaxAbs(resultSignal, resultSignal_size) + 0.01;
    for (i = 0; i < resultSignal_size; i++)
        resultSignal[i] = resultSignal[i] / maxAbs;
    wavWrite(resultSignal, resultSignal_size, 44100, 32, output.string().c_str());
    free(resultSignal);
}

void ExportSound::exportSolutionSet(PopulationPtr solutionSet, string outName)
{
    boost::filesystem::path output(outName);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int                     instru, s, i;
    vector<int>             variableTable;
    for (i = 0; i < solutionSet->getSolutionsIDs()->size(); i++)
        variableTable.push_back(solutionSet->getSolutionsIDs()->operator()(i));
    vecUnique(variableTable);
    vector<bool>            isSOL;
    vector<string>          values              = sSession->getKnowledge()->getBDBConnector()->getPaths(variableTable, isSOL);
    vector<SolutionPtr>     solutions           = solutionSet->getSolutions();
    float                   tDuration           = sSession->getTarget()->getFeaturesList().duration;
    int                     targetSignalSize    = sSession->getTarget()->getSoundSize();
    int                     neutralElement      = sSession->getKnowledge()->getNeutralID();
    SolutionPtr             curSol;
    vector<IndividualPtr>   individuals;
    IndividualPtr           curIndiv;
    vector<int>::iterator   it;
    int                     sID, signal_size, cutP;
    string                  instruPath;
    float                   sr_hz;
    
    for (instru = 0; instru < solutions.size(); instru++)
    {
        float* resultSignal = (float*)calloc(tDuration * 44100, sizeof(float));
        int resultSignal_size = tDuration * 44100;
        curSol      = solutions[instru];
        individuals = curSol->getIndividuals();
        for (s = 0; s < individuals.size(); s++)
        {
            curIndiv    = individuals[s];
            it          = std::find(variableTable.begin(), variableTable.end(), curIndiv->getInstrument());
            if (it != variableTable.end() && curIndiv->getInstrument() != neutralElement)
            {
                sID         = (int)(it - variableTable.begin());
                instruPath  = (isSOL[sID]) ? libraryPath + "/" + values[sID] : values[sID];
                float*  instruSignal;
                if (importSignal(instruPath, sr_hz, instruSignal, signal_size))
                {
                    //float maxAbs = getMaxAbs(instruSignal, signal_size);
                    //for (i = 0; i < signal_size; i++)
                    //    instruSignal[i] = instruSignal[i] / maxAbs;
                    int pad_size = floor(curIndiv->getOnset() * (tDuration / 128) * 44100);
                    if (curIndiv->getDuration() != 0)
                        cutP = floor(curIndiv->getDuration() * (tDuration / 128) * 44100);
                    for (i = cutP; i < signal_size; i++)
                        instruSignal[i] = 0;
                    padarray<float>(instruSignal, signal_size, pad_size, 0, "pre");
                    signal_size += pad_size;
                    int lenDiff = abs(signal_size - resultSignal_size);
                    if (signal_size < resultSignal_size)
                    {
                        padarray<float>(instruSignal, signal_size, lenDiff, 0, "post");
                        signal_size += lenDiff;
                    }
/*                    
                    else
                    {
                        padarray<float>(resultSignal, resultSignal_size, lenDiff, 0, "post");
                        resultSignal_size += lenDiff;
                    }
 */
//                    wavWrite(instruSignal, resultSignal_size, 44100, 32, (dir.string() + "/" + to_string(instru) + "/ins" + boost::lexical_cast<std::string>(s) + ".wav").c_str());
                    for (i = 0; i < resultSignal_size; i++)
                        resultSignal[i] += instruSignal[i];
                    free(instruSignal);
                }
                else
                {
                    printf("ExportWav::Error - Can't find sound file %s", instruPath.c_str());
                }
            }
        }
        // Fade out and cut
        /*
        resultSignal_size       = floor((tDuration + 0.2) * (float)targetSignalSize / tDuration);
        resultSignal            = (float*)realloc(resultSignal, resultSignal_size * sizeof(float));
        float curEnergyFactor   = 1;
        for (i = targetSignalSize; i < resultSignal_size; i++)
        {
            resultSignal[i] *= curEnergyFactor;
            curEnergyFactor -= (1 / ((float)resultSignal_size - (float)targetSignalSize));
        }
        */
        float maxAbs = getMaxAbs(resultSignal, resultSignal_size) + 0.01;
        for (i = 0; i < resultSignal_size; i++)
            resultSignal[i] = resultSignal[i] / maxAbs;
        string outfilename = dir.string() + "/" + to_string(instru) + ".wav";
        wavWrite(resultSignal, resultSignal_size, 44100, 32, outfilename.c_str());
        free(resultSignal);
        string progression = to_string(instru + 1) + "/" + to_string(solutions.size());
        cout << "Export solution " << progression << endl;
        sendOSCMessage("/Orchids/progression/exportSolutions", progression);
    }
}

void ExportSound::exportMultiTargetSolution(PopulationPtr solutionSet, string outName, vector<int> segments)
{
    boost::filesystem::path output(outName);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int                     instru, s, i;
    vector<int>             variableTable;
    for (i = 0; i < solutionSet->getSolutionsIDs()->size(); i++)
        variableTable.push_back(solutionSet->getSolutionsIDs()->operator()(i));
    vecUnique(variableTable);
    vector<bool>            isSOL;
    vector<string>          values              = sSession->getKnowledge()->getBDBConnector()->getPaths(variableTable, isSOL);
    vector<SolutionPtr>     solutions           = solutionSet->getSolutions();
    float                   tDuration           = sSession->getTarget()->getFeaturesList().duration;
    int                     targetSignalSize    = sSession->getTarget()->getSoundSize();
    int                     neutralElement      = sSession->getKnowledge()->getNeutralID();
    SolutionPtr             curSol;
    vector<IndividualPtr>   individuals;
    IndividualPtr           curIndiv;
    vector<int>::iterator   it;
    int                     sID, signal_size;
    string                  instruPath;
    float                   sr_hz;
    
    // Fill segments marker structure
    vector<pair<int, int> > segmentsMarker;
    segmentsMarker.push_back(pair<int, int>(0, segments[0]));
    for (i = 0; i < segments.size() - 1; i++)
        segmentsMarker.push_back(pair<int, int>(segments[i], segments[i + 1]));
    segmentsMarker.push_back(pair<int, int>(segments[segments.size() - 1], targetSignalSize));
    
    float*              resultSignal        = (float*)calloc(1, sizeof(float));
    int                 resultSignal_size   = 1;
    for (instru = 0; instru < solutions.size(); instru++)
    {
        curSol      = solutions[instru];
        individuals = curSol->getIndividuals();
        for (s = 0; s < individuals.size(); s++)
        {
            curIndiv    = individuals[s];
            float duration = boost::dynamic_pointer_cast<MultiTarget>(sSession->getTarget())->getSegmentDuration(instru);
            it          = std::find(variableTable.begin(), variableTable.end(), curIndiv->getInstrument());
            if (it != variableTable.end() && curIndiv->getInstrument() != neutralElement)
            {
                sID         = (int)(it - variableTable.begin());
                instruPath  = (isSOL[sID]) ? libraryPath + "/" + values[sID] : values[sID];
                float*  instruSignal;
                if (importSignal(instruPath, sr_hz, instruSignal, signal_size))
                {
                    float maxAbs = getMaxAbs(instruSignal, signal_size);
                    for (i = 0; i < signal_size; i++)
                        instruSignal[i] = instruSignal[i] / maxAbs;
                    
                    int pad_size = segmentsMarker[instru].first + floor(curIndiv->getOnset() * (duration / 128) * 44100);
                    padarray<float>(instruSignal, signal_size, pad_size, 0, "pre");
                    signal_size += pad_size;

                    // Fade out and cut instrument
                    int start = (float)segmentsMarker[instru].second - (float)segmentsMarker[instru].second * 0.2;
                    float curEnergyFactor   = 1;
                    for (i = start; i < signal_size; i++)
                    {
                        instruSignal[i] *= curEnergyFactor;
                        curEnergyFactor -= (1 / ((float)signal_size - (float)start));
                    }
                    
                    int lenDiff = abs(signal_size - resultSignal_size);
                    if (signal_size < resultSignal_size)
                    {
                        padarray<float>(instruSignal, signal_size, lenDiff, 0, "post");
                        signal_size += lenDiff;
                    }
                    else
                    {
                        padarray<float>(resultSignal, resultSignal_size, lenDiff, 0, "post");
                        resultSignal_size += lenDiff;
                    }
                    for (i = 0; i < signal_size; i++)
                        resultSignal[i] += instruSignal[i];
                    free(instruSignal);
                }
                else
                    printf("ExportWav::Error - Can't find sound file %s", instruPath.c_str());
            }
        }
    }
    // Fade out and cut
    resultSignal_size       = floor((tDuration + 0.2) * (float)targetSignalSize / tDuration);
    resultSignal            = (float*)realloc(resultSignal, resultSignal_size * sizeof(float));
    float curEnergyFactor   = 1;
    for (i = targetSignalSize; i < resultSignal_size; i++)
    {
        resultSignal[i] *= curEnergyFactor;
        curEnergyFactor -= (1 / ((float)resultSignal_size - (float)targetSignalSize));
    }

    float maxAbs = getMaxAbs(resultSignal, resultSignal_size) + 0.01;
    for (i = 0; i < resultSignal_size; i++)
        resultSignal[i] = resultSignal[i] / maxAbs;
    wavWrite(resultSignal, resultSignal_size, 44100, 32, (outName + "multiExport.wav").c_str());
    free(resultSignal);
    cout << "Export solution: finished" << endl;
}


void ExportRaw::initializeExport()
{
    
}

void ExportRaw::exportSingleSolution(SolutionPtr sol, string outName)
{
    
}

void ExportRaw::exportSolutionSet(PopulationPtr solutionSet, string outName)
{
    boost::filesystem::path output(outName);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int                     instru, s, i, ons, id;
    vector<int>             variableTable;
    for (i = 0; i < solutionSet->getSolutionsIDs()->size(); i++)
        variableTable.push_back(solutionSet->getSolutionsIDs()->operator()(i));
    vecUnique(variableTable);
    vector<bool>            isSOL;
    vector<string>          values = sSession->getKnowledge()->getBDBConnector()->getPaths(variableTable, isSOL);
    vector<SolutionPtr>     solutions = solutionSet->getSolutions();
    SolutionPtr             curSol;
    vector<IndividualPtr>   individuals;
    IndividualPtr           curIndiv;
    FILE *solFile = fopen(outName.c_str(), "w+");
    
    for (instru = 0; instru < solutions.size(); instru++)
    {
        curSol              = solutions[instru];
        individuals         = curSol->getIndividuals();
        fprintf(solFile, ">%d\n", instru);
        for (s = 0; s < individuals.size(); s++)
        {
            curIndiv = individuals[s];
            ons = curIndiv->getOnset();
            id = curIndiv->getInstrument();
            if (id == sSession->getKnowledge()->getNeutralID())
                fprintf(solFile, "-\n");
            else
            {
                Dbt key;
                init_dbt(&key, &id, sizeof(int));
                symbolics s;
                sSession->getKnowledge()->getBDBConnector()->getSymbolics("symbolics", &key, s);
                fprintf(solFile, "%d %s %s %s %s\n", ons, s.instrument, s.playingStyle, s.note, s.dynamics);
            }
        }
        string progression = to_string(instru + 1) + "/" + to_string(solutions.size());
        cout << "Export raw solution " << progression << endl;
        sendOSCMessage("/Orchids/progression/exportSolutions", progression);
    }
    fclose(solFile);
}

void ExportRaw::exportPopulation(Population *solutionSet, string outName)
{
    boost::filesystem::path output(outName);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int                     instru, s, i, ons, dur, id;
    vector<int>             variableTable;
    for (i = 0; i < solutionSet->getSolutionsIDs()->size(); i++)
    variableTable.push_back(solutionSet->getSolutionsIDs()->operator()(i));
    vecUnique(variableTable);
    vector<bool>            isSOL;
    vector<string>          values = sSession->getKnowledge()->getBDBConnector()->getPaths(variableTable, isSOL);
    vector<SolutionPtr>     solutions = solutionSet->getSolutions();
    SolutionPtr             curSol;
    vector<IndividualPtr>   individuals;
    IndividualPtr           curIndiv;
    FILE *solFile = fopen(outName.c_str(), "w+");
    
    for (instru = 0; instru < solutions.size(); instru++)
    {
        curSol              = solutions[instru];
        individuals         = curSol->getIndividuals();
        fprintf(solFile, ">%d\n", instru);
        for (s = 0; s < individuals.size(); s++)
        {
            curIndiv = individuals[s];
            ons = curIndiv->getOnset();
            dur = curIndiv->getDuration();
            id = curIndiv->getInstrument();
            if (id == sSession->getKnowledge()->getNeutralID())
            fprintf(solFile, "-\n");
            else
            {
                Dbt key;
                init_dbt(&key, &id, sizeof(int));
                symbolics s;
                sSession->getKnowledge()->getBDBConnector()->getSymbolics("symbolics", &key, s);
                fprintf(solFile, "%d %d %s %s %s %s\n", ons, dur, s.instrument, s.playingStyle, s.note, s.dynamics);
            }
        }
    }
    fclose(solFile);
}