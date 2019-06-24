//
//  BDBInterface.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 25/02/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "BDBInterface.h"


vector<string> getStaticDescriptors()
{
    string descriptorsArray[20] =
    {
        "EnergyEnvelope",
        "FundamentalFrequency",
        "HarmonicEnergy",
        /*
        "HarmonicOddToEvenRatio",
        "HarmonicSpectralCentroid",
        "HarmonicSpectralDecrease",
        "HarmonicSpectralDeviation",
        "HarmonicSpectralKurtosis",
        "HarmonicSpectralRolloff",
        "HarmonicSpectralSkewness",
        "HarmonicSpectralSlope",
        "HarmonicSpectralSpread",
        "HarmonicSpectralVariation",
        */
        "Inharmonicity",
        "Loudness",
        "NoiseEnergy",
        "Noisiness",
        "PerceptualOddToEvenRatio",
        /*
        "PerceptualSpectralCentroid", 
        "PerceptualSpectralDecrease",
        "PerceptualSpectralDeviation",
        "PerceptualSpectralKurtosis",
        "PerceptualSpectralRolloff",
        "PerceptualSpectralSkewness",
        "PerceptualSpectralSlope",
        "PerceptualSpectralSpread",
        "PerceptualSpectralVariation",
        */
        "Sharpness",
        "SignalZeroCrossingRate",
        "SpectralCentroid",
        "SpectralDecrease",
        "SpectralKurtosis",
        "SpectralRolloff",
        "SpectralSkewness",
        "SpectralSlope",
        "SpectralSpread",
        "SpectralVariation",
        "Spread",
        "TotalEnergy"
    };
    
    vector<string> descriptors(descriptorsArray, descriptorsArray + 20);
    return descriptors;
}

vector<string> getDynamicDescriptors()
{
    string descriptorsArray[8] =
    {
        "AutoCorrelation",
        "Chroma",
        "HarmonicTristimulus",
        "MFCC",
        "PerceptualTristimulus",
        "RelativeSpecificLoudness",
        "SpectralCrest",
        "SpectralFlatness"
    };
    
    vector<string> descriptors(descriptorsArray, descriptorsArray + 8);
    return descriptors;
}

vector<string> getTablesList()
{
    string fieldsArray[62] =
    {
        "name",
        "instrument",
        "note",
        "dynamics",
        "playingStyle",
        "stringMute",
        "brassMute",
        "family",
        "duration",
        "MIDIcent",
        "EnergyEnvelope",
        "EnergyEnvelopeMean",
        "FundamentalFrequency",
        "FundamentalFrequencyMean",
        "HarmonicEnergy",
        "HarmonicEnergyMean",
        /*
        "HarmonicOddToEvenRatio",
        "HarmonicOddToEvenRatioMean",
        "HarmonicSpectralCentroid",
        "HarmonicSpectralCentroidMean",
        "HarmonicSpectralDecrease",
        "HarmonicSpectralDecreaseMean",
        "HarmonicSpectralDeviation",
        "HarmonicSpectralDeviationMean",
        "HarmonicSpectralKurtosis",
        "HarmonicSpectralKurtosisMean",
        "HarmonicSpectralRolloff",
        "HarmonicSpectralRolloffMean",
        "HarmonicSpectralSkewness",
        "HarmonicSpectralSkewnessMean",
        "HarmonicSpectralSlope",
        "HarmonicSpectralSlopeMean",
        "HarmonicSpectralSpread",
        "HarmonicSpectralSpreadMean",
        "HarmonicSpectralVariation",
        "HarmonicSpectralVariationMean",
        */
        "Inharmonicity",
        "InharmonicityMean",
        "Loudness",
        "LoudnessMean",
        "NoiseEnergy",
        "NoiseEnergyMean",
        "Noisiness",
        "NoisinessMean",
        "PerceptualOddToEvenRatio",
        "PerceptualOddToEvenRatioMean",
        /*
        "PerceptualSpectralCentroid",
        "PerceptualSpectralCentroidMean",
        "PerceptualSpectralDecrease",
        "PerceptualSpectralDecreaseMean",
        "PerceptualSpectralDeviation",
        "PerceptualSpectralDeviationMean",
        "PerceptualSpectralKurtosis",
        "PerceptualSpectralKurtosisMean",
        "PerceptualSpectralRolloff",
        "PerceptualSpectralRolloffMean",
        "PerceptualSpectralSkewness",
        "PerceptualSpectralSkewnessMean",
        "PerceptualSpectralSlope",
        "PerceptualSpectralSlopeMean",
        "PerceptualSpectralSpread",
        "PerceptualSpectralSpreadMean",
        "PerceptualSpectralVariation",
        "PerceptualSpectralVariationMean",
        */
        "Sharpness",
        "SharpnessMean",
        "SignalZeroCrossingRate",
        "SignalZeroCrossingRateMean",
        "SpectralCentroid",
        "SpectralCentroidMean",
        "SpectralDecrease",
        "SpectralDecreaseMean",
        "SpectralKurtosis",
        "SpectralKurtosisMean",
        "SpectralRolloff",
        "SpectralRolloffMean",
        "SpectralSkewness",
        "SpectralSkewnessMean",
        "SpectralSlope",
        "SpectralSlopeMean",
        "SpectralSpread",
        "SpectralSpreadMean",
        "SpectralVariation",
        "SpectralVariationMean",
        "Spread",
        "SpreadMean",
        "TotalEnergy",
        "TotalEnergyMean",
        "PartialsFrequency",
        "PartialsAmplitude",
        "PartialsMeanFrequency",
        "PartialsMeanAmplitude",
        "PartialsEnergy",
        "PartialsMeanEnergy",
        "LoudnessFactor",
        "MelAmplitude",
        "MelMeanAmplitude",
        "MelEnergy",
        "MelMeanEnergy",
        "source"
    };
    // source is at the end in order to optimized the database querying
    
    vector<string> fields(fieldsArray, fieldsArray + 62);
    return fields;
}

