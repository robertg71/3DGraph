/*
 * GFont.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: CodeArt
 */


//------------------------------------------------------------------------------------------
// Filename: fontclass.cpp
//
// Supports BMFont (BitMap Font Generator)
//
#include "gfont.h"
#include "Utils.h"
#include "ReadCSV.h"
#include <GLES2/gl2.h>
#include <string>
#include <vector>
#include <hash_map>
#include <glm/glm.hpp>
#include <mavsprintf.h>


//
// Create the texture used for rendering.
//----------------------------------------
GLuint CreateTexture(MAHandle resource)
{
	GLuint textureHandle;
	// Create an OpenGL 2D texture from the R_BOX resource.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	int res = maOpenGLTexImage2D(resource);
	if (res == (int)MA_GL_TEX_IMAGE_2D_INVALID_IMAGE)
		return (GLuint)MA_GL_TEX_IMAGE_2D_INVALID_IMAGE;

	// Set texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureHandle;
}


FontClass::FontClass()
{
	m_string2enum["info"]		= PBM_INFO;
	m_string2enum["common"]		= PBM_COMMON;
	m_string2enum["page"]		= PBM_PAGE;
	m_string2enum["chars"]		= PBM_CHARS;
	m_string2enum["char"]		= PBM_CHAR;
	m_string2enum["kernings"]	= PBM_KERNINGS;
	m_string2enum["kerning"]	= PBM_KERNING;
	m_string2enum["idle"]		= PBM_IDLE;
}

