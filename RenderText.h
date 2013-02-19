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
#include <hash_map>
#include "Shaders.h"
namespace MoGraph
{
	class Scene;

	typedef std::pair<std::string, void * > TextCachePair;
	typedef std::hash_map<std::string,void * > TextCacheTable;

	class RenderText
	{
	public:
		RenderText();
		~RenderText();

		bool Init(float screenWidth, float screenHeight,BMFont *font=0);
		void SetFont(BMFont *font);
		float DrawText(const char*str,glm::vec3 &pos, glm::vec4 &rgba, Scene &scene, bool bUseCache);
		float DrawText(const char*str,glm::vec3 &pos, glm::vec4 &rgba, float gridWitdth, float gridHeight, glm::mat4 &projectionViewWorldMat, float tick = 0, bool bUseCache = false );

		void Release();

		void SetProjMatrix(glm::mat4 &proj)		{m_proj 	= proj;}
		void SetWorldMatrix(glm::mat4 &world) 	{m_world 	= world;}
		void SetViewMatrix(glm::mat4 &view)		{m_view		= view;}

	protected:
		glm::vec4 *RenderText::getVertices(const char *str, bool bUseCache);	// helper function to create or retrieved cached buffer
		void releaseVertices(glm::vec4 *vertices, bool bUseCache);

		BMFont			*m_font;		// current pointer for the font to use
		float			m_width;		// screen width
		float			m_height;		// screen height
		glm::mat4		m_world;		// World matrix
		glm::mat4		m_view;			// View matrix
		glm::mat4		m_proj;			// Perspective projection matrix
		TextShader		m_textShader;	// Text shader to be used for the rendering
		TextCacheTable	m_textCache;	// using text cache re using vertex buffers for specific text.
	};

}


#endif /* TEXT_H_ */
