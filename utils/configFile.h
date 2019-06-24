/**
 *
 *  @file			configFile.h
 *  @brief          Configuration file parser
 *  @version        0.04
 *  @author			Antoine Bouchereau
 *	@date			16-12-2013
 *
 */

#ifndef __Orchids__configFile__
#define __Orchids__configFile__

#include "Orchids.h"

/**
 *  @brief Print error and exit program
 */
void exitWithError(const std::string &error);


/**
 *  @class Convert
 */
class Convert
{
public:

	template <typename T>
	static std::string T_to_string(T const &val)
	{
		std::ostringstream ostr;
		ostr << val;
        
		return ostr.str();
	}
    
	template <typename T>
	static T string_to_T(std::string const &val)
	{
		std::istringstream istr(val);
		T returnVal;
		if (!(istr >> returnVal))
			exitWithError("CFG: Not a valid " + (std::string)typeid(T).name() + " received!\n");
        
		return returnVal;
	}
    
	static std::string string_to_T(std::string const &val)
    {
        return val;
    }
};

/**
 *  @class ConfigFile
 */
class ConfigFile
{
private:
	std::map<std::string, std::string> contents;
	std::string fName;
    
	void removeComment(std::string &line) const;
	bool onlyWhitespace(const std::string &line) const;
	bool validLine(const std::string &line) const;
	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const;
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const;
	void extractContents(const std::string &line);
	void parseLine(const std::string &line, size_t const lineNo);
    
	void ExtractKeys();
    
public:
	ConfigFile(const std::string &fName);
    
	bool keyExists(const std::string &key) const;
    
	template <typename ValueType>
	ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
	{
		if (!keyExists(key))
			return defaultValue;
        
		return Convert::string_to_T<ValueType>(contents.find(key)->second);
	}
};


#endif
