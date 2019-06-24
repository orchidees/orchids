/**
 *
 *  @file			string_utils.hpp
 *  @brief          Utility functions for strings
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__string_utils__
#define __Orchids__string_utils__

#include "Orchids.h"

/**
 *  @brief Convert a variable to a string
 *  @param Value Variable to convert
 */
template<typename T>
std::string to_string( const T & Value )
{
    // utiliser un flux de sortie pour créer la chaîne
    std::ostringstream oss;
    // écrire la valeur dans le flux
    oss << Value;
    // renvoyer une string
    return oss.str();
}

/**
 *  @brief Replace a char by another char
 *  @param str String to analyze
 *  @param o_s Character to replace
 *  @param n_s Replace by the new character
 */
template<class T>
const char* strrep(
                   const char *str,
                   const char *o_s,
                   const char *n_s)
{
    char *newstr    = NULL;
    char *c         = NULL;
    
    // No substring found
    if ((c = strstr(str, o_s)) == NULL) {
        return str;
    }
    
    if ((newstr = (char *) malloc((int) sizeof(str) -
                                  (int) sizeof(o_s) +
                                  (int) sizeof(n_s) + 1)) == NULL) {
        printf("ERROR: unable to allocate memory\n");
        return NULL;
    }
    
    strncpy(newstr, str, c-str);
    sprintf(newstr+(c-str), "%s%s", n_s, c+strlen(o_s));
    
    return newstr;
}

/**
 *  @brief Replace all occurences of a string in a string by another string
 *  @param str String to analyze
 *  @param from Substring to replace
 *  @param to Replace by the new substring
 */
template<class T>
void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

/**
 *  @brief Take a full path and return only the file name
 *  @param s Path to analyze
 */
template<class T>
inline string getFileName(string& s)
{
    return boost::filesystem::path(s).filename().string();
}

/**
 *  @brief Return the current working directory
 */
template<class T>
inline string getCwd()
{
    return boost::filesystem::path(boost::filesystem::current_path()).string();
}

/**
 *  @brief Return full path to Resources folder
 */
template<class T>
boost::filesystem::path getResourcesFolder()
{
    boost::filesystem::path path = boost::filesystem::path(getCwd<string>());
    path.remove_filename();
    path += "/Resources/";
    return path;
}

#endif