/*
 * Utils.h
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
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
