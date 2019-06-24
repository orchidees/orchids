//
//  query.cpp
//  Orchids
//
//  Created by Antoine Bouchereau on 23/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#include "query.h"


void Query::print()
{
    if (m_type == "IS")
        if (m_descriptor == "MUTED" || m_descriptor == "UNMUTED")
            printf("Query:\t %s\t%s\n", m_type.c_str(), m_descriptor.c_str());
        else
            printf("Query:\t %s\t%s\t%s\n", m_descriptor.c_str(), m_type.c_str(), boost::any_cast<string>(m_value).c_str());
    else if (m_type == "REGEXP")
        printf("Query:\t %s\t%s\t%s\n", m_descriptor.c_str(), m_type.c_str(), boost::any_cast<string>(m_value).c_str());
    else if (m_type == "UNDER" || m_type == "OVER")
        printf("Query:\t %s\t%s\t%f\n", m_descriptor.c_str(), m_type.c_str(), boost::any_cast<float>(m_value));
    else if (m_type == "BETWEEN")
        printf("Query:\t %s\t%s\t%f\t%f\n", m_descriptor.c_str(), m_type.c_str(), boost::any_cast<pair<float, float> >(m_value).first, boost::any_cast<pair<float, float> >(m_value).second);
    else if (m_type == "FOLLOWS")
        printf("Query:\t %s\t%s\t{128x1} float\n", m_descriptor.c_str(), m_type.c_str());
}