/**
 *
 *  @file			analysis.cpp
 *  @brief			Launch Analysis Module
 *  @author			Antoine Bouchereau
 *  @version        0.04
 *	@date			16-12-2013
 *
 */

#include "analysis.h"

int launchAnalysisModule(int argc, const char * argv[], sqlStruct& sqlFeatures)
{    
    //Analyse Sound
    //sqlStruct       sqlFeatures;
    analysis_params parameters      = defaultAnalysisParams();
    bool            success         = false;
    bool            outputDefined   = false;
    bool            optionDefined   = false;
    string          output, configFile;
    
    if (argc < 2)
    {
        printHelp();
        return 1;
    }
    
    if (strcmp(argv[1], "-help") == 0)
    {
        printHelp();
        return 0;
    }
    
    string soundfile = argv[1];
    ++argv;
    --argc;
    
    while ((argc > 1) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
			case 't':
            {
				optionDefined = true;
                //Launch target analysis
                try {
                    success = analyzeTarget(soundfile, parameters, sqlFeatures);
                } catch (const char * e) {
                    cerr << e << endl;
                    return 1;
                }
				break;
            }
                
			case 's':
            {
				optionDefined = true;
                //Launch sound analysis
                try {
                    success = analyzeSound(soundfile, parameters, sqlFeatures, "SOL");
                } catch (const char * e) {
                    cerr << e << endl;
                    return 1;
                }
				break;
            }
                
            case 'o':
            {
				outputDefined = true;
                output = argv[1];
                output.erase(0, 3);
				break;
            }
                
            case 'c':
            {
                configFile = argv[1];
                configFile.erase(0, 3);
				getConfigAnalysisParams(parameters, configFile);
                int err = checkConfigParams(parameters);
                if (err != -1)
                {
                    printf("Value error in configuration file. Parameters %s is out of range\n", parameters.irc_desc_params[err].name.c_str());
                    return 1;
                }
                break;
            }
                
			default:
            {
				printf("Wrong Argument: %s\n", argv[1]);
				printHelp();
                return 1;
            }
		}
        
		++argv;
		--argc;
	}
    
    if (!optionDefined)
    {
        //Launch target analysis
        try {
            success = analyzeTarget(soundfile, parameters, sqlFeatures);
        } catch (const char * e) {
            cerr << e << endl;
            return 1;
        }
    }
    
    if (outputDefined)
    {
        try {
            exportXML(sqlFeatures, output, soundfile);
        } catch (const char * e) {
            cerr << e << endl;
            return 1;
        }
    }else{
        try {
            exportXML(sqlFeatures, soundfile);
        } catch (const char * e) {
            cerr << e << endl;
            return 1;
        }
    }

    //Analysis did fine, clear memory
    if (success)
        sqlFeatures.deallocate();

    return 0;
}