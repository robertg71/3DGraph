/*
 * GFont.h
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
 */

#ifndef GFONT_H_
#define GFONT_H_


// Filename: BMFont.h
//---------------------------------------------------------------------------------

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <hash_map>
#include <GLES2/gl2.h>


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
// Class name: BMFont
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

struct BMKerning
{
	int m_first;
	int m_second;
	int m_amount;

	void parse(std::vector<std::string> &line);
};

struct BMPage
{
/*	BMPage(const BMPage &p)
	{
		m_id 	= p.m_id;
		m_file 	= p.m_file;
	}
*/
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

class IFont
{
public:
	virtual ~IFont() {};

	virtual bool Init(MAHandle fontResource, std::vector<MAHandle> &textureResources) = 0;
	virtual void Clear() = 0;
	virtual GLuint GetTexture(int i=0) = 0;
	virtual float BuildVertexArray(glm::vec4 * vertexPtr, const char * str, float dX, float dY, float scaleX, float scaleY) = 0;
};

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
	bool LoadFontData(MAHandle resource);
	void ReleaseFontData();
	void ReleaseTexture();
	ParseBMFont getBMType(std::string &header);

private:

	std::hash_map<std::string, ParseBMFont> 	m_string2enum;	// parsing setup should be static! or singleton
	BMInfo										m_info;
	BMCommon									m_common;
	std::hash_map<int,BMChar>					m_chars;
	std::hash_map<int, std::vector<BMKerning> >	m_kernings;	// there might be need to have an inverted lookup table from second to first.
	std::vector<TextureStore>					m_texStores;
	std::vector<BMPage>							m_pages;
	int 										m_nchars;
	int											m_nkernings;
};





#endif /* GFONT_H_ */
