/*
 * Copyright (C) 2013 MoSync AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include "AxisMgr.h"
#include "graph.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <mavsprintf.h>

namespace MoGraph
{

	/**
	 * \brief AxisMgr::create3D  creates default vertex buffer
	 */
	void AxisMgr::create3D()
	{
		const float v[] =
		{
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
		// set up 2D axis X and Y
		mAxisArray[0].vertices().push_back(glm::vec3(v[0],v[1],v[2]));
		mAxisArray[0].vertices().push_back(glm::vec3(v[3],v[4],v[5]));
		mAxisArray[1].vertices().push_back(glm::vec3(v[6],v[7],v[8]));
		mAxisArray[1].vertices().push_back(glm::vec3(v[9],v[10],v[11]));

		if(mAxisArray.size()>2)		// 3D Graph use a 3rd axis in Z.
		{
			mAxisArray[2].vertices().push_back(glm::vec3(v[12],v[13],v[14]));
			mAxisArray[2].vertices().push_back(glm::vec3(v[15],v[16],v[17]));
		}
		else
		{
			lprintfln("AxisMgr::create3d: ERROR Axis system got Graph bars can either be 2D or 3D hence 2 or 3 as input");   			// error not declared.
		}
	}

	/**
	 * \brief AxisMgr::addAxis
	 * @param int n, 	add new axis 1,2,3 dimensions. x,y,z
	 */
	void AxisMgr::addAxis(int n)
	{
		mAxisArray.resize(n);
		create3D();
	}

	/**
	 * \brief AxisMgr::init
	 * initiates buffers for openGL for all Axis
	 */
	void AxisMgr::init()
	{
		mShader.init();
		for (size_t i=0; i<mAxisArray.size();i++)
		{
			Axis &axis = mAxisArray[i];								// get reference of obj.
			glGenBuffers(1, &mShader.mVertexbuffer[i]);					// Generate a vertex buffer for all axis (line)
			glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, axis.size() * sizeof(glm::vec3), &axis.vertices()[0], GL_STATIC_DRAW);
		}
	}

	/**
	 * \brief AxisMgr::draw,	render axis Axis lines,
	 */
	void AxisMgr::draw()
	{
		const float tick = mScene->getElapsedTime();
		LineShader &shader 	= getShader();
		glDisable(GL_CULL_FACE);
		glUseProgram(shader.mShader);
		checkGLError("glUseProgram");

		glm::vec3 sv(1.0f,1.0f,1.0f);

		// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
		// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
		glUniform1f(shader.mTimeLoc, tick);
		checkGLError("glUniform1f");
		glUniform2f(shader.mResolutionLoc, 1.0f/(float)mScene->getWidth(), 1.0f/(float)mScene->getHeight());
		checkGLError("glUniform2f");
		glUniformMatrix4fv(shader.mMatrixPVW, 1, GL_FALSE, &mScene->getPVWMat()[0][0]);
		checkGLError("glUniformMatrix4fv");
		glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
		checkGLError("glUniform3fv");

		const float centerX = mScene->getCx()*mScene->getBoundScale();
		const float centerZ = mScene->getCz()*mScene->getBoundScale();
		const GraphDesc &desc = mScene->getGraphDesc();
		bool bMirror = (desc.bNegGridLines != 0);
		for(size_t i=0; i<mAxisArray.size(); i++)
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

			float totGridHeight = desc.gridStepYLines * desc.gridYLines;


			// Dray Line for Y Axis
			glm::vec4 col(0.5f,0.5f,0.5f,1.0f);
			glm::vec3 llength(-centerX*2.0f,totGridHeight,-centerZ*2.0f);	// length is always abs
			glm::vec4 tpos(centerX, 0.0f,centerZ, 1.0f);
			glUniform4fv(shader.mTPos,1, (float *)&tpos.x);
			glUniform4fv(shader.mColor,1, (float *)&col.x);
			glUniform3fv(shader.mLength,1, (float *)&llength.x);				// mScale location => variable "ScaleV" in vertex shader

			glDrawArrays(GL_LINES, 0, 2);
			if (bMirror)
			{
				llength.y = -llength.y;
				glUniform3fv(shader.mLength,1, (float *)&llength.x);				// mScale location => variable "ScaleV" in vertex shader
				glDrawArrays(GL_LINES, 0, 2);
			}


			// Set up grid lines on height for X-Axis
			if (i == 0)
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);

				for (int l=1;l<mGridLines;l++)
				{
					float gridY = static_cast<float>(l) * mGridStep;

					glm::vec4 tpos(centerX, gridY, centerZ, 1.0f);
					glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
					glDrawArrays(GL_LINES, 0, 2);

					if (bMirror)
					{
						tpos.y = -tpos.y;
						glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
						glDrawArrays(GL_LINES, 0, 2);
					}
				}
			}
			else if (i==2)	// Set up grid lines in height for Z-Axis
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);
				for (int l=1;l<mGridLines;l++)
				{
					float gridY = static_cast<float>(l) * mGridStep;
					glm::vec4 tpos(centerX, gridY, centerZ, 1.0f);
					glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
					glDrawArrays(GL_LINES, 0, 2);

					if (bMirror)
					{
						tpos.y = -tpos.y;
						glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
						glDrawArrays(GL_LINES, 0, 2);
					}
				}
			}
			glDisableVertexAttribArray(shader.mAttribVtxLoc);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		// Clean-up
		glUseProgram(0);
	}
}