FontClass::FontClass(const FontClass& other) : 
	m_chars(other.m_chars), 
	m_kernings(other.m_kernings), 
	m_texStores(other.m_texStores), 
	m_pages(other.m_pages)
{
	if (this != &other)
		return;

	m_info		= other.m_info;
	m_common	= other.m_common;
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(MAHandle fontResource, std::vector<MAHandle> &textureResources)
{
	// Load in the text file containing the font data.
	bool result = LoadFontData(fontResource);	// with its font specific data. (see BM font for more format info) use first example for now

	lprintfln("Info: FontClass::Initialize => load Font Textures %i (%i)",m_common.m_pages,(int)textureResources.size());

	if(result == false)
	{
		lprintfln("Error: FontClass::Initialize => load failed = %i",result);
		return false;
	}

	if (m_common.m_pages > (int)textureResources.size())
	{
		lprintfln("Error: FontClass::Initialize => pages=%i > amount of texture resources:%i",m_common.m_pages, (int)textureResources.size());
		return false;
	}

	// Load the texture that has the font characters on it.
	for(int i=0; i<m_common.m_pages; i++)
	{
		GLuint texHandle = CreateTexture(textureResources[i]);

		if (texHandle < 0)
		{
			lprintfln("Error: FontClass::Initialize => Invalid 2D Image err=%i",texHandle);
			return false;
		}
		TextureStore t;
		t.m_texture = texHandle;
		m_texStores.push_back(t);
	}
	return true;
}

void FontClass::Clear()
{
	ReleaseTexture();
	ReleaseFontData();
	return;
}

// todo initiate by building a hashtable of the keys.
FontClass::ParseBMFont FontClass::getBMType(std::string &header)
{
	std::hash_map<std::string, ParseBMFont>::iterator it = m_string2enum.find(header);
	return (it!=m_string2enum.end())? it->second: PBM_IDLE;
}

int GetValue(std::string &keyValue)
{
	std::vector<std::string> key_value = Utils::split(keyValue,'=',true);
//	key		= key_value[0];
	return atoi((key_value[1]).c_str());
}

void GetKeyValueFrom(std::string &keyValue,std::string &key, std::string &value)
{
	std::vector<std::string> key_value = Utils::split(keyValue,'=',true);
	key		= key_value[0];
	value	= key_value[1];
}

void BMInfo::parse(std::vector<std::string> &line)
{
	std::string key,value;
	for(size_t i=1; i<line.size(); i++)
	{
		GetKeyValueFrom(line[i],key,value);
		if (key == "face")
			m_face = Utils::unQuote(value);
		else if(key == "size")
			m_size = atoi(value.c_str());
		else if (key == "bold")
			m_bold = atoi(value.c_str())? true: false;
		else if (key == "italic")
			m_italic = atoi(value.c_str())? true: false;
		else if (key == "charset")
			m_charset = value;
		else if (key == "unicode")
			m_unicode = atoi(value.c_str());
		else if (key == "stretchH")
			m_stretchH = atoi(value.c_str());
		else if (key == "smooth")
			m_smooth = atoi(value.c_str());
		else if (key == "aa")
			m_aa = atoi(value.c_str());
		else if (key == "padding")
		{
			std::vector<std::string> padding = Utils::split(value,',',true);
			for(size_t j=0; j<padding.size() && j<4;j++)
				m_padding[j] = atoi(padding[j].c_str());
		}
		else if (key == "spacing")
		{
			std::vector<std::string> spacing = Utils::split(value,',',true);
			for(size_t j=0; j<spacing.size() && j<2;j++)
				m_spacing[j] = atoi(spacing[j].c_str());
		}
		else if (key == "outline")
			m_outline = atoi(value.c_str());
		else
		//	debugOut << "BMInfo::parse => Unknown type:" << key.c_str() << std::endl;
			lprintfln("BMInfo::parse at line %d => Unknown type: %s/n",(int)i,key.c_str());

	}
}

void BMCommon::parse(std::vector<std::string> &line)
{
	std::string key,value;
	for(size_t i=1; i<line.size(); i++)
	{
		GetKeyValueFrom(line[i],key,value);
		if (key == "lineHeight")
			m_lineHeight = atoi(value.c_str());
		else if (key == "base")
			m_base = atoi(value.c_str());
		else if (key == "scaleW")
			m_scaleW = atoi(value.c_str());
		else if (key == "scaleH")
			m_scaleH = atoi(value.c_str());
		else if (key == "pages")
			m_pages = atoi(value.c_str());
		else if (key == "packed")
			m_packed = atoi(value.c_str());
		else if (key == "alphaChnl")
			m_alphaChnl = atoi(value.c_str());
		else if (key == "redChnl")
			m_redChnl = atoi(value.c_str());
		else if (key == "greenChnl")
			m_greenChnl = atoi(value.c_str());
		else if (key == "blueChnl")
			m_blueChnl = atoi(value.c_str());
		else
//			debugOut << "BMCommon::parse => Unknown type:" << key.c_str() << std::endl;
			lprintfln("BMCommon::parse at line %d => Unknown type:%s/n",(int)i,key.c_str());
	}
}

void BMPage::parse(std::vector<std::string> &line)
{
	std::string key,value;
	for(size_t i=1; i<line.size(); i++)
	{
		GetKeyValueFrom(line[i],key,value);
		if (key == "id")
			m_id = atoi(value.c_str());
		else if (key == "file")
			m_file = Utils::unQuote(value);
		else
//			debugOut << "BMPage::parse => Unknown type:" << key.c_str() << std::endl;
			lprintfln("BMPage::parse at line %d => Unknown type:%s/n",(int)i,key.c_str());
	}
}

void BMChar::parse(std::vector<std::string> &line)
{
	std::string key,value;
	for(size_t i=1; i<line.size(); i++)
	{
		GetKeyValueFrom(line[i],key,value);
		if (key == "id")
			m_id = atoi(value.c_str());
		else if (key == "x")
			m_x = atoi(value.c_str());
		else if (key == "y")
			m_y = atoi(value.c_str());
		else if (key == "width")
			m_width = atoi(value.c_str());
		else if (key == "height")
			m_height = atoi(value.c_str());
		else if (key == "xoffset")
			m_xoffset = atoi(value.c_str());
		else if (key == "yoffset")
			m_yoffset = atoi(value.c_str());
		else if (key == "xadvance")
			m_xadvance = atoi(value.c_str());
		else if (key == "page")
			m_page = atoi(value.c_str());
		else if (key == "chnl")
			m_chnl = atoi(value.c_str());
		else
//			debugOut << "BMChar::parse => Unknown type:" << key.c_str() << std::endl;
			lprintfln("BMChar::parse at line %d => Unknown type:%s/n",(int)i,key.c_str());
	}
}

void BMKerning::parse(std::vector<std::string> &line)
{
	std::string key,value;
	for(size_t i=1; i<line.size(); i++)
	{
		GetKeyValueFrom(line[i],key,value);
		if (key == "first")
			m_first = atoi(value.c_str());
		else if (key == "second")
			m_second = atoi(value.c_str());
		else if (key == "amount")
			m_amount = atoi(value.c_str());
		else 
//			debugOut << "BMKerning::parse => Unknown type:" << key.c_str() << std::endl;
			lprintfln("BMKerning::parse at line %d => Unknown type:%s/n",(int)i,key.c_str());
	}
}

bool FontClass::LoadFontData(MAHandle resource)
{
	char delim = ' ';
	bool trim = true;
	ReadCSV csv;


	csv.load(resource,delim,trim);
	lprintfln("Info: FontClass::LoadFontData => loaded db size=%i/n",(int)csv.getDB().size());



	std::vector<std::vector<std::string> >& lineData = csv.getDB();
	std::string key,value;
	
	for(size_t i=0; i<lineData.size(); i++)			// array of lines
	{
		// Note beginning of each line there is a chapter tag.
		// 1. get header info..			"info"
		// 2. get common info header..	"common"
		// 3. get page ID..				"page"
		// 4. get count chars (items)	"chars"
		// 5. get char type array		"char"
		// 6. get count kernings (items)"kernings"
		// 7. get kerning				"kerning"
		std::vector<std::string> &line = lineData[i];
//		lprintfln("FontClass::LoadFontData => line %d => tokens %d",(int)i,(int)line.size());

		if (line.size() < 2)
		{
			lprintfln("INFO FontClass::LoadFontData => Skip line %d => tokens %d",(int)i,(int)line.size());
			continue;
		}
		
		BMChar		bmchar;
		BMKerning	bmkerning;
		BMPage		bmpage;

		ParseBMFont state = getBMType(line[0]);
		switch(state)
		{
			case PBM_INFO:		m_info.parse(line);		break;
			case PBM_COMMON:	m_common.parse(line);		break;
			case PBM_PAGE:		bmpage.parse(line); m_pages.push_back(bmpage);	break;
			case PBM_CHARS:		m_nchars = GetValue(line[1]);//	lprintfln("FontClass::LoadFontData => PBM_CHARS n_kernings=%d",m_nchars);
				break;
			case PBM_CHAR:		bmchar.parse(line); m_chars[bmchar.m_id] = bmchar;	break;
			case PBM_KERNINGS:	m_nkernings = GetValue(line[1]);
				lprintfln("FontClass::LoadFontData => PBM_KERNINGS n_kernings=%d",m_nkernings);
				break;
			case PBM_KERNING:
//				lprintfln("FontClass::LoadFontData => PBM_KERNING %d",(int)line.size());
//				for(size_t i=0; i<line.size();++i)
//					lprintfln("FontClass::LoadFontData =>%d %s",(int)i,line[i].c_str());

				bmkerning.parse(line);
				m_kernings[bmkerning.m_first].push_back(bmkerning);
				break;
			default: // Note check last empty line what happens there?
			{	//debugOut << "FontClass::LoadFontData => Unknown type:" << line[0].c_str() << std::endl;
				lprintfln("FontClass::LoadFontData => Unknown type:%s",line[0].c_str());
				return false;
			}
		}
	}
	lprintfln("FontClass::LoadFontData => parsed %d chars, %d kernings",m_nchars,m_nkernings);

	return true;
}

void FontClass::ReleaseFontData()
{
	// Release the font data array.
	return;
}

void FontClass::ReleaseTexture()
{
	// Release the texture object.
	return;
}

GLuint FontClass::GetTexture(int i)
{
	return m_texStores[i].m_texture;
}

float FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter, second;
	float start = drawX;
	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;
	
	// Draw each letter onto a quad.
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]);
		second = ((int)sentence[i+1]);
		std::hash_map<int, std::vector<BMKerning> >::iterator itk = m_kernings.find(letter);
		std::hash_map<int,BMChar>::iterator itc = m_chars.find(letter);



		std::vector<BMKerning> *kernings = &itk->second;
		BMChar *bc = &itc->second;

		// If the letter is a space then just move over three pixels.
		if(bc == 0)	// unknown default
		{
			drawX = drawX + 3.0f;	// Could be other values.
		}
		else
		{
			// First triangle in quad.
			float tx,ty,tw,th,sw,sh,w,h,ox,oy,posX,posY,ax,ks;
			// positions and relative ofsets etc.
			ox = (float)bc->m_xoffset;
			oy = (float)bc->m_yoffset;
			w  = (float)bc->m_width;
			h  = (float)bc->m_height;
			ax = (float)bc->m_xadvance;
			ks = 0.0f;

			// texture coordinates. 0..1
			sw = (float)m_common.m_scaleW;
			sh = (float)m_common.m_scaleH;
			tx = (float)bc->m_x/sw;
			ty = (float)bc->m_y/sh;
			tw = w/sw;
			th = h/sh;

			posX = drawX + ox;	
			posY = drawY - oy;

			vertexPtr[index].position = glm::vec3(posX, posY, 0.0f);  // Top left.
			vertexPtr[index].texture = glm::vec2(tx, ty);
			index++;

			vertexPtr[index].position = glm::vec3((posX+w), (posY-h), 0.0f);  // Bottom right.
			vertexPtr[index].texture = glm::vec2(tx+w, ty+h);
			index++;

			vertexPtr[index].position = glm::vec3(posX, (posY-h), 0.0f);  // Bottom left.
			vertexPtr[index].texture = glm::vec2(tx, ty+h);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = glm::vec3(posX, posY, 0.0f);  // Top left.
			vertexPtr[index].texture = glm::vec2(tx, ty);
			index++;

			vertexPtr[index].position = glm::vec3(posX+w, posY, 0.0f);  // Top right.
			vertexPtr[index].texture = glm::vec2(tx+w, ty);
			index++;

			vertexPtr[index].position = glm::vec3((posX+w), (posY-h), 0.0f);  // Bottom right.
			vertexPtr[index].texture = glm::vec2(tx+w, ty+h);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.

			if (kernings && second)
			{
				for(size_t j=0;j<kernings->size(); j++)
				{
					BMKerning &kn = kernings->at(j);
					if (second == kn.m_second)
					{
						ks = (float)kn.m_amount;
						break;
					}
				}
			}
			//      start posX + width + advanceX + kerning (negative values)
			drawX = posX + w + ax + ks;	
		}
	}
	return drawX-start;
}


