/*
 * Text.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: CodeArt
 */

#include <mavsprintf.h>
#include "RenderText.h"
#include "graph.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


namespace MoGraph
{
// A SIMPLYFIED TEXT RENDERING CLASS
//-----------------------------------------------------

RenderText::RenderText()
:	m_font(0),
	m_width(0),
	m_height(0),
	m_world(1),
	m_view(1),
	m_proj(1)
{
}

RenderText::~RenderText()
{
	Release();
}

void RenderText::Release()
{
}

// Suppose we could use text box support with the width / height
bool RenderText::Init(float width, float height, BMFont *font)
{
	m_width		= width;
	m_height	= height;
	m_font		= font;

	m_textShader.init();
	// This could be commonly used.
	// TODO create a common blend state class that holds all blendstates we need.
	return true;
}

void RenderText::SetFont(BMFont *font)
{
	m_font = font;
}


//TODO DISABLE SCENE DEPENDENCY HERE
// should have a separate draw text a direct and a cashed.
float RenderText::DrawText(const char*str,glm::vec3 &pos, glm::vec4 &rgba, Scene &scene)
{
	return DrawText(str, pos, rgba, (float)scene.getGridX(), (float)scene.getGridZ(), scene.getProjectionMat(), scene.getViewMat(), scene.getWorldMat(), scene.getTick());
}

float RenderText::DrawText(const char*str, glm::vec3 &pos, glm::vec4 &rgba, float gridWidth, float gridHeight, glm::mat4 &projectionMat, glm::mat4 &viewMat, glm::mat4 &worldMat, float tick)
{
	checkGLError("RenderText::DrawText   Should be ok!");

	float width = 0;
	float drawX = pos.x;
	float drawY = pos.y;
	float drawZ	= pos.z;

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	// Create the vertex array.
	int num		= strlen(str);											// get number chars

//	lprintfln("RenderText::DrawText size=%d string=\"%s\"\n",num,str);
	glm::vec4 *vertices	= new glm::vec4[6*num];
	if(!vertices)
		return false;

	width = m_font->BuildVertexArray(vertices, str, drawX, drawY, 2.0f/gridWidth, 2.0f/gridHeight);			// get vertex array from string,
	TextShader &shader= m_textShader;

//	lprintfln("shader.mShader = %d",shader.mShader);
	// Use the program object
	glUseProgram(shader.mShader);
	checkGLError("RenderText::DrawText   glUseProgram (Text)");

	// 1. create buffers tri array for simplicity? so that every tri can be handled separately or with a new index buffer could be expensive though???
	// Load the vertex data

	glVertexAttribPointer(shader.mAttribVtxLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float)*4, &vertices[0].x);
	checkGLError("RenderText::DrawText   glVertexAttribPointer (V)");

	glEnableVertexAttribArray(shader.mAttribVtxLoc);
	checkGLError("RenderText::DrawText   glEnableVertexAttribArray (V)");

	glActiveTexture(GL_TEXTURE0 + 0);
	checkGLError("RenderText::DrawText   glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, m_font->GetTexture(0));
	checkGLError("glBindTexture");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLError("glTexParameteri");
	glUniform1i(shader.mTexture, 0 ); // Texture Unit 0
	checkGLError("RenderText::DrawText   glUniform1i");

	// Once per frame
	glUniform1f(shader.mTimeLoc, tick);
	checkGLError("RenderText::DrawText   glUniform1f");
	glUniform2f(shader.mResolutionLoc, 1.0f/(float)m_width, 1.0f/(float)m_height);
	checkGLError("RenderText::DrawText   glUniform2f");

	glm::mat4 pvw = projectionMat * viewMat * worldMat;

	glUniformMatrix4fv(shader.mMatrixPVW, 1, GL_FALSE, &pvw[0][0]);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	glm::vec3 sv(1.0f, 1.0f, 1.0f);
	glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
	checkGLError("RenderText::DrawText   glUniformMatrix3fv");

	glm::vec4 color = rgba;
	glUniform4fv(shader.mColor,1, (float *)&color.x);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	glm::vec4 TPos(0.0f,0.0f,-drawZ, 1.0f);
	glUniform4fv(shader.mTPos, 1, (float *)&TPos.x);
	checkGLError("RenderText::DrawText   glUniformMatrix4fv");

	glDrawArrays(GL_TRIANGLES, 0, 6*num);
	checkGLError("RenderText::DrawText   glDrawArrays");

	// Clean-up
	glDisableVertexAttribArray(shader.mAttribVtxLoc);
	checkGLError("RenderText::DrawText   glDisableVertexAttribArray (Vtx)");

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	checkGLError("RenderText::DrawText   glUseProgram(0)");

	delete [] vertices;

	// Generate a buffer for the vertices
	return true;
}


}

