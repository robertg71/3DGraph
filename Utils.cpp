/*
Copyright (C) 2011 MoSync AB

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
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