// A SIMPLYFIED TEXT RENDERING CLASS
//-----------------------------------------------------

RenderText::RenderText()
:	m_font(0),
	m_width(0), 
	m_height(0), 
/*	m_pContext(0),
	m_pDevice(0),
	m_pVertexBuffer(0),
	m_pIndexBuffer(0),
	m_pSampleState(0),
	m_alphaDisableBlendingState(0),
	m_alphaEnableBlendingState(0), */
	m_world(1),
	m_view(1),
	m_proj(1)
{
}

RenderText::~RenderText()
{
	Release();
}

void RenderText::Release()
{
/*
	SAFE_RELEASE(m_alphaEnableBlendingState);
	SAFE_RELEASE(m_alphaDisableBlendingState);
	SAFE_RELEASE(m_pSampleState);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
*/
}
// Suppose we could use text box support with the width / height
bool RenderText::Initiate(/*LPDIRECT3DDEVICE pDevice, LPDEVICECONTEXT pContext,*/ float width, float height, FontClass *font)
{
	m_width		= width;
	m_height	= height;
//	m_pDevice	= pDevice;
//	m_pContext	= pContext;
	m_font		= font;

	// This could be commonly used. 
	// TODO create a common blend state class that holds all blendstates we need.
/*
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable			= TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	HRESULT result = pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(result))
		return false;

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = pDevice->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(result))
	{
		SAFE_RELEASE(m_alphaEnableBlendingState);	// release first blend state.
		return false;
	}

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;

	memset(&samplerDesc,0,sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias		= 0.0f;
	samplerDesc.MaxAnisotropy	= 1;
	samplerDesc.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;	// D3D11_DEPTH_STENCIL_DESC should do the job unless we want to deal with deffered system
	samplerDesc.BorderColor[0]	= 0;
	samplerDesc.BorderColor[1]	= 0;
	samplerDesc.BorderColor[2]	= 0;
	samplerDesc.BorderColor[3]	= 0;
	samplerDesc.MinLOD			= 0;
	samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = pDevice->CreateSamplerState(&samplerDesc, &m_pSampleState);
	if(FAILED(result))
		return false;

	if (m_font)
		createDefaultBuffers(256);

	// initiate ShaderStore vtx & pix shader for FONT.
	std::string name = "font";
	Shaders sh = SHADER_FONT;
	g_globals.Dx().CreateShader(pDevice,&m_pShader,name,sh); */
	return true;
}

