//
//  individual.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "individual.h"


Individual::Individual()
{
    sOnset          = 0;
    sDuration       = 0;
    sInstrument     = 0;
    sMicrotonality  = -1;
    isFilled        = false;
    isComputed      = false;
}

Individual::Individual(Individual &orig)
{
    sOnset              = orig.sOnset;
    sDuration           = orig.sDuration;
    sInstrument         = orig.sInstrument;
    sMicrotonality      = orig.sMicrotonality;
    isFilled            = orig.isFilled;
    isComputed          = orig.isComputed;
    originalFeatures    = orig.originalFeatures;
    finalFeatures       = orig.finalFeatures;
}

Individual::Individual(int id, int maxOn)
{
    sOnset          = maxOn;
    sInstrument     = id;
    sMicrotonality  = -1;
    sDuration       = 0;
    isFilled        = false;
    isComputed      = false;
}

Individual::Individual(int id, int maxOn, int maxDur)
{
    sOnset          = maxOn;
    sDuration       = maxDur;
    sInstrument     = id;
    sMicrotonality  = -1;
    isFilled        = false;
    isComputed      = false;
}

void Individual::setInstrument(int idInstru)
{
    sInstrument     = idInstru;
    isFilled        = false;
    isComputed      = false;
}

void Individual::setOnset(int onset)
{
    sOnset          = onset;
    isComputed      = false;
}

void Individual::setDuration(int dur)
{
    sDuration       = dur;
    isComputed      = false;
}

void Individual::setMicrotonality(int mic)
{
    sMicrotonality  = mic;
    //isComputed      = false;
}

void Individual::computeFinalFeatures()
{
    int j, k;
    if (isComputed)
        return;

    map<string, matrixPtr>::iterator it = originalFeatures.begin();
    for ( it = originalFeatures.begin(); it != originalFeatures.end(); ++it )
    {
        int nbTimePts = it->second->cols();
        //Column vector
        if (nbTimePts == 1 || it->first == "MelMeanAmplitude" || it->first == "PartialsMeanAmplitude")
            finalFeatures.insert(pair<string, matrixPtr>(it->first, it->second));
        else
        {
            if (it->second->rows() == 1)
            {
                //Row vector
                matrixPtr t;
                if (sOnset != 0)
                {
                    t = matrixPtr(new matrix(1, nbTimePts));
                    int index = 0;
                    if (sDuration != 0)
                    {
                        if (sDuration < 2)
                            sDuration = 2;
                        for (j = sOnset; j < nbTimePts && j < sOnset + sDuration; j++)
                            t->operator()(j) = it->second->operator()(index++);
                    }
                    else
                    {
                        for (j = sOnset; j < nbTimePts; j++)
                            t->operator()(j) = it->second->operator()(index++);
                    }
                }
                else
                    t = matrixPtr(new matrix(it->second));
                finalFeatures.insert(pair<string, matrixPtr>(it->first, t));
            }
            //Matrix
            else
            {
                matrixPtr t;
                if (sOnset != 0)
                {
                    t = matrixPtr(new matrix(it->second->rows(), it->second->cols()));
                    int col = 0;
                    for (j = 0; j < it->second->rows(); j++)
                    {
                        if (sDuration != 0)
                        {
                            if (sDuration < 2)
                                sDuration = 2;
                            for (k = (sOnset * nbTimePts) / 128; k < it->second->cols() && k < ((sOnset + sDuration) * nbTimePts) / 128; k++)
                                t->operator()(j, k) = it->second->operator()(j, col++);
                        }
                        else
                        {
                            for (k = (sOnset * nbTimePts) / 128; k < it->second->cols(); k++)
                                t->operator()(j, k) = it->second->operator()(j, col++);
                        }
                        col = 0;
                    }
                }
                else
                    t = matrixPtr(new matrix(it->second));
                finalFeatures.insert(pair<string, matrixPtr>(it->first, t));
            }
        }
    }
    isComputed = true;
}

void Individual::setFeature(string fName, matrixPtr fValue)
{
    originalFeatures.insert(pair<string, matrixPtr>(fName, fValue));
    isFilled = true;
    isComputed = false;
}

void Individual::setFeatures(map<string, matrixPtr>& featSet)
{
    //originalFeatures.clear();
    originalFeatures = featSet;
    isFilled = true;
    isComputed = false;
}

void Individual::setComputed()
{
    isFilled = true;
    isComputed = true;
}

matrixPtr Individual::getFeature(string fName)
{
    return finalFeatures[fName];
}

matrixPtr Individual::getOriginalFeature(string fName)
{
    return originalFeatures[fName];
}

map<string, matrixPtr> &Individual::getOriginalFeatures()
{
    return originalFeatures;
}

map<string, matrixPtr> &Individual::getFeatures()
{
    return finalFeatures;
}

/*
symbolics Individual::getSymbolicFeatures()
{
    return symbolicfeatures;
}
*/

bool Individual::getIsFilled()
{
    return isFilled;
}

bool Individual::getIsComputed()
{
    return isComputed;
}

int Individual::getOnset()
{
    return sOnset;
}

int Individual::getDuration()
{
    return sDuration;
}

int Individual::getMicrotonality()
{
    return sMicrotonality;
}

void Individual::dumpIndividual(string filename)
{
    boost::filesystem::path output(filename);
    boost::filesystem::path dir = output.parent_path();
    boost::filesystem::create_directories(dir);
    
    int i, j;
    /* Create the directory, read/write/access owner only. */
    FILE *f = fopen((filename + ".txt").c_str(), "w");
    fprintf(f, "N° Instrument : %i\n\n", sInstrument);
    fprintf(f, "Onset : %i\n\n", sOnset);
    fprintf(f, "Duration : %i\n\n", sDuration);
    fprintf(f, "Microtonality : %i\n\n", sMicrotonality);
    fclose(f);
    map<string, matrixPtr>::iterator it;
    for (it = originalFeatures.begin(); it != originalFeatures.end(); ++it)
    {
        FILE *f = fopen((filename + "_" + it->first + ".original.txt").c_str(), "w");
        for (i = 0; i < it->second->rows(); i++)
        {
            for (j = 0; j < it->second->cols(); j++)
                fprintf(f, "%f ", it->second->operator()(i, j));
            fprintf(f, "\n");
        }
        fclose(f);
    }
    for (it = finalFeatures.begin(); it != finalFeatures.end(); ++it)
    {
        FILE *f = fopen((filename + "_" + it->first + ".final.txt").c_str(), "w");
        for (i = 0; i < it->second->rows(); i++)
        {
            for (j = 0; j < it->second->cols(); j++)
                fprintf(f, "%f ", it->second->operator()(i, j));
            fprintf(f, "\n");
        }
        fclose(f);
    }
}