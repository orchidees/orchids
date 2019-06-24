//
//  ircamdescriptor.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 18/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "ircamdescriptor.h"


void ircamConfiguration(
                        int writeConfig,
                        vector<ircamDescriptor_param> params)
{
    int i;
    if (writeConfig == 1)
    {
        FILE *fid = fopen("ircamDescriptor.cfg", "w");
        fprintf(fid, "\n[Parameters]\n\n");
        
        for (i=0; i<nbParameters; i++)
            if (params[i].name.compare("WindowType") == 0)
                fprintf(fid, "%s = %s\n", params[i].name.c_str(), params[i].sVal.c_str());
            else
                fprintf(fid, "%s = %f\n", params[i].name.c_str(), params[i].fVal);
        
        fprintf(fid, "\n[StandardDescriptors]\n\n");
        for (i=nbParameters; i<nbParameters+nbStandard; i++)
            fprintf(fid, "%s = %f\n", params[i].name.c_str(), params[i].fVal);
        fprintf(fid, "\n");
        
        FILE *fidDesc = fopen("configurationFull.desc", "r");
        char line[1024];
        
        while ( fgets(line, sizeof(line), fidDesc) )
            fputs(line, fid);

        fclose(fidDesc);
        
        fprintf(fid, "\n[EnergyDescriptors]\n\n");
        for (i=nbParameters+nbStandard ; i<nbParameters+nbStandard+nbEnergy ; i++)
            fprintf(fid, "%s = %f\n", params[i].name.c_str(), params[i].fVal);
        
        fclose(fid);
    }
};


vector< feature<float> > ircamDescriptors(
                                          const char                        *soundfile,
                                          vector<ircamDescriptor_param>     params)
{
    vector< feature<float> >    features;
    string                      iDescDir    = "./ircamdescriptor";
    int                         j, k, l;
    cout << "* Preparing configuration file" << endl;
    ircamConfiguration(1, params);

    cout << "* Processing " << soundfile << endl;
    string iDescCmd = iDescDir + " " + soundfile + " ircamDescriptor.cfg";        
    if (system(iDescCmd.c_str()) > 0)
        throw "Analysis::Error while executing ircamDescriptor";
        
    string outFile = (string)soundfile + ".descr.sdif";
    SdifFileT *file = open_file(outFile);
    cout << "* Reading SDIF descriptors headers: " << outFile << endl;
        
    //Retrieve descriptors names
    string         fsigstr  = "1DSC";
    SdifSignature  fsig     = SdifStringToSignature(fsigstr.c_str());
    SdifFrameTypeT *ftype   = SdifTestFrameType(file, fsig);
    int            fnumcomp = SdifFrameTypeGetNbComponents(ftype);
    SdifComponentT *fcomp;
    
    // Get list of descriptors we need
    vector<string> descriptors = getStaticDescriptors();
    for (j = 1; j <= fnumcomp - 26; j++) // Don't check the 26 last frames = Info frames
    {
        fcomp = SdifFrameTypeGetNthComponent(ftype, j);
        string fName = SdifFrameTypeGetComponentName(fcomp);
        if (std::find(descriptors.begin(), descriptors.end(), fName) != descriptors.end())
        {
            feature<float> f(SdifFrameTypeGetComponentName(fcomp), SdifSignatureToString(fcomp->MtrxS));
            features.push_back(f);
        }
    }
    SdifFClose(file);
        
    //Retrieve datas and times in features
    unsigned long nread;
    nread = SdifReadFile(outFile.c_str(), NULL, MySdifQueryFrameType, NULL, MySdifQueryMatrixData, NULL, &features);

    //assert(nread > 0);
        
    //Cleanup
    SdifGenKill();
    
    //Modifying descriptors structure
    cout << "* Modifying descriptors structure" << endl;
    for (j = 0; j < features.size(); j++)
    {
        switch (features[j].values->cols())
        {
            case 9:
                features[j].values->resizeMatrixByCol(7, 9);
                break;
                    
            case 6:
                features[j].values->resizeMatrixByCol(2, 2);
                break;
                    
            case 3:
                features[j].values->resizeMatrixByCol(3, 3);
                break;
                    
            default:
                break;
        }
    }
    
    /*
    for (j = 0; j < features.size(); j++)
        if (features[j].name.compare("EnergyEnvelope") == 0)
            if (features[j].values->size() == 0)
                for (k = 0; k < features.size(); k++)
                    if (features[k].name.compare("TotalEnergy") == 0)
                    {
                        features[j].values = features[k].values.get();
                        break;
                    }
    */
    
    //boost::filesystem::remove(outFile.c_str());
    
    return features;
};
