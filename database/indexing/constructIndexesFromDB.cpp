//
//  constructIndexesFromDB.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 07/08/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "constructIndexesFromDB.h"


indexTreePtr constructIndexesFromDB(
                                    KnowledgeBDBPtr  knowledge,
                                    string           descName)
{
    int i, j, k;
    indexTreePtr index(new indexTree(8, 8, 100, 8));
    int nbSounds = knowledge->getNbEntries();
    
    boost::filesystem::path path = getResourcesFolder<string>();
    path += "Indexes";
    string filename = path.string() + "/" + descName + ".txt";
    if (boost::filesystem::exists(filename))
    {
        // Deserialization
        cout << "Computing indexes on " << descName << "..." << endl;
        std::ifstream ifs(filename.c_str());
        boost::archive::text_iarchive ia(ifs);
        boost::serialization::load(ia, index, 1);
        ifs.close();
        
        // If time series index on this feature is not up to date
        if ((int)getMax(&index->getRoot()->getIdList()[0], (int)index->getRoot()->getIdList().size()) != nbSounds)
        {
            // ...then remove the file and compute new index
            boost::filesystem::remove(filename.c_str());
            // Reset the index
            index = indexTreePtr(new indexTree(8, 8, 100, 8));
        }
        else
        {
            // ...else return the deserialized index
            return index;
        }
    }
    
    int maxThous = floor(nbSounds / 1000);
    for (i = 0; i <= maxThous; i++)
    {
        vector<int> listIDs;
        int val = i * 1000 + 1;
        while (val <= (i + 1) * 1000 && val <= nbSounds)
            listIDs.push_back(val++);
        vector<string> features;
        features.push_back("duration");
        features.push_back(descName);
        vector<int> fId = isMember(features, knowledge->getFieldsList().fields);
        map<int, vector<matrixPtr> > values;
        knowledge->getBDBConnector()->getMultipleFinalOptimized(listIDs, features, fId, values, false);
        printf("Creating index for %s %i / %i\n", descName.c_str(), i, maxThous);
        for (j = 0; j < listIDs.size(); j++)
        {
            // If ID was found
            //if (values[listIDs[j]].size() != 0)
            if (values.find(listIDs[j]) != values.end())
            {
                matrix dValue = (*values[listIDs[j]][1]);
                float min = getMin(dValue.c_data(), dValue.size());
                for (k = 0; k < dValue.size(); k++)
                    if (isinf(dValue(k)) || isnan(dValue(k)))
                        dValue(k) = min;
                if (dValue(0) < ((dValue(1) / 2) + 1))
                    dValue(0) = dValue(1);
                vector<float> rawData(dValue.begin(), dValue.end());
                index->insertSeries(rawData, listIDs[j], values[listIDs[j]][0]->operator()(0));
            }
        }
    }
    
    // Serialization
    std::ofstream ofs(filename.c_str());
    boost::archive::text_oarchive oa(ofs);
    boost::serialization::save(oa, index, 1);
    ofs.close();
    
    return index;
}