void RenderText::releaseDefaultBuffer()
{
//	SAFE_RELEASE(m_pVertexBuffer);
//	SAFE_RELEASE(m_pIndexBuffer);
}

bool RenderText::createDefaultBuffers(int strSz)
{
/*	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	// make sure that any previous vertex or index buffer are cleared
	releaseDefaultBuffer();

	const int nChars = strSz;
	const int sz = nChars*6;	// 4 vertices (quad) / character (sprite)

	int numElements,strideVtx,strideInstanceDmy; 
	GetVertexElement(SHADER_FONT, numElements, strideVtx, strideInstanceDmy);

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage					= D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth				= strideVtx * sz;
	vertexBufferDesc.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags				= 0;
	vertexBufferDesc.StructureByteStride	= 0;

	// Create the vertex buffer.
	HRESULT result = m_pDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_pVertexBuffer);
	if(FAILED(result))
		return false;

	const int isz = 6*nChars;

	// Create the index array.
	unsigned short *indices = new unsigned short[isz];
	if(!indices)
		return false;

	// Initialize the index array.
	for(int i=0; i<isz; i++)
		indices[i] = i;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth			= sizeof(short) * isz;
	indexBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags		= 0;
	indexBufferDesc.MiscFlags			= 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem					= indices;
	indexData.SysMemPitch				= 0;
	indexData.SysMemSlicePitch			= 0;

	// Create the index buffer.
	result = m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	delete [] indices;

	return (FAILED(result))? false: true;*/
	return true;
}

