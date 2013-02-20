/*
 * IFont.h
 *
 *  Created on: Feb 19, 2013
 *      Author: CodeArt
 */

#ifndef IFONT_H_
#define IFONT_H_

#include <glm/glm.hpp>
#include <vector>

class IFont
{
public:
	virtual ~IFont() {};
	virtual bool Init(MAHandle fontResource, std::vector<MAHandle> &textureResources) = 0;
	virtual void Clear() = 0;
	virtual GLuint GetTexture(int i=0) = 0;
	virtual float BuildVertexArray(glm::vec4 * vertexPtr, const char * str, float dX, float dY, float scaleX, float scaleY) = 0;
};

#endif /* IFONT_H_ */
