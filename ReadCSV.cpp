/*
 * ReadCSV.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
 */


#include "ReadCSV.h"
#include "Utils.h"
#include <mavsprintf.h>

char* GetString(MAHandle resource)
{
    // Get the length of the string data.
    int length = maGetDataSize(resource);

    // Allocate space for the string data plus the
    // null termination character.
    char* buffer = new char[length + 1];

    // Read data.
    maReadData(resource, buffer, 0, length);

    // Null terminate the string.
    buffer[length] = '\0';

    // Return the string.
    return buffer;
}

bool ReadCSV::load( MAHandle resource, char delim, bool trim)
{
	char *data = GetString(resource);	// will allocate buffer
	if (data == 0)
	{
	    lprintfln("Error ReadCSV::load failed to load resource\n");
		return false;
	}

	std::string line, tmp;
	std::vector<std::string> lineData;
	char *pch = strtok(data,"\r\n");
	for (int i=0;pch != NULL;i++)
	{
	    pch = strtok(NULL, "\r\n");
	    line = pch;
		lineData = Utils::split(line,delim,trim);
		if (lineData.size() > /*1*/0)
			m_db.push_back(lineData);
		else
		    lprintfln("WARNING ReadCSV::load missing lineData from line %i => %s\n",i,line.c_str());
	}

	delete [] data;						// need to free temp buffer.
	return true;
}

bool ReadCSV_hash::load(MAHandle resource, char delim, bool trim)
{
	ReadCSV::load(resource,delim,trim);
	for(size_t i=0; i<m_db.size(); i++)
		m_table.insert( CSV_pair(m_db[i][0], m_db[i]) );

	return true;
}

