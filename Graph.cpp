/*
 * Graph.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: CodeArt
 */

#include "graph.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace Graph
{
	/*
	 * helper function to set the vertex array index array
	 */
	void BarMgr::createBar3D()
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

	void Scene::create(int gridX, int gridZ,bool bFitToScreen)
	{
		mGridX = gridX;
		mGridZ = gridZ;
		mBarMgr.addBars(gridX*gridZ);
		mFitToScreen = bFitToScreen;
		float cx = getCx();
		float cz = getCz();
		float res = glm::sqrt(cx*cx+cz*cz);
		mDistToCam = res;

		float aspect = 3.0f/4.0f;
		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		mProjection = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
		// Camera matrix
		mView       = glm::lookAt(
			glm::vec3(0.0f,res*0.5f,res*1.0f/aspect), // Camera is at (4,3,3), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		mWorld		= glm::mat4(1.0f);	// set up an identity matrix
	}
}












