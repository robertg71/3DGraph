/*
 * Text.h
 *
 *  Created on: Jan 31, 2013
 *      Author: CodeArt
 */

#ifndef TEXT_H_
#define TEXT_H_

#include "GFont.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include "Shaders.h"
namespace Graph{
class Scene;

class RenderText
{
public:
	RenderText();
	~RenderText();

	bool Init(/*LPDIRECT3DDEVICE pDevice, LPDEVICECONTEXT pContext,*/ float screenWidth, float screenHeight,BMFont *font=0);
	void SetFont(BMFont *font);
	float DrawText(const char*str,float x,float y,glm::vec4 &rgba, Scene &scene);
	void Release();

	void SetProjMatrix(glm::mat4 &proj)		{m_proj = proj;}
	void SetWorldMatrix(glm::mat4 &world) 	{m_world = world;}
	void SetViewMatrix(glm::mat4 &view)		{m_view = view;}

protected:
	bool createDefaultBuffers(int strSz);
	void releaseDefaultBuffer();

	BMFont			*m_font;
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
	ShaderStore				*m_pShader;*/
//	int						m_nIndex;
//	GLuint					m_shader;
	TextShader				m_textShader;

};

}


#endif /* TEXT_H_ */
