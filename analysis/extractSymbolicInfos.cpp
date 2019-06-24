//
//  extractSymbolicInfos.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/12/13.
//  Copyright (c) 2013 Antoine Bouchereau. All rights reserved.
//

#include "extractSymbolicInfos.h"
#include <boost/algorithm/string.hpp>

void extractSymbolicInfos(string soundfile, sqlStruct &sqlFeatures, string source)
{
    string s = getFileName<string>(soundfile);
    sqlFeatures.name = s;
    sqlFeatures.note = "";
    sqlFeatures.instrument = "";
    string altNote = "";
    string mute;
    
    boost::regex expression ("(?<note>[A-G]#?[0-9]\\+?)");
    boost::regex altExpression ("(?<note>[A-G]#?[0-9]\\+?)");
    std::string::const_iterator start, end, sDyn, eDyn;
    start = s.begin();
    end = s.end();
    boost::match_results<std::string::const_iterator> what;
    while(regex_search(start, end, what, expression))
    {
        sqlFeatures.note += what[0] + "_";
        altNote += what[0] + "_";
        // update search position:
        start = what[0].second;
    }
    if (sqlFeatures.note.length() != 0)
    {
        sqlFeatures.note.erase(sqlFeatures.note.end() - 1);
        altNote.erase(altNote.end() - 1);
        replaceAll<string>(sqlFeatures.note, "+", "\\+");
        expression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?(?<sourdine>[a-z_A-Z]*)-(?<modeDeJeu>.*)-(?<note>"+ sqlFeatures.note +")-(?<dynamique>[m f p N =]+)-?";
        altExpression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?(?<sourdine>[a-z_A-Z]*)-(?<modeDeJeu>.*)-(?<note>"+ altNote +")-(?<dynamique>[m f p N =]+)-?";
    }
    else
    {
        expression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?(?<sourdine>[a-z_A-Z]*)-(?<modeDeJeu>.*)-(?<note>.*)-(?<dynamique>[m f p N =]+)-?";
        altExpression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?(?<sourdine>[a-z_A-Z]*)-(?<modeDeJeu>.*)-?";
    }
    
    start = s.begin();
    end = s.end();
    while(regex_search(start, end, what, expression))
    {
        sqlFeatures.instrument = what[1];
        mute = what[2];
        sqlFeatures.playingStyle = what[3];
        sqlFeatures.note = what[4];
        sqlFeatures.dynamics = what[5];
        std::vector<std::string> strs;
        boost::split(strs, sqlFeatures.dynamics, boost::is_any_of("="));
        sqlFeatures.dynamics = strs[0];
        sqlFeatures.intentDynamic = strs[0];
        if (strs.size() > 1)
            sqlFeatures.intentDynamic = strs[1];
        // update search position:
        start = what[0].second;
    }
    //printf("\n*** 1 ***\n%s\n%s\n%s\n%s\n*** 1 ***\n\n",sqlFeatures.instrument.c_str(),sqlFeatures.playingStyle.c_str(),sqlFeatures.note.c_str(),sqlFeatures.dynamics.c_str());
    if (sqlFeatures.instrument.length() == 0 && sqlFeatures.note.length() != 0)
    {
        start = s.begin();
        end = s.end();
        while(regex_search(start, end, what, altExpression))
        {
            sqlFeatures.instrument = what[1];
            mute = what[2];
            sqlFeatures.playingStyle = what[3];
            sqlFeatures.dynamics = what[5];
            // update search position:
            start = what[0].second;
        }
    }
    if (sqlFeatures.instrument.length() == 0 && sqlFeatures.note.length() != 0)
    {
        expression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?(?<sourdine>[a-z_A-Z]*)-(?<modeDeJeu>.*)-(?<note>"+ sqlFeatures.note +")";
        start = s.begin();
        end = s.end();
        while(regex_search(start, end, what, expression))
        {
            sqlFeatures.instrument = what[1];
            mute = what[2];
            sqlFeatures.playingStyle = what[3];
            
            // update search position:
            start = what[0].second;
        }
    }
    
    //printf("\n*** 3 ***\n%s\n%s\n%s\n%s\n*** 3 ***\n\n",sqlFeatures.instrument.c_str(),sqlFeatures.playingStyle.c_str(),sqlFeatures.note.c_str(),sqlFeatures.dynamics.c_str());
    if (sqlFeatures.instrument.length() == 0)
    {
        expression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?-(?<modeDeJeu>.*)-(?<dynamique>[m f p]+)?(-[1-5]c)?\\x2E";
        start = s.begin();
        end = s.end();
        while(regex_search(start, end, what, expression))
        {
            sqlFeatures.instrument = what[1];
            mute = "";
            sqlFeatures.playingStyle = what[2];
            sqlFeatures.dynamics = what[3];
            break;
        }
    }
    
    if (sqlFeatures.instrument.length() == 0)
    {
        expression = "(?<instrument>^[a-z_A-Z_0-9]+)\\+?-(?<modeDeJeu>.*)(-[1-5]c?)?\\x2E";
        start = s.begin();
        end = s.end();
        while(regex_search(start, end, what, expression))
        {
            sqlFeatures.instrument = what[1];
            mute = "";
            sqlFeatures.playingStyle = what[2];
            sqlFeatures.dynamics = "mp";
            break;
        }
    }
        
    //Final parsing of symbolic informations
    //Check file/family/source
    //If instrument is empty, then sound is no SOL
    if (sqlFeatures.instrument.size() == 0 || source != "SOL")
    {
        sqlFeatures.file        = soundfile;
        sqlFeatures.family      = "NA";
        sqlFeatures.source      = source;
        // If sound is in User library, instrument is "Others"
        sqlFeatures.instrument  = "Others";
    }
    //Sound is SOL
    else
    {
        vector<size_t> positions;
        size_t pos = soundfile.find("/", 0);
        while(pos != string::npos)
        {
            positions.push_back(pos);
            pos = soundfile.find("/", pos + 1);
        }
        if (positions.size() > 3)
            sqlFeatures.file = soundfile.substr(positions[positions.size() - 4] + 1, soundfile.size());
        else
            sqlFeatures.file = soundfile;
        pos = sqlFeatures.file.find("/");
        sqlFeatures.family = sqlFeatures.file.substr(0, pos);
        sqlFeatures.source = source;
    }
    
    //Check dynamics
    if (sqlFeatures.dynamics.length() == 0)
        sqlFeatures.dynamics = "mp";
    
    //Check instrument
    if (sqlFeatures.instrument.length() == 0)
        sqlFeatures.instrument = "NA";
    
    //Check playingStyle
    if (sqlFeatures.playingStyle == "NA")
        sqlFeatures.playingStyle = "ord";
    
    //Check stringMute/brassMute
    if (mute == "") mute = "N";
    vector<string> family;
    family.push_back("Trumpets");
    family.push_back("Trombones");
    family.push_back("Tubas");
    family.push_back("Horns");
    if (std::find(family.begin(), family.end(), sqlFeatures.family) != family.end())
        sqlFeatures.brassMute = mute;
    if (sqlFeatures.family == "Strings")
        sqlFeatures.stringMute = mute;
    
    //Check note
    if (sqlFeatures.note.length() == 0)
        sqlFeatures.note = "NA";
    else
    {
        replaceAll<string>(sqlFeatures.note, "-", ",");
        replaceAll<string>(sqlFeatures.note, "\\+", "");
    }
}