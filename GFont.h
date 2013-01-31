/*
 * GFont.h
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
 */

#ifndef GFONT_H_
#define GFONT_H_


// Filename: fontclass.h
//---------------------------------------------------------------------------------

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <hash_map>
#include <GLES2/gl2.h>


class ParseBMFont;
struct BMChar;
struct BMKerning;

typedef std::pair <std::string, ParseBMFont> String_ParseBMFont_Pair;
typedef std::pair <int,BMChar> Int_BMChar_Pair;
typedef std::pair <int, std::vector<BMKerning> > Int_Vector_BMKerning_Pair;

struct TextureStore
{
	GLuint m_texture;
};
// Class name: FontClass
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
	int			m_id;
	std::string m_file;

	void parse(std::vector<std::string> &line);
};

struct FontType
{
	float left, right;
	int size;
};


class FontClass
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

	struct VertexType
	{
		glm::vec3 position;
	    glm::vec2 texture;
	};

public:

	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(MAHandle fontResource, std::vector<MAHandle> &textureResources);
//	bool Initialize(std::string& fontTable, std::string& textureFile);
	void Clear();

	GLuint GetTexture(int i=0);

	float BuildVertexArray(void*, const char*, float, float);

private:
	bool LoadFontData(MAHandle resource);
	void ReleaseFontData();
	void ReleaseTexture();
	ParseBMFont getBMType(std::string &header);

private:

	std::hash_map<std::string, ParseBMFont> 	m_string2enum;
	BMInfo										m_info;
	BMCommon									m_common;
	std::hash_map<int,BMChar>					m_chars;
	std::hash_map<int, std::vector<BMKerning> >	m_kernings;	// there might be need to have an inverted lookup table from second to first.
	std::vector<TextureStore>					m_texStores;
	std::vector<BMPage>							m_pages;
	int 										m_nchars;
	int											m_nkernings;
};

class RenderText
{
public:
	RenderText();
	~RenderText();

	bool Initiate(/*LPDIRECT3DDEVICE pDevice, LPDEVICECONTEXT pContext,*/ float screenWidth, float screenHeight,FontClass *font=0);
	void SetFont(FontClass *font);
	float DrawText(const char*str,float x,float y,glm::vec4 &rgba);
	void Release();

	void SetProjMatrix(glm::mat4 &proj)	{m_proj = proj;}
	void SetWorldMatrix(glm::mat4 &world) {m_world = world;}
	void SetViewMatrix(glm::mat4 &view)	{m_view = view;}

protected:
	bool createDefaultBuffers(int strSz);
	void releaseDefaultBuffer();

	FontClass		*m_font;
	float			m_width;
	float			m_height;
	glm::mat4		m_world;
	glm::mat4		m_view;
	glm::mat4		m_proj;
//	glm::mat4		*m_matrixParam[3];

/*
	LPDIRECT3DBLENDSTATE	m_alphaEnableBlendingState;
	LPDIRECT3DBLENDSTATE	m_alphaDisableBlendingState;
	LPDIRECT3DDEVICE		m_pDevice;
	LPDEVICECONTEXT			m_pContext;
	LPDIRECT3DSAMPLERSTATE  m_pSampleState;
	LPDIRECT3DVERTEXBUFFER	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER   m_pIndexBuffer;
	ShaderStore				*m_pShader;
	int						m_nIndex;
*/
};




#endif /* GFONT_H_ */
