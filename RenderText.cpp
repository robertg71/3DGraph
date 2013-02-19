/*
 * Text.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: CodeArt
 */

#include <mavsprintf.h>
#include "RenderText.h"
#include "graph.h"
#include "GFont.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// A SIMPLYFIED TEXT RENDERING CLASS
//-----------------------------------------------------

RenderText::RenderText()
:	m_font(0),
	m_width(0),
	m_height(0)
{
}

RenderText::~RenderText()
{
	Release();
}

void RenderText::Release()
{
	TextCacheTable::iterator it;
	for(it=m_textCache.begin();it!=m_textCache.end();it++)
	{
		delete [] static_cast<glm::vec4 *>(it->second);
		it->second = 0;
	}
}

// Suppose we could use text box support with the width / height
bool RenderText::Init(float width, float height, IFont *font)
{
	m_width		= width;
	m_height	= height;
	m_font		= font;

	m_textShader.init();
	// This could be commonly used.
	// TODO create a common blend state class that holds all blendstates we need.
	return true;
}

void RenderText::SetFont(IFont *font)
{
	m_font = font;
}

glm::vec4 *RenderText::getVertices(const char *str, bool bUseCache)
{
	glm::vec4 *vertices = 0;
	int num = 0;
	if (bUseCache)
	{
		std::string key = str;
		num				= key.size();
		TextCacheTable::iterator it = m_textCache.find(key);
		if (it == m_textCache.end())
		{										// doesn't exists in tabler, cache it.
			vertices = new glm::vec4[6*num];  	// create vertex array
			if(!vertices)						// check if memory alocation was ok
				maPanic(1,"RenderText: Failed to allocate vertex buffer");

			if ( (m_textCache.insert(TextCachePair(key,vertices))).second == false )
			{
				// warn in log double insertion failed to insert this one.
				lprintfln("RenderText::TextCache insertion double entry detected of key=\"%s\"",key.c_str());
			}
		}
		else	// exists re use vertex buffer from table
		{
			vertices = static_cast<glm::vec4 *>(it->second);
		}
	}
	else	// Not using cache create one
	{
		num			= strlen(str);						// get number chars
		vertices	= new glm::vec4[6*num];
		if(!vertices)
			maPanic(1,"RenderText: Failed to allocate vertex buffer");
	}
	return vertices;
}

void RenderText::releaseVertices(glm::vec4 *vertices, bool bUseCache)
{
	if (bUseCache == false)
		delete [] vertices;
}

float RenderText::DrawText(const char*str, glm::vec3 &pos, glm::vec4 &rgba, float gridWidth, float gridHeight, glm::mat4 &pvw, float tick, bool bUseCache)
{
//	checkGLError("RenderText::DrawText   Should be ok!");

	float width = 0;
	float drawX = pos.x;
	float drawY = pos.y;
	float drawZ	= pos.z;
	int num = strlen(str);
	glm::vec4 *vertices = 0;
	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	// Create the vertex array.

	vertices = getVertices(str, bUseCache);		// creates or retrieves existing vertex buffer from a cache table

	width = m_font->BuildVertexArray(vertices, str, drawX, drawY, 2.0f/gridWidth, 2.0f/gridHeight);			// get vertex array from string,
	TextShader &shader= m_textShader;

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
	checkGLError("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkGLError("glTexParameteri");
	glUniform1i(shader.mTexture, 0 ); // Texture Unit 0
	checkGLError("RenderText::DrawText   glUniform1i");

	// Once per frame
	glUniform1f(shader.mTimeLoc, tick);
	checkGLError("RenderText::DrawText   glUniform1f");
	glUniform2f(shader.mResolutionLoc, 1.0f/(float)m_width, 1.0f/(float)m_height);
	checkGLError("RenderText::DrawText   glUniform2f");
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

	releaseVertices(vertices, bUseCache);

	return width;			// Generate a buffer for the vertices
}



