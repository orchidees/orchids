//
//  query.h
//  Orchids
//
//  Created by Antoine Bouchereau on 23/06/14.
//  Copyright (c) 2014 Antoine Bouchereau. All rights reserved.
//

#ifndef __Orchids__query__
#define __Orchids__query__

#include "Orchids.h"

/**
 *  @class Query
 *
 *  @brief Object to store a BerkeleyDB query
 */
class Query
{
protected:
    string          m_type;         ///< Type of query
    string          m_descriptor;   ///< Descriptor on which the query applies
    boost::any      m_value;        ///< Value(s) to query
    
public:
    /**
     *  @brief Constructor 1
     *  @param type Type of query
     *  @param descriptor Descriptor on which the query applies
     */
    Query(string type, string descriptor)
    {
        m_type          = type;
        m_descriptor    = descriptor;
    }
    
    /**
     *  @brief Constructor 2
     *  @param type Type of query
     *  @param descriptor Descriptor on which the query applies
     *  @param value Value(s) to query (can be float, string or pair of float)
     */
    Query(string type, string descriptor, boost::any value)
    {
        m_type          = type;
        m_descriptor    = descriptor;
        m_value         = value;
    }
    
    /**
     *  @brief Destructor
     */
    ~Query(){};
    
    /**
     *  @brief Set value
     *  @param val New query value
     */
    void setValue(boost::any val){ m_value = val; }
    /**
     *  @brief Get query value
     */
    boost::any getValue(){ return m_value; }
    /**
     *  @brief Get query type
     */
    string getType(){ return m_type; };
    /**
     *  @brief Get descriptor on which the query applies
     */
    string getDescriptor(){ return m_descriptor; };
    
    void print();
};


#endif /* defined(__Orchids__query__) */
