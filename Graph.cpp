/*
 * Graph.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: CodeArt
 */
#include <mavsprintf.h>
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
	void BarMgr::create3D()
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
		mFitToScreen = bFitToScreen;
		mGridX = gridX;
		mGridZ = gridZ;
		int newSize = gridX*gridZ;

		lprintfln("Scene::create: %i*%i=%i",mGridX,mGridZ,newSize);
		mBarMgr.addBars(gridX*gridZ);
		lprintfln("vector<Bar> bars, size() = %i == %i", mBarMgr.size(),gridX*gridZ);

		int axis = ((gridX>1)&&(gridZ>1))?3:2;	// how many axis should be displayed 2 or 3 dependent on layout... 2d => 2 => 3d => 3
		mAxisMgr.addAxis(axis);


		const float aspect = 3.0f/4.0f;
		const float cx = getCx();		// requires grid to be set
		const float cz = getCz();		// requires grid to be set
		const float res = glm::sqrt(cx*cx+cz*cz);
		mDistToCam = res;

		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		mProjection = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
		mWorld		= glm::mat4(1.0f);	// set up an identity matrix

		if (bFitToScreen == true)
		{
			// Camera matrix
			mView       = glm::lookAt(
			glm::vec3(0.0f,res*0.5f,res*1.0f/aspect), // Camera is at (0,x/2,x*(1/aspec)), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		}
		else	// set up a default camera matrix
		{
			mView       = glm::lookAt(
			glm::vec3(0.0f,10.0f,20.0f), // Camera is at (0,10,20), in World Space
			glm::vec3(0,0,0), // and looks at the origin
			glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		}
	}

	void AxisMgr::create3D()
	{
		float v[] = { 	0.0f, 0.0f, 0.0f,
						1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f
		};

		mAxis[0].vertices().push_back(glm::vec3(v[0],v[1],v[2]));
		mAxis[0].vertices().push_back(glm::vec3(v[3],v[4],v[5]));
		mAxis[1].vertices().push_back(glm::vec3(v[6],v[7],v[8]));
		mAxis[1].vertices().push_back(glm::vec3(v[9],v[10],v[11]));

		if(mAxis.size()>2)
		{
			mAxis[2].vertices().push_back(glm::vec3(v[12],v[13],v[14]));
			mAxis[2].vertices().push_back(glm::vec3(v[15],v[16],v[17]));
		}
		else
		{
			// error not declared.
		}
	}

	void AxisMgr::addAxis(int n)
	{
		mAxis.resize(n);
		create3D();
	}


}












