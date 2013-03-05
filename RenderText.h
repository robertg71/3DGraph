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

struct VertStore
{
	VertStore(void *vertices, float width, glm::vec2 &scaleXZ) : mVertices(vertices), mWidth(width), mScaleXZ(scaleXZ) {};

	void 		*mVertices;
	float		mWidth;
	glm::vec2 	mScaleXZ;
};

typedef std::pair<std::string, VertStore > 	TextCachePair;
typedef std::hash_map<std::string,VertStore > 	TextCacheTable;

/*
 * RenderText class generic text rendering class based on the output
 * NOTE RenderText Class can handle one Texture for the whole font.
 * This is due to speedup of the rendering system to avoid texture changes and multiple texture switching within one string.
 */
enum BlendType{ BL_ADDITIVE, BL_TRANSLUCENT, BL_SUBTRACTIVE, BL_TRANSPARENT};

class RenderText
{
public:
	RenderText();
	virtual ~RenderText();
	virtual bool init(float screenWidth, float screenHeight,IFont *font=0);
	virtual void setFont(IFont *font);
	virtual float drawText3D(const char*str,glm::vec3 &pos, glm::vec4 &rgba, glm::mat4 &projectionViewWorldMat, float tick = 0, bool bUseCache = false );
	virtual float drawText(const char *str,glm::vec3 &pos, glm::vec4 &rgba, float tick = 0.0f);
	virtual float getTextWidth(const char *str);
	virtual void setScale(float x,float z) {m_scaleX = x; m_scaleZ = z;}
protected:
	void release();

	glm::vec4 *getVertices(const char *str, bool bUseCache, float *width);	// helper function to create or retrieved cached buffer
	void releaseVertices(glm::vec4 *vertices, bool bUseCache);

	IFont			*m_font;		// current pointer for the font to use
	float			m_width;		// screen width
	float			m_height;		// screen height
	float 			m_scaleX;
	float			m_scaleZ;
	glm::vec3		m_pos;
	TextShader		m_textShader;	// Text shader to be used for the rendering

	TextCacheTable	m_textCache;	// using text cache re using vertex buffers for specific text.
	BlendType		m_blendType;
};

#endif /* TEXT_H_ */