vector<string> getTypesList()
{
    string fieldsArray[62] =
    {
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "string",
        "float",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        /*
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        */
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        /*
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        */
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "descriptor",
        "float",
        "array2D",
        "array2D",
        "array",
        "array",
        "array",
        "float",
        "float",
        "array2D",
        "array",
        "array",
        "array",
        "string"
    };
    
    vector<string> fields(fieldsArray, fieldsArray + 62);
    return fields;
}


vector<string> getSupportedFormats()
{
    string formatsArray[12] =
    {
        ".wav",
        ".wave",
        ".aif",
        ".aiff",
        ".au",
        ".mp3",
        ".WAV",
        ".WAVE",
        ".AIF",
        ".AIFF",
        ".AU",
        ".MP3"
    };
    
    vector<string> formats(formatsArray, formatsArray + 12);
    return formats;
}

symbolics setSymbolics(sqlStruct& sql)
{
    symbolics s;
    strncpy(s.name,         sql.name.c_str(),           strlen(sql.name.c_str()) + 1);
    strncpy(s.file,         sql.file.c_str(),           strlen(sql.file.c_str()) + 1);
    strncpy(s.source,       sql.source.c_str(),         strlen(sql.source.c_str()) + 1);
    strncpy(s.instrument,   sql.instrument.c_str(),     strlen(sql.instrument.c_str()) + 1);
    strncpy(s.family,       sql.family.c_str(),         strlen(sql.family.c_str()) + 1);
    strncpy(s.playingStyle, sql.playingStyle.c_str(),   strlen(sql.playingStyle.c_str()) + 1);
    strncpy(s.dynamics,     sql.dynamics.c_str(),       strlen(sql.dynamics.c_str()) + 1);
    strncpy(s.stringMute,   sql.stringMute.c_str(),     strlen(sql.stringMute.c_str()) + 1);
    strncpy(s.brassMute,    sql.brassMute.c_str(),      strlen(sql.brassMute.c_str()) + 1);
    strncpy(s.note,         sql.note.c_str(),           strlen(sql.note.c_str()) + 1);
    s.duration = sql.duration;
    s.MIDIcent = noteToMidiCent(s.note);
    return s;
}

u_int32_t getDescriptorSize(feature<float> &f)
{
    return ( sizeof(int) * 2 + sizeof(float) * 4 + f.resampled_val->bytes() );
}

u_int32_t getPartialSize(partials &p)
{
    return ( sizeof(int) * 7
            + sizeof(float)
            + p.partialsFrequency->bytes()
            + p.partialsAmplitude->bytes()
            + p.partialsMeanFrequencySize * sizeof(float)
            + p.partialsMeanAmplitudeSize * sizeof(float)
            + p.partialsEnergySize * sizeof(float) );
}

u_int32_t getMelBandSize(melStruct &m)
{
    return ( 2 * sizeof(float) + 6 * sizeof(int) + m.melAmplitude->bytes() + m.melMeanAmp->bytes() + m.melEnergy->bytes() );
}

void setData(float* &dest, float *data, int size)
{
    dest = (float *)malloc(size * sizeof(float));
    std::copy(&data[0], &data[size], dest);
}

int recordMelBands(melStruct& val, char* &buffer)
{
    int buffer_length; /* The amount of data stored in our buffer */
    char *bufferPtr; /* A pointer into the buffer */
    
    /* Initialize the buffer */
    memset(buffer, 0, getMelBandSize(val));
    
    bufferPtr = &buffer[0];
    /* Copy loudnessFactor into the buffer */
    memcpy(bufferPtr, &val.loudnessFactor, sizeof(float));
    buffer_length = sizeof(float);
    
    /* Copy the struct's rows into the buffer */
    bufferPtr = &buffer[buffer_length];
    int rows = val.melAmplitude->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's cols into the buffer */
    bufferPtr = &buffer[buffer_length];
    int cols = val.melAmplitude->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's data into the buffer */
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.melAmplitude->c_data(), val.melAmplitude->bytes());
    buffer_length += val.melAmplitude->bytes();
    
    //Copy mel mean amp
    /* Copy the struct's rows into the buffer */
    bufferPtr = &buffer[buffer_length];
    rows = val.melMeanAmp->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's cols into the buffer */
    bufferPtr = &buffer[buffer_length];
    cols = val.melMeanAmp->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's data into the buffer */
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.melMeanAmp->c_data(), val.melMeanAmp->bytes());
    buffer_length += val.melMeanAmp->bytes();
    
    //Copy mel energy
    /* Copy the struct's rows into the buffer */
    bufferPtr = &buffer[buffer_length];
    rows = val.melEnergy->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's cols into the buffer */
    bufferPtr = &buffer[buffer_length];
    cols = val.melEnergy->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    
    /* Copy the struct's data into the buffer */
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.melEnergy->c_data(), val.melEnergy->bytes());
    buffer_length += val.melEnergy->bytes();
    
    /* Copy loudnessFactor into the buffer */
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &val.melMeanEnergy, sizeof(float));
    buffer_length += sizeof(float);
    
    return buffer_length;
}

void restoreMelBands(melStruct& val, Dbt& vald)
{
    char *bufferPtr = (char *)vald.get_data(); /* A pointer into the buffer */
    
    val.loudnessFactor = *((float *)bufferPtr);
    bufferPtr += sizeof(float);
    
    val.melAmplitude = matrixPtr(new matrix());
    /* First, find the rows int (the first bit of data that we stored) */
    val.melAmplitude->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* Second, find the cols int (the first bit of data that we stored) */
    val.melAmplitude->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* And finally, the matrixPtr */
    val.melAmplitude->setData(((float *)bufferPtr), val.melAmplitude->size());
    bufferPtr += val.melAmplitude->bytes();
    val.melAmplitude->setOwnsData(true);
    
    val.melMeanAmp = matrixPtr(new matrix());
    /* First, find the rows int (the first bit of data that we stored) */
    val.melMeanAmp->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* Second, find the cols int (the first bit of data that we stored) */
    val.melMeanAmp->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* And finally, the matrixPtr */
    val.melMeanAmp->setData(((float *)bufferPtr), val.melMeanAmp->size());
    bufferPtr += val.melMeanAmp->bytes();
    val.melMeanAmp->setOwnsData(true);
    
    val.melEnergy = matrixPtr(new matrix());
    /* First, find the rows int (the first bit of data that we stored) */
    val.melEnergy->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* Second, find the cols int (the first bit of data that we stored) */
    val.melEnergy->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    /* And finally, the matrixPtr */
    val.melEnergy->setData(((float *)bufferPtr), val.melEnergy->size());
    bufferPtr += val.melEnergy->bytes();
    val.melEnergy->setOwnsData(true);
    
    val.melMeanEnergy = *((float *)bufferPtr);
}

