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

#ifndef IFONT_H_
#define IFONT_H_

#include <glm/glm.hpp>
#include <vector>

/**
 * \brief Interface for any Font class to use.
 */
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