void RenderText::SetFont(FontClass *font)
{
	m_font = font;
}

float RenderText::DrawText(const char*str,float x,float y, glm::vec4 &rgba)
{

/*
	float width = 0;
	// first time, create intex and vertex tables, for generic text usage. that scales with the text.
	// once there is a index and vertex tables then copy our data to it via map, in this case each sting can be uniqely used without any cacheing issues.. 
	// it will be useing same buffers from the cpu.. 
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	BYTE *verticesPtr;

	float drawX = (float)(((m_width / 2) * -1) + x);
	float drawY = (float)((m_height / 2) - y);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	
	// Create the vertex array.
	int sz			= m_pShader->getVertexShaderStore()->VertexStride();	// get stride in bytes / vertex
	int num			= strlen(str);											// get number chars
	m_nIndex		= num * 6;
	BYTE*vertices	= new BYTE[sz*6*num];									// quad * num
	if(!vertices)
		return false;

	width = m_font->BuildVertexArray((void*)vertices, str, drawX, drawY);			// get vertex array from string,

	// Lock the vertex buffer so it can be written to.
	HRESULT result = m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);	// get map
	if(FAILED(result))
	{
		delete [] vertices;
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (BYTE*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sz*6*num));	

	// Unlock the vertex buffer.
	m_pContext->Unmap(m_pVertexBuffer, 0);

	delete [] vertices;
	vertices = 0;

	m_matrixParam[0] = &m_proj;
	m_matrixParam[1] = &m_view;
	m_matrixParam[2] = &m_world;
	
	m_pShader->renderUpdates(m_pDevice,(void*)&FontParam(&rgba,m_matrixParam,1),1);	// Shader updates, preparing constant tables etc.

	{
		VertexShaderStore*	stv		= m_pShader->getVertexShaderStore();
		PixelShaderStore*	stp		= m_pShader->getPixelShaderStore();
		LPXCONSTANTTABLE	cbufv	= stv->ConstantTable();
		LPXCONSTANTTABLE	cbufp	= stp->ConstantTable();
		m_pContext->VSSetShader( stv->VertexShader(), NULL, 0 );
		if (cbufv)
			m_pContext->VSSetConstantBuffers( 0, 1, &cbufv );			// note for future ref, they might not need to be updated all the time.. but for time being better safe then sorry

		m_pContext->PSSetShader( stp->PixelShader(), NULL, 0 );
		if (cbufp)
			m_pContext->PSSetConstantBuffers( 1, 1, &cbufp );

		ID3D11ShaderResourceView *textures[2] = {m_font->GetTexture(),0};
		m_pContext->PSSetShaderResources(0, 1, textures);		// Set the texture
		m_pContext->PSSetSamplers(0, 1, &m_pSampleState);			// Set its texture states..
		m_pContext->IASetInputLayout( stv->VertexLayout() );

		// Initiate default.
		UINT stride[2]	= {stv->VertexStride(),0};
		UINT offset[2]	= {0,0};	// changes rarely
		ID3D11Buffer* bufferPointers[2] = {m_pVertexBuffer,0};

		DXGI_FORMAT val = DXGI_FORMAT_R16_UINT;
		FLOAT bfactor[] = {1.0f, 1.0f, 1.0f, 1.0f };		

		m_pContext->IASetIndexBuffer( m_pIndexBuffer, val, 0 );
		m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		//pContext->OMSetBlendState( m_pBlendState, bfactor, 0xfff);	//donne( pBS );
		//pContext->OMSetDepthStencilState( m_pDepthStencilState, 1 );	

		//if (isInstancing)
		//{
		//	stride[1]			= stv->InstanceStride();
		//	bufferPointers[1]	= pMaterial->s_pInstanceBuffer;
		//	pContext->IASetVertexBuffers( 0, 2, bufferPointers, stride, offset );
		//	pContext->DrawIndexedInstanced( pMaterial->m_nIndex, pMaterial->s_pInstanceData.size(),0,0,0);
		//	cnt += pMaterial->s_pInstanceData.size();
		//	optcnt++;
		//}
		//else
		{
			m_pContext->IASetVertexBuffers( 0, 1, bufferPointers, stride, offset );
			m_pContext->DrawIndexed(m_nIndex,0,0);
		}

	}
	// Render the text using the font shader.
	//result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), 
	//							  pixelColor);


	return width;*/
	return true;
}


