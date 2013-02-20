/*
 * ReadCSV.h
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
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
/*
 * Class ReadCSV
 *
 * Description: Read comma separated values (String), simple form of database container
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

/*
 * Class: ReadCSV_hash
 *
 * Description: Read Comma Separated Values (String), hash based formof data base (dictionary)
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
