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

typedef std::pair<std::string, void * > 	TextCachePair;
typedef std::hash_map<std::string,void * > 	TextCacheTable;

/*
 * RenderText class generic text rendering class based on the output
 * NOTE RenderText Class can handle one Texture for the whole font.
 * This is due to speedup of the rendering system to avoid texture changes and multiple texture switching within one string.
 */

class RenderText
{
public:
	RenderText();
	virtual ~RenderText();
	virtual bool Init(float screenWidth, float screenHeight,IFont *font=0);
	virtual void SetFont(IFont *font);
	virtual float DrawText(const char*str,glm::vec3 &pos, glm::vec4 &rgba, float gridWitdth, float gridHeight, glm::mat4 &projectionViewWorldMat, float tick = 0, bool bUseCache = false );

protected:
	void Release();

	glm::vec4 *RenderText::getVertices(const char *str, bool bUseCache);	// helper function to create or retrieved cached buffer
	void releaseVertices(glm::vec4 *vertices, bool bUseCache);

	IFont			*m_font;		// current pointer for the font to use
	float			m_width;		// screen width
	float			m_height;		// screen height
	TextShader		m_textShader;	// Text shader to be used for the rendering
	TextCacheTable	m_textCache;	// using text cache re using vertex buffers for specific text.
};

#endif /* TEXT_H_ */
