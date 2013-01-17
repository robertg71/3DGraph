/*
 * Graph.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: CodeArt
 */

#include "graph.h"
#include <GLES2/gl2.h>

namespace Graph
{
	/*
	 * helper function to set the vertex array index array
	 */
	void Bar::createBar3D()
	{
		// Define quad vertices. origo Y = 0
	 const float Vertices[] = {
		    0.5f, 0.0f, 0.5f,
		    0.5f, 1.0f, 0.5f,
		    -0.5f, 1.0f, 0.5f,
		    -0.5f, 0.0f, 0.5f,
		    0.5f, 0.0f, -0.5f,
		    0.5f, 1.0f, -0.5f,
		    -0.5f, 1.0f, -0.5f,
		    -0.5f, 0.0f, -0.5f,
		};

		const unsigned short Indices[] = {
		    // Front
		    0, 1, 2,
		    2, 3, 0,
		    // Back
		    4, 6, 5,
		    4, 7, 6,
		    // Left
		    2, 7, 3,
		    6, 7, 2,
		    // Right
		    0, 4, 1,
		    1, 4, 5,
		    // Top
		    6, 2, 1,
		    5, 6, 1,
		    // Bottom
		    0, 3, 7,
		    0, 7, 4
		};

		for(int i=0; i<8*3; i+=3)
		{
			mVertices.push_back(glm::vec3(Vertices[i],Vertices[i+1],Vertices[i+2]));
		}

		for(int i=0; i<12*3; i++)
		{
			mFaces.push_back(Indices[i]);
		}

	}



}


