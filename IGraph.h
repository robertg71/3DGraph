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

#ifndef GRAPHAPI_H_
#define GRAPHAPI_H_

#include <glm/glm.hpp>

class IFont;

namespace MoGraph
{
	class Scene;
	class TouchInput;

	/**
	 * \brief IGraph class,	interface for the Graph
	 * \note Touch input is optional
	 */
	class IGraph
	{
	public:
		virtual ~IGraph()  {}
		virtual int init(int x,int z, int gridLines, float step, bool bFitScreen, IFont* font,int width,int height) = 0;
		virtual void setBKColor(glm::vec4 &color) = 0;
		virtual void setValues(float *values,int sz) = 0;
		virtual void setColors(glm::vec4 *colors, int sz) = 0;
		virtual void draw() = 0;
		virtual Scene &getScene() = 0;
		virtual TouchInput &getTouch() = 0;
	};
}

#endif /* GRAPHAPI_H_ */
