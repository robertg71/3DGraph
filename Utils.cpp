/*
 * Utils.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
 */

#include "Utils.h"
#include <vector>
#include <string>


namespace Utils
{
	// StringCopy
	//-------------------------------------------------------------------------
	std::string stringCopy(std::string &in, size_t start, size_t end, bool trim)
	{
		std::string value;
		for(size_t i=start; i<end; ++i)
		{
			if (trim && in[i] == ' ')
				continue;
			value.push_back(in[i]);
		}
		return value;
	}

	// Spilt function, like from boost, perl, Java etc.
	//-------------------------------------------------------------------------
	std::vector<std::string> split(std::string &line,char delimiter,bool trim)
	{
		size_t len = line.size();
		std::vector<std::string> lineData;
		for(size_t start = 0; start<len;)
		{
			size_t end=start;
			for(; end<=len && (line[end] != delimiter); ++end);		// search for first space

			if (!(start == end && line[end] == delimiter))
				lineData.push_back(stringCopy(line,start,end,trim));	// only store something that starts with != delim otherwise skip entry

			start = end+1;	//update next entry
		}
		return lineData;
	}

	// Un Quote a string (remove its quotes)
	//-----------------------------------------------------------
	std::string unQuote(const std::string &str,const char quote)
	{
		std::string out;
		out.reserve(str.size());
		for(size_t i=0; i<str.size(); i++)
		{
			char c = str[i];
			if (c == quote)
				continue;
			out.push_back(c);
		}
		return out;
	}

}
