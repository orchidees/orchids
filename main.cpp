/**
 *
 *  @file			main.cpp
 *  @brief          Main
 *  @version        0.9.2
 *  @author			Philippe Esling
 *	@date			16-12-2013
 *
 */


#include "Orchids.h"
#include <omp.h>
#include <unistd.h>

boost::mt19937 rng;

int main(int argc, const char * argv[])
{
    /*
    const clock_t begin_time = clock();
    
    Session s;
    s.constructDefaultSession();
    */
    //mainScript();
    
    // Set OpenMP threads
    omp_set_num_threads(MIN(omp_get_num_procs(), 16));//omp_get_num_procs());
    // Initialize a global randomizer
    rng.seed(static_cast<unsigned int>(std::time(0)));
    if (argc == 2)
    {
        Session s;
        KnowledgeBDBPtr sKnowledge = KnowledgeBDBPtr(new KnowledgeBDB(&s, BDB_DIR, "OrchidsDb.db"));
        sKnowledge->getBDBConnector()->addSoundDirectory(argv[1], "SOL", "");
        return 0;
        /*
        Dbt key;
        init_dbt(&key, "play+sing-A4+C4", 16 * sizeof(char));
        sKnowledge->getBDBConnector()->del("playingStyle", &key);
        sKnowledge->getBDBConnector()->commit();
         */
    }
    if (argc == 3)
    {
        debugDatFuckinDatabase(argv[1], argv[2]);
//        debugDiversity();
        return 0;
    }
    if (argc < 2)
    {
        printf("Orchids - Server version :)\n-----------------\n");
        printf("Fetching database in : %s\n", BDB_DIR);
        Session s;
        s.setOSCStart();
        s.constructDefaultSession();
        s.setOSCJoin();
    }
    else
    {
        printf("Orchids - Command-line version :)\n-----------------\n");
        printf(" ./Search <targetSoundFile> -i=<orchestraList> -c=<criteriaList> -a=<algorithm> -s=<0,1> -h=<0,1> -f=<5bitsFlags>\n\n");
        printf(" <targetSoundFile>\t: Sound file to orchestrate (wave, aiff, mp3, au)\n");
        printf(" <orchestraList>\t: Set of instruments separated by a comma (ie. Vn,Vn,TpC,Bn)\n");
        printf(" <criteriaList>\t: Set of criteria separated by a comma (ie. SpectralCentroidMean,PartialsAmplitude)\n");
        printf(" <algorithm>\t: Algorithm to use, choose either Genetic or OptimalWarping\n");
        printf(" -s=<0,1>\t: Flag for static (1) or temporal (0) target\n");
        printf(" -h=<0,1>\t: Flag for using (1) or not (0) harmonic filtering\n");
        printf(" <5bitsFlag>\t: Bit flags (xxxxx) for activating different options of the algorithm \n");
        printf(" \t 1st bit : Activate selective mutation\n");
        printf(" \t 2nd bit : Activate global probabilities adaptation\n");
        printf(" \t 3rd bit : Activate local optimization every 5 iterations\n");
        printf(" \t 4th bit : Activate adaptive optimization every 5 iterations [in Optimal Warping only]\n");
        printf(" \t 5th bit : Activate time series indexing [in Optimal Warping only]\n");
        launchSearch(argc, argv);
    }
    //cout << "Process time: " << float( clock () - begin_time ) / CLOCKS_PER_SEC << endl;
    return 0;
}