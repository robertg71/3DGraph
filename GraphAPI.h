/*
 * GraphAPI.h
 *
 *  Created on: Feb 13, 2013
 *      Author: CodeArt
 */

#ifndef GRAPHAPI_H_
#define GRAPHAPI_H_

#include <glm/glm.hpp>
class IFont;

namespace MoGraph
{
	class Scene;

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
	};
}

#endif /* GRAPHAPI_H_ */
