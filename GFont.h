/*
 * GFont.h
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
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

typedef std::pair <std::string, ParseBMFont> String_ParseBMFont_Pair;
typedef std::pair <int,BMChar> Int_BMChar_Pair;
typedef std::pair <int, std::vector<BMKerning> > Int_Vector_BMKerning_Pair;

struct TextureStore
{
	GLuint m_texture;
};

// Struct name: BMFont
//---------------------------------------------------------------------------------
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

// Struct name: BMCommon
//---------------------------------------------------------------------------------
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

// Struct name: BMChar
//---------------------------------------------------------------------------------
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

// Struct name: BMKerning
//---------------------------------------------------------------------------------
struct BMKerning
{
	int m_first;
	int m_second;
	int m_amount;

	void parse(std::vector<std::string> &line);
};

// Struct name: BMPage
//---------------------------------------------------------------------------------
struct BMPage
{
	int			m_id;
	std::string m_file;

	void parse(std::vector<std::string> &line);
};

struct FontType
{
	float left, right;
	int size;
};

struct VertexType
{
	glm::vec2 position;
    glm::vec2 texture;
};

// Class name: BMFont (main class)
//---------------------------------------------------------------------------------
class BMFont : public IFont
{
private:

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

private:

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
