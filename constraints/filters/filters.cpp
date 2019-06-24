//
//  filters.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 20/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "filters.h"


Filters::Filters(Session* sessObj, string name, string attribute)
{
    sSession = sessObj;
    fAttribute = attribute;
    fMode = "bypass";
    fName = name;
}

string Filters::getAttribute()
{
    return fAttribute;
}

string Filters::getMode()
{
    return fMode;
}

string Filters::getName()
{
    return fName;
}

void Filters::setMode(string mode)
{
    fMode = mode;
}

void Filters::initialize()
{
    
}

bool Filters::checkFeasibility()
{
    return true;
}


FiltersSymbolic::FiltersSymbolic(Session* sessObj, string name, string attribute)
: Filters(sessObj, name, attribute) {}

void FiltersSymbolic::setValuesList(vector<string> vals)
{
    valueList = vals;
    includeList = vals;
}

void FiltersSymbolic::setIncludeList(vector<string> vals)
{
    includeList = vals;
}
    
void FiltersSymbolic::addIncludeList(vector<string> vals)
{
    if (includeList.size() == 0)
        includeList = vals;
    else
        includeList.insert(includeList.end(), vals.begin(), vals.end());
}

vector<string> FiltersSymbolic::apply()
{
    if (fMode == "free")
        return valueList;
    else if (fMode == "force")
        return includeList;
    else
        return valueList;
}


FiltersTemporal::FiltersTemporal(Session* sessObj, string name, string attribute)
: Filters(sessObj, name, attribute) {}


FiltersSpectral::FiltersSpectral(Session* sessObj, string name, string attribute)
: Filters(sessObj, name, attribute) {}

void FiltersSpectral::setValuesRange(pair<float, float> range)
{
    valuesRange = range;
    filterRange = range;
}

void FiltersSpectral::setFilterRange(pair<float, float> range)
{
    filterRange = range;
}

pair<float, float> FiltersSpectral::apply()
{
    /*
     if (fMode == "free")
        return valuesRange;
    else
     */
    if (fMode == "BETWEEN")
        return filterRange;
    else if (fMode == "UNDER")
        return filterRange;
    else if (fMode == "OVER")
        return filterRange;
    /*
     else if (fMode == "approx")
        return filterRange;
     */
    else
        return pair<float, float>(0, 0);
}