//
//  exportXML.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "exportXML.h"

void writeXMLHeader(FILE *f)
{
    fseek(f, 0, SEEK_END);
    fprintf(f, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<Mpeg7 xmlns=\"urn:mpeg:mpeg7:schema:2001\"\n:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\nxmlns:mpeg7=\"urn:mpeg:mpeg7:schema:2001\"\nxsi:schemaLocation=\"urn:mpeg:mpeg7:schema:2001 Mpeg7-2001.xsd\">\n<Description xsi:type=\"ContentEntityType\">\n<MultimediaContent xsi:type=\"AudioType\">\n<Audio xsi:type=\"AudioSegmentType\">\n\n");
}

void writeMatrixtoXML(matrixPtr m, FILE *f)
{
    int a, b;
    for(a = 0; a < m->rows(); a++)
    {
        for(b = 0; b < m->cols(); b++)
            fprintf(f, "%lf ", m->operator()(a, b));
        fprintf(f, "\n");
    }
}

void writeDescriptortoXML(feature<float> descriptor, FILE *f)
{
    int i;
    fseek(f, 0, SEEK_END);
    fprintf(f, "<AudioDescriptor xsi:type=\"%s\">\n", descriptor.name.c_str());
    
    if (descriptor.values->cols() > 1)
    {
        //matrixPtr case
        fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", descriptor.values->cols(), descriptor.values->rows());
        fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", descriptor.values->rows(), descriptor.values->cols());
        if (descriptor.values->size() == 0)
            fprintf(f, "%d ", 0);
        else
            writeMatrixtoXML(descriptor.values, f);
        fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n");
    }
    else
    {
        //Vector case
        //Export Mean and stdDev
        fprintf(f, "\t\t<Mean>  %f</Mean>\n", descriptor.mean);
        fprintf(f, "\t\t<StandardDeviation>  %f</StandardDeviation>\n", descriptor.stdDev);
        
        //Export original values
        fprintf(f, "\t\t<SeriesOfScalar hopSize=\"PT10N1000F\" totalNumOfSamples=\"%d\" >\n\t\t<Raw> ", descriptor.values->rows());
        if (descriptor.values->rows() == 0)
            fprintf(f, "%d ", 0);
        else
            for (i = 0; i < descriptor.values->rows(); i++)
                fprintf(f, "%f ", descriptor.values->operator()(i, 0));
        fprintf(f, "\n\t\t</Raw>\n\t</SeriesOfScalar>\n");

        //Export resampled values
        fprintf(f, "\t<SeriesOfScalar hopSize=\"PT10N1000F\" totalNumOfSamples=\"%d\" >\n\t\t<Resampled> ", descriptor.resampled_val->rows());
        if (descriptor.resampled_val->rows() == 0)
            fprintf(f, "%d ", 0);
        else
            for (i = 0; i < descriptor.resampled_val->rows(); i++)
                fprintf(f, "%f ", descriptor.resampled_val->operator()(i, 0));
        fprintf(f, "\n\t\t</Resampled>\n\t</SeriesOfScalar>\n");
    }
    
    fprintf(f, "</AudioDescriptor>\n\n");
}

void writePartialstoXML(partials partial, FILE *f)
{
    int i, j;
    fseek(f, 0, SEEK_END);
    
    //Export Partials frequency
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsFrequency\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", partial.partialsFrequency->cols(), partial.partialsFrequency->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", partial.partialsFrequency->rows(), partial.partialsFrequency->cols());
    if (partial.partialsFrequency->size() == 0)
        fprintf(f, "%d ", 0);
    else
        writeMatrixtoXML(partial.partialsFrequency, f);
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n");
    
    //Export Symbolic Notes
    fprintf(f, "<AudioDescriptor xsi:type=\"SymbolicNotes\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", partial.partialsFrequency->cols(), partial.partialsFrequency->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", partial.partialsFrequency->rows(), partial.partialsFrequency->cols());
    if (partial.symbolicNotes.size() == 0)
        fprintf(f, "%d ", 0);
    else
        for (i = 0; i < partial.partialsFrequency->rows(); i++)
        {
            for (j = 0; j < partial.partialsFrequency->cols(); j++)
                fprintf(f, "%s ", partial.symbolicNotes[partial.partialsFrequency->cols() * i + j].c_str());
            fprintf(f, "\n");
        }
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n"); 
    
    //Export Partials amplitude
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsAmplitude\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", partial.partialsAmplitude->cols(), partial.partialsAmplitude->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", partial.partialsAmplitude->rows(), partial.partialsAmplitude->cols());
    if (partial.partialsAmplitude->size() == 0)
        fprintf(f, "%d ", 0);
    else
        writeMatrixtoXML(partial.partialsAmplitude, f);
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n");
    
    //Export Partials mean frequency
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsMeanFrequency\">\n");
    fprintf(f, "\t\t<SeriesOfScalar hopSize=\"PT10N1000F\" totalNumOfSamples=\"%d\" >\n\t\t<Raw> ", partial.partialsMeanFrequencySize);
    if (partial.partialsMeanFrequencySize == 0)
        fprintf(f, "%d ", 0);
    else
        for (i = 0; i < partial.partialsMeanFrequencySize; i++)
            fprintf(f, "%f ", partial.partialsMeanFrequency[i]);
    fprintf(f, "\n\t\t</Raw>\n\t</SeriesOfScalar>\n</AudioDescriptor>\n\n");
    
    //Export Partials mean anplitude
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsMeanAmplitude\">\n");
    fprintf(f, "\t\t<SeriesOfScalar hopSize=\"PT10N1000F\" totalNumOfSamples=\"%d\" >\n\t\t<Raw> ", partial.partialsMeanAmplitudeSize);
    if (partial.partialsMeanAmplitudeSize == 0)
        fprintf(f, "%d ", 0);
    else
        for (i = 0; i < partial.partialsMeanAmplitudeSize; i++)
            fprintf(f, "%f ", partial.partialsMeanAmplitude[i]);
    fprintf(f, "\n\t\t</Raw>\n\t</SeriesOfScalar>\n</AudioDescriptor>\n\n");
    
    //Export Partials energy
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsEnergy\">\n");
    fprintf(f, "\t\t<SeriesOfScalar hopSize=\"PT10N1000F\" totalNumOfSamples=\"%d\" >\n\t\t<Raw> ", partial.partialsEnergySize);
    if (partial.partialsEnergySize == 0)
        fprintf(f, "%d ", 0);
    else
        for (i = 0; i < partial.partialsEnergySize; i++)
            fprintf(f, "%f ", partial.partialsEnergy[i]);
    fprintf(f, "\n\t\t</Raw>\n\t</SeriesOfScalar>\n</AudioDescriptor>\n\n");
    
    //Export Partials mean energy
    fprintf(f, "<AudioDescriptor xsi:type=\"PartialsMeanEnergy\">\n");
    fprintf(f, "\t\t<Scalar>  %f</Scalar>\n", partial.partialsMeanEnergy);
    fprintf(f, "</AudioDescriptor>\n\n");
}

void writeMelBandstoXML(melStruct mel, FILE *f)
{
    fseek(f, 0, SEEK_END);
    
    //Export Mel Amplitude
    fprintf(f, "<AudioDescriptor xsi:type=\"MelAmplitude\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", mel.melAmplitude->cols(), mel.melAmplitude->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", mel.melAmplitude->rows(), mel.melAmplitude->cols());
    if (mel.melAmplitude->size() == 0)
        fprintf(f, "%d ", 0);
    else
        writeMatrixtoXML(mel.melAmplitude, f);
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n");
    
    //Export Mel Mean Amplitude
    fprintf(f, "<AudioDescriptor xsi:type=\"MelMeanAmplitude\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", mel.melMeanAmp->cols(), mel.melMeanAmp->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", mel.melMeanAmp->rows(), mel.melMeanAmp->cols());
    if (mel.melMeanAmp->size() == 0)
        fprintf(f, "%d ", 0);
    else
        writeMatrixtoXML(mel.melMeanAmp, f);
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n");
    
    //Export Mel Energy
    fprintf(f, "<AudioDescriptor xsi:type=\"MelEnergy\">\n");
    fprintf(f, "\t<SeriesOfVector hopSize=\"PT10N1000F\" vectorSize=\"%d\" totalNumOfSamples=\"%d\" >\n", mel.melEnergy->cols(), mel.melEnergy->rows());
    fprintf(f, "\t\t<Raw mpeg7:dim=\"%d %d\"> ", mel.melEnergy->rows(), mel.melEnergy->cols());
    if (mel.melEnergy->size() == 0)
        fprintf(f, "%d ", 0);
    else
        writeMatrixtoXML(mel.melEnergy, f);
    fprintf(f, "\t\t</Raw>\n\t</SeriesOfVector>\n</AudioDescriptor>\n\n");
    
    //Export mel mean energy
    fprintf(f, "<AudioDescriptor xsi:type=\"MelMeanEnergy\">\n");
    fprintf(f, "\t\t<Scalar>  %f</Scalar>\n", mel.melMeanEnergy);
    fprintf(f, "</AudioDescriptor>\n\n");
    
    //Export loudness factor
    fprintf(f, "<AudioDescriptor xsi:type=\"LoudnessFactor\">\n");
    fprintf(f, "\t\t<Scalar>  %f</Scalar>\n", mel.loudnessFactor);
    fprintf(f, "</AudioDescriptor>\n\n");
}

//write sqlFeatures to XML MPEG-7 file
int exportXML(sqlStruct sqlFeatures, string filepath, string soundfile)
{
    int i;
    size_t pos = filepath.find(".xml");
    if (pos != string::npos)
    {
        pos = filepath.find("/");
        if (pos == string::npos)
        {
            string curDir = getCwd<string>() + "/" + filepath;
            filepath = curDir;
        }
    }
    else
    {
        soundfile = getFileName<string>(soundfile) + ".xml";
        char end = filepath[filepath.length() - 1];
        if (end != '/')
            filepath += "/";
        filepath += soundfile;
    }
    
    cout << "Output file: " << filepath << endl;
    FILE *f = fopen(filepath.c_str(), "w");
    if (f == NULL)
        throw "ExportXML::Error while opening XML output directory";
    
    writeXMLHeader(f);
    //Export symbolic infos
    fprintf(f, "<Name> %s</Name>\n", sqlFeatures.name.c_str());
    fprintf(f, "<Instrument> %s</Instrument>\n", sqlFeatures.instrument.c_str());
    fprintf(f, "<PlayingStyle> %s</PlayingStyle>\n", sqlFeatures.playingStyle.c_str());
    fprintf(f, "<Dynamics> %s</Dynamics>\n", sqlFeatures.dynamics.c_str());
    fprintf(f, "<StringMute> %s</StringMute>\n", sqlFeatures.stringMute.c_str());
    fprintf(f, "<BrassMute> %s</BrassMute>\n", sqlFeatures.brassMute.c_str());
    fprintf(f, "<Note> %s</Note>\n", sqlFeatures.note.c_str());
    fprintf(f, "<Duration> %f</Duration>\n\n", sqlFeatures.duration);
    
    for (i = 0; i < sqlFeatures.descriptors.size(); i++)
        writeDescriptortoXML(sqlFeatures.descriptors[i], f);
    writePartialstoXML(sqlFeatures.partials, f);
    writeMelBandstoXML(sqlFeatures.melbands, f);
    fprintf(f, "</Audio>\n</MultimediaContent>\n</Description>\n</Mpeg7>");
    
    fclose(f);
    return 0;
}

//write sqlFeatures to XML MPEG-7 file
int exportXML(sqlStruct sqlFeatures, string soundfile)
{
    int i;
    soundfile = getFileName<string>(soundfile);
    string filepath = getCwd<string>() + "/" + soundfile + ".xml";
    cout << "Output file: " << filepath << endl;
    FILE *f = fopen(filepath.c_str(), "w");
    if (f == NULL)
        throw "ExportXML::Error while opening XML output directory";
    
    writeXMLHeader(f);
    //Export symbolic infos
    fprintf(f, "<Name> %s</Name>\n", sqlFeatures.name.c_str());
    fprintf(f, "<File> %s</File>\n", sqlFeatures.file.c_str());
    fprintf(f, "<Source> %s</Source>\n", sqlFeatures.source.c_str());
    fprintf(f, "<Instrument> %s</Instrument>\n", sqlFeatures.instrument.c_str());
    fprintf(f, "<Family> %s</Family>\n", sqlFeatures.family.c_str());
    fprintf(f, "<PlayingStyle> %s</PlayingStyle>\n", sqlFeatures.playingStyle.c_str());
    fprintf(f, "<Dynamics> %s</Dynamics>\n", sqlFeatures.dynamics.c_str());
    fprintf(f, "<StringMute> %s</StringMute>\n", sqlFeatures.stringMute.c_str());
    fprintf(f, "<BrassMute> %s</BrassMute>\n", sqlFeatures.brassMute.c_str());
    fprintf(f, "<Note> %s</Note>\n", sqlFeatures.note.c_str());
    fprintf(f, "<Duration> %f</Duration>\n\n", sqlFeatures.duration);
    
    for (i = 0; i < sqlFeatures.descriptors.size(); i++)
        writeDescriptortoXML(sqlFeatures.descriptors[i], f);
    writePartialstoXML(sqlFeatures.partials, f);
    writeMelBandstoXML(sqlFeatures.melbands, f);
    fprintf(f, "</Audio>\n</MultimediaContent>\n</Description>\n</Mpeg7>");
    
    fclose(f);
    return 0;
}
