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

#ifndef GFONT_H_
#define GFONT_H_


// Filename: BMFont.h
//
// Handles font format from the tool BMFont (windows)
//
// BMFont Class contain all info necessary for a Font.
// Where as each letters offset position texture coordinates spacing (kerning) etc towards other characters
// Input : Parses the .fnt (textfile) containing with and height of each letter and its kerning towards other characters etc.
// Input : Texture that contains the alphabet setup N
// Font can generate vertex data with texture coordinate.
// NOTE this class is independent from the Graph system and can be use generically.
// THIS CLASS DOESN'T RENDER THE TEXT, IT CONTAINS INFORMATION ABOUT THE FONT
// SEE RenderText class for the text rendering part, that uses BMFont as input.
//---------------------------------------------------------------------------------

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <hash_map>
#include <GLES2/gl2.h>
#include "IFont.h"

class ParseBMFont;
struct BMChar;
struct BMKerning;
struct VertexType;

/**
 * \brief helper typedef using pairs for hash_map<>
 */
typedef std::pair <std::string, ParseBMFont> String_ParseBMFont_Pair;
typedef std::pair <int,BMChar> Int_BMChar_Pair;
typedef std::pair <int, std::vector<BMKerning> > Int_Vector_BMKerning_Pair;

/**
 * \brief TextureStore
 * stores texture index for the shader to use.
 * TODO add more texture info in here in the future.
 */
struct TextureStore
{
	GLuint m_texture;
};

/**
* \brief Struct name: BMFont
*  used for parsing BMInfo (header part of .fnt file)
*/
struct BMInfo
{
	std::string m_face;
	int m_size;
	bool m_bold;
	bool m_italic;
	std::string m_charset; 
	int m_unicode; 
	int m_stretchH; 
	int m_smooth; 
	int m_aa; 
	int m_padding[4];
	int m_spacing[2]; 
	int m_outline;

	void parse(std::vector<std::string> &line);

};

/**
* \brief Struct name: BMCommon
*	used for parsing BMCommon information from .fnt file
*	contains information that is common for this font
*/
struct BMCommon
{

	int m_lineHeight;
	int m_base;
	int m_scaleW;
	int m_scaleH;
	int m_pages;
	int m_packed;
	int m_alphaChnl;
	int m_redChnl; 
	int m_greenChnl;
	int m_blueChnl;

	void parse(std::vector<std::string> &line);
};

/**
* Struct name: BMChar
* used for parsing the BMChar entry from .fnt file
* contains information for one character.
*/

struct BMChar
{
	int m_id;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_xoffset;
	int m_yoffset;
	int m_xadvance;
	int m_page;
	int m_chnl;

	void parse(std::vector<std::string> &line);
};

/**
* \brief Struct name: BMKerning
* used for parsing the BMKerning entry from .fnt file
* contains kerning information, spacing between one character to another character.
*/

struct BMKerning
{
	int m_first;
	int m_second;
	int m_amount;

	void parse(std::vector<std::string> &line);
};

/**
* \brief Struct name: BMPage
* used for parsing the BMPage entry from .fnt file
* contains texture index to be used, one font could contain several texture
*/

struct BMPage
{
	int			m_id;
	std::string m_file;

	void parse(std::vector<std::string> &line);
};

/**
 * \brief FontType
 */

struct FontType
{
	float left, right;
	int size;
};

/**
 * \brief VertexType
 * contains vertex info, position and texture coordinate
 */

struct VertexType
{
	glm::vec2 position;
    glm::vec2 texture;
};

/**
* \brief Class name: BMFont (main class)
*  main Font class that holds all information about the specific font.
*  Texture and each character info.
*  also provides with building a vertex table
*/

class BMFont : public IFont
{
private:

	// ParseBMFont enumeration for each different element .fnt file contains.
	enum ParseBMFont
	{
		PBM_IDLE = 0,
		PBM_INFO,
		PBM_COMMON,
		PBM_PAGE,
		PBM_CHARS,
		PBM_CHAR,
		PBM_KERNINGS,
		PBM_KERNING,
	};

public:

	BMFont();
	BMFont(const BMFont&);
	virtual ~BMFont();

	virtual bool Init(MAHandle fontResource, std::vector<MAHandle> &textureResources);
	virtual void Clear();
	virtual GLuint GetTexture(int i=0);
	virtual float BuildVertexArray(glm::vec4 * vertexPtr, const char * str, float dX, float dY, float scaleX, float scaleY);

private:
	// Helper functions (internal use)
	bool LoadFontData(MAHandle resource);
	void ReleaseFontData();
	void ReleaseTexture();
	ParseBMFont getBMType(std::string &header);

	std::hash_map<std::string, ParseBMFont> 	m_string2enum;	// Parsing setup should be static! or singleton
	BMInfo										m_info;			// Header info of current Font
	BMCommon									m_common;		// Common data for current Font
	std::hash_map<int,BMChar>					m_chars;		// Contains table for each letter info texture coord width height offset position etc.
	std::hash_map<int, std::vector<BMKerning> >	m_kernings;		// Contains table of letters kerning toward other letters (spacing)
	std::vector<TextureStore>					m_texStores;	// Contains all textures that this alphabet has (but renderer uses only one)
	std::vector<BMPage>							m_pages;		// Page info, not in use! parses data for future ref.
	int 										m_nchars;		// amount of characters in font
	int											m_nkernings;	// amount of kerning information in font
};

#endif /* GFONT_H_ */
