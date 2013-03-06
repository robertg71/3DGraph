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

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <string>

namespace Utils
{
	std::vector<std::string> split(std::string &line,char delimiter,bool trim);
	std::string stringCopy(std::string &in, size_t start, size_t end, bool trim);
	std::string unQuote(const std::string &str,const char quote= '\"');
}

#endif /* UTILS_H_ */
