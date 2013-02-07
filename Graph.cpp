/*
 * Graph.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: CodeArt
 */
#include <mavsprintf.h>
#include "graph.h"
#include "GFont.h"
#include "RenderText.h"
#include "MAHeaders.h"
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

		// set up the font
/*		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);
		mFont.Init(R_BOX_FNT, fontTexArray);
		mRenderText.Init(10.0f,10.0f,&mFont);
*/

		const float aspect = 3.0f/4.0f;
		const float cx = getCx();		// requires grid to be set
		const float cz = getCz();		// requires grid to be set
		const float res = glm::sqrt(cx*cx+cz*cz);
		mDistToCam 		= res;

		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		mProjection 	= glm::perspective(45.0f, aspect, 0.1f, 100.0f);
		mWorld			= glm::mat4(1.0f);	// set up an identity matrix

		if (bFitToScreen == true)
		{
			// Camera matrix
			mView       = glm::lookAt(
			glm::vec3(0.0f,res*0.5f,res*1.2f/aspect), // Camera is at (0,x/2,x*(1/aspec)), in World Space
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


	void AxisMgr::init()
	{
		mShader.init();
		for (size_t i=0; i<mAxis.size();i++)
		{
			Graph::Axis &axis = mAxis[i];

			// Generate a vertex buffer for all axis (line)
			glGenBuffers(1, &mShader.mVertexbuffer[i]);
			glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, axis.size() * sizeof(glm::vec3), &axis.vertices()[0], GL_STATIC_DRAW);
		}
	}

	void AxisMgr::draw()
	{
		float tick = mScene.getTick();

		LineShader &shader = getShader();

//		glCullFace(GL_BACK);
//		glFrontFace(GL_CCW);
		glDisable(GL_CULL_FACE);

		glUseProgram(shader.mShader);
		checkGLError("glUseProgram");

		glm::vec3 sv(0.5f,1.0f,0.5f);

		// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
		// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
		glUniform1f(shader.mTimeLoc, tick);
		checkGLError("glUniform1f");
		glUniform2f(shader.mResolutionLoc, 1.0f/(float)mScene.getWidth(), 1.0f/(float)mScene.getHeight());
		checkGLError("glUniform2f");
		glUniformMatrix4fv(shader.mMatrixP, 1, GL_FALSE, &mScene.getProjectionMat()[0][0]);
		checkGLError("glUniformMatrix4fv");
		glUniformMatrix4fv(shader.mMatrixV, 1, GL_FALSE, &mScene.getViewMat()[0][0]);
		checkGLError("glUniformMatrix4fv");
		glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
		checkGLError("glUniform3fv");

		const float centerX = mScene.getCx()*1.01f;
		const float centerZ = mScene.getCz()*1.01f;

		for(size_t i=0; i<mAxis.size(); i++)
		{
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(shader.mAttribVtxLoc);
			glBindBuffer(GL_ARRAY_BUFFER, shader.mVertexbuffer[i]);
			glVertexAttribPointer(
				shader.mAttribVtxLoc,      // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			checkGLError("glEnableVertexAttribArray");
			glLineWidth(2);

			glm::vec4 col(0.5f,0.5f,0.5f,1.0f);
			glm::vec3 llength(-centerX*2.0f,5.0f,centerZ*2.0f);
			glUniform4fv(shader.mColor,1, (float *)&col.x);
			glUniform3fv(shader.mLength,1, (float *)&llength.x);				// mScale location => variable "ScaleV" in vertex shader

			glm::mat4 m 	= glm::translate(mScene.getWorldMat(),centerX,0.0f,-centerZ);	// does a mat mul
			glUniformMatrix4fv(shader.mMatrixM, 1, GL_FALSE, &m[0][0]);	// to the mMatrix Location => variable "World" in vertex shader
			glDrawArrays(GL_LINES, 0, 2);

			// Set up grid lines on height for X-Axis
			if (i == 0)
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);
				for (int l=0;l<5;l++)
				{
					glm::mat4 m 	= glm::translate(mScene.getWorldMat(),centerX,(float)l,-centerZ);	// does a mat mul
					glUniformMatrix4fv(shader.mMatrixM, 1, GL_FALSE, &m[0][0]);	// to the mMatrix Location => variable "World" in vertex shader
					glDrawArrays(GL_LINES, 0, 2);
				}
			}
			else if (i==2)	// Set up grid lines in height for Z-Axis
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);
				for (int l=0;l<5;l++)
				{
					glm::mat4 m 	= glm::translate(mScene.getWorldMat(),centerX,(float)l,-centerZ);	// does a mat mul
					glUniformMatrix4fv(shader.mMatrixM, 1, GL_FALSE, &m[0][0]);	// to the mMatrix Location => variable "World" in vertex shader
					glDrawArrays(GL_LINES, 0, 2);
				}
			}

			glDisableVertexAttribArray(shader.mAttribVtxLoc);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		// Clean-up
		//glDisableVertexAttribArray(shader.mAttribVtxLoc);
		glUseProgram(0);

	}

	void BarMgr::init()
	{
		std::vector<unsigned short> & indices = getScene().getIndices();	// Index list of faces

		// make a copy for now because we might need to add multiple indicies to this obj (there is only one index list)
		for(size_t i=0;i<mFaces.size();i++)
		{
			indices.push_back(mFaces[i]);
		}

		mShader.init();
		// Generate a buffer for the vertices
		glGenBuffers(1, &mShader.mVertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices
		glGenBuffers(1, &mShader.mElementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mShader.mElementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	}

	void BarMgr::draw()
	{
		float tick = mScene.getTick();

		glEnable(GL_CULL_FACE);

		BarShader &shader = getShader();
		std::vector<unsigned short> &indices = mScene.getIndices();	// Index list of faces

		// Use the program object   shader and its specific arguments
		glUseProgram(shader.mShader);
		checkGLError("glUseProgram");

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(shader.mAttribVtxLoc);
		glBindBuffer(GL_ARRAY_BUFFER, shader.mVertexbuffer);
		glVertexAttribPointer(
			shader.mAttribVtxLoc,      // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		checkGLError("glEnableVertexAttribArray");

		// bind the Index buffer with vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader.mElementbuffer);

		// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
		// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
		glUniform1f(shader.mTimeLoc, tick);
		checkGLError("glUniform1f");
		glUniform2f(shader.mResolutionLoc, 1.0f/(float)mScene.getWidth(), 1.0f/(float)mScene.getHeight());
		checkGLError("glUniform2f");
		glUniformMatrix4fv(shader.mMatrixP, 1, GL_FALSE, &mScene.getProjectionMat()[0][0]);
		checkGLError("glUniformMatrix4fv");
		glUniformMatrix4fv(shader.mMatrixV, 1, GL_FALSE, &mScene.getViewMat()[0][0]);
		checkGLError("glUniformMatrix4fv");

		// setting up a 2D grid.  prepare const variable for a tight loop
		const int iGridX	= mScene.getGridX();
		const int iGridZ	= mScene.getGridZ();
		const float centerX = mScene.getCx();
		const float centerZ = mScene.getCz();
		glm::vec3 sv(0.5f,0.5f,0.5f);
		int k = 0;
		for(int j=0; j<iGridZ; j++)
		{
			// if grid is even then extra add would be required
			k += 1-(iGridX&1);
			for(int i=0; i<iGridX; i++)
			{
				Graph::Bar &bar = getBar(j*iGridX+i);
//				bar.setValue(1.1f+1.0f*sin(j*0.3f+i*0.3f+1.3f*tick));
				bar.setValue(1.1f+1.0f*(sin(j*0.3f+	1.3f*tick)+cos(i*0.3f+1.3f*tick)));
				glm::mat4 m 	= glm::translate(mScene.getWorldMat(),(centerX-i)-0.5f,0.0f,(-centerZ+j)+0.5f);	// does a mat mul
				sv.y 			= bar.getValue();

				// set test colors for bars.. every second bar
				float c = 0.5f+0.5f*(float)(k&1);
				bar.setColor(1.0f-c,0.75f,c,1.0f);
//				bar.setColor(i/30.0f,0.0f,j/30.0f,1.0f);

				// upload our obj matrix to the vertex shader.
				glUniformMatrix4fv(shader.mMatrixM, 1, GL_FALSE, &m[0][0]);	// to the mMatrix Location => variable "World" in vertex shader
				glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
				glUniform4fv(shader.mColor,1, (float *)&bar.getColor().x);
				glDrawElements(
					 GL_TRIANGLES,      	// mode
					 indices.size(),    	// count
					 GL_UNSIGNED_SHORT,   	// type
					 (void*)0           	// element array buffer offset
				 );
				k++;
			}
		}

		// Clean-up
		glDisableVertexAttribArray(shader.mAttribVtxLoc);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
}