int recordPartials(partials& val, char* &buffer)
{
    int buffer_length; /* The amount of data stored in our buffer */
    char *bufferPtr; /* A pointer into the buffer */
    
    /* Initialize the buffer */
    memset(buffer, 0, getPartialSize(val));
    //Copy partials frequency rows
    bufferPtr = &buffer[0];
    int rows = val.partialsFrequency->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length = sizeof(int);
    //Copy partials frequency cols
    bufferPtr = &buffer[buffer_length];
    int cols = val.partialsFrequency->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials frequency
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.partialsFrequency->c_data(), val.partialsFrequency->bytes());
    buffer_length += val.partialsFrequency->bytes();
    
    //Copy partials amplitude rows
    bufferPtr = &buffer[buffer_length];
    rows = val.partialsAmplitude->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials amplitude cols
    bufferPtr = &buffer[buffer_length];
    cols = val.partialsAmplitude->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials amplitude
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.partialsAmplitude->c_data(), val.partialsAmplitude->bytes());
    buffer_length += val.partialsAmplitude->bytes();
    
    //Copy partials frequency mean size
    bufferPtr = &buffer[buffer_length];
    int size = val.partialsMeanFrequencySize;
    memcpy(bufferPtr, &size, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials frequency mean
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.partialsMeanFrequency, size * sizeof(float));
    buffer_length += size * sizeof(float);
    
    //Copy partials amplitude mean size
    bufferPtr = &buffer[buffer_length];
    size = val.partialsMeanAmplitudeSize;
    memcpy(bufferPtr, &size, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials amplitude mean
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.partialsMeanAmplitude, size * sizeof(float));
    buffer_length += size * sizeof(float);
    
    //Copy partials energy size
    bufferPtr = &buffer[buffer_length];
    size = val.partialsEnergySize;
    memcpy(bufferPtr, &size, sizeof(int));
    buffer_length += sizeof(int);
    //Copy partials energy
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.partialsEnergy, size * sizeof(float));
    buffer_length += size * sizeof(float);
    
    //Copy partials energy mean
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &val.partialsMeanEnergy, sizeof(float));
    buffer_length += sizeof(float);
    
    return buffer_length;
}

void restorePartials(partials& val, Dbt& vald)
{
    char *bufferPtr = (char *)vald.get_data(); /* A pointer into the buffer */
    
    val.partialsFrequency = matrixPtr(new matrix());
    val.partialsFrequency->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.partialsFrequency->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.partialsFrequency->setData(((float *)bufferPtr), val.partialsFrequency->size());
    bufferPtr += val.partialsFrequency->bytes();
    val.partialsFrequency->setOwnsData(true);
    
    val.partialsAmplitude = matrixPtr(new matrix());
    val.partialsAmplitude->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.partialsAmplitude->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.partialsAmplitude->setData(((float *)bufferPtr), val.partialsAmplitude->size());
    bufferPtr += val.partialsAmplitude->bytes();
    val.partialsAmplitude->setOwnsData(true);
    
    val.partialsMeanFrequencySize = *((int *)bufferPtr);
    bufferPtr += sizeof(int);
    setData(val.partialsMeanFrequency, (float *)bufferPtr, val.partialsMeanFrequencySize);
    bufferPtr += val.partialsMeanFrequencySize * sizeof(float);
    
    val.partialsMeanAmplitudeSize = *((int *)bufferPtr);
    bufferPtr += sizeof(int);
    setData(val.partialsMeanAmplitude, (float *)bufferPtr, val.partialsMeanAmplitudeSize);
    bufferPtr += val.partialsMeanAmplitudeSize * sizeof(float);
    
    val.partialsEnergySize = *((int *)bufferPtr);
    bufferPtr += sizeof(int);
    setData(val.partialsEnergy, (float *)bufferPtr, val.partialsEnergySize);
    bufferPtr += val.partialsEnergySize * sizeof(float);
    
    val.partialsMeanEnergy = *((float *)bufferPtr);
}

