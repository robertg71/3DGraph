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

#ifndef READCSV_H_
#define READCSV_H_

#pragma once
#include "MAHeaders.h"
#include <MAUtil/Moblet.h>
#include <hash_map>
#include <vector>
#include <string>


class IReadCSV
{
public:
	virtual bool load(MAHandle resource,char delim=',',bool trim=true) = 0;
};

//typedef std::pair<std::string, std::vector<std::string>> CSVElm;
/**
 * \brief Class ReadCSV
 *
 * \description: Read comma separated values (String), simple form of database container
 * Vector array with vector... or 2 dimensional arrays e.g. array[x] => vector of string => array[x][y] => string
 */

class ReadCSV : public IReadCSV
{
protected:
	std::vector<std::vector<std::string> >  m_db;

public:
	virtual ~ReadCSV() {}
	virtual bool load(MAHandle resource,char delim=',',bool trim=true);
	std::vector<std::vector<std::string> >& getDB() {return m_db;}
};

/**
 * \brief Class: ReadCSV_hash
 *
 * \Description: Read Comma Separated Values (String), hash based formof data base (dictionary)
 * where the lookup key is a string table[ "test" ] => vector of strings
 *
 */
typedef std::pair<std::string, std::vector<std::string> > CSV_pair;
typedef std::hash_map<std::string,std::vector<std::string> > CSVhashTable;
class ReadCSV_hash : public ReadCSV
{
protected:
	std::hash_map<std::string,std::vector<std::string> >  m_table;

public:
	virtual ~ReadCSV_hash() {}
	virtual bool load(MAHandle resource,char delim=',',bool trim=true);
	CSVhashTable& getTable() {return m_table;}
};



#endif /* READCSV_H_ */