int recordDescriptor(feature<float>& val, char* &buffer)
{
    int buffer_length = 0, rows, cols; /* The amount of data stored in our buffer */
    char *bufferPtr; /* A pointer into the buffer */
    
    /* Initialize the buffer */
    memset(buffer, 0, getDescriptorSize(val));

#ifndef DATABASE_LIGHT_VERSION
    bufferPtr = &buffer[0];
    rows = val.values.rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length = sizeof(int);
    bufferPtr = &buffer[buffer_length];
    cols = val.values.cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.values.c_data(), val.values.bytes());
    buffer_length += val.values.bytes();
#endif
  
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &val.mean, sizeof(float));
    buffer_length += sizeof(float);
    
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &val.stdDev, sizeof(float));
    buffer_length += sizeof(float);
    
#ifdef DATABASE_LIGHT_VERSION
    float max = getMax(val.values->c_data(), val.values->size());
    float min = getMin(val.values->c_data(), val.values->size());
    
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &max, sizeof(float));
    buffer_length += sizeof(float);
    
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, &min, sizeof(float));
    buffer_length += sizeof(float);
#endif
    
    bufferPtr = &buffer[buffer_length];
    rows = val.resampled_val->rows();
    memcpy(bufferPtr, &rows, sizeof(int));
    buffer_length += sizeof(int);
    bufferPtr = &buffer[buffer_length];
    cols = val.resampled_val->cols();
    memcpy(bufferPtr, &cols, sizeof(int));
    buffer_length += sizeof(int);
    bufferPtr = &buffer[buffer_length];
    memcpy(bufferPtr, val.resampled_val->c_data(), val.resampled_val->bytes());
    buffer_length += val.resampled_val->bytes();
    
    return buffer_length;
}

void restoreDescriptor(feature<float>& val, Dbt& vald)
{
    char *bufferPtr = (char *)vald.get_data(); /* A pointer into the buffer */

#ifndef DATABASE_LIGHT_VERSION
    val.values.setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.values.setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.values.setData(((float *)bufferPtr), val.values.size());
    bufferPtr += val.values.bytes();
    val.values.setOwnsData(true);
#endif
  
    val.mean = *((float *)bufferPtr);
    bufferPtr += sizeof(float);
    val.stdDev = *((float *)bufferPtr);
    bufferPtr += sizeof(float);

#ifdef DATABASE_LIGHT_VERSION
    val.max = *((float *)bufferPtr);
    bufferPtr += sizeof(float);
    val.min = *((float *)bufferPtr);
    bufferPtr += sizeof(float);
#endif

    val.resampled_val = matrixPtr(new matrix());
    val.resampled_val->setRows(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.resampled_val->setCols(*((int *)bufferPtr));
    bufferPtr += sizeof(int);
    val.resampled_val->setData(((float *)bufferPtr), val.resampled_val->size());
    val.resampled_val->setOwnsData(true);
}

vector<int> getQuantiles(string desc, int N, BDBTxn* txn)
{
    boost::filesystem::path path = getResourcesFolder<string>();
    path += "Quantiles";
    string filename = path.string() + "/" + desc + ".txt";
    // If quantiles already computed
    if (boost::filesystem::exists(filename))
    {
        // Retrieve file and deserialize it
        vector<int> freq;
        std::ifstream ifs(filename.c_str());
        boost::archive::text_iarchive ia(ifs);
        boost::serialization::load(ia, freq, 1);
        ifs.close();
        
        // If database has changed, recompute quantiles
        if (getSum(&freq[0], (int)freq.size()) == txn->key_count("melbands"))
            return freq;
        else
            boost::filesystem::remove(filename.c_str());
    }
    
    if (!boost::filesystem::exists(path))
        boost::filesystem::create_directories(boost::filesystem::path(path));

    // Compute quantiles
    cout << "Computing quantiles on " << desc << "..." << endl;
    vector<float> means;
    txn->getAllSoundsDescriptorMean( desc, means );
    vector<int> freq = histogram<float>(means, N);
    
    // Serialize data
    std::ofstream ofs(filename.c_str());
    boost::archive::text_oarchive oa(ofs);
    boost::serialization::save(oa, freq, 1);
    ofs.close();
        
    return freq;
}
