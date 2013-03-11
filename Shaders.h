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

#ifndef SHADERS_H_
#define SHADERS_H_

#include <GLES2/gl2.h>


/**
 * \brief, base class for Shader
 */
class Shader
{
public:
	virtual ~Shader(){}

	virtual void init() 		= 0;	// initiate loads shader and setsup each parameter locations
};

/**
 * \brief BarShader class
 */
struct BarShader : public Shader
{
	BarShader() {}				// NOTE common problem do not initiate in constr.
	virtual ~BarShader(){}

	GLuint 	mShader;			// shader for bars
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixPVW;			// Shader Perspective Projection
	GLuint	mTPos;				// Translate position
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a bar.
	GLuint 	mElementbuffer;		// Element buffer holding the index buffer for bars
	GLuint 	mVertexbuffer;		// Vertex buffer for bars. (cube)

	virtual void init();
};

/**
 * \brief LineShader class
 */
struct LineShader : public Shader
{
	LineShader() {}
	virtual ~LineShader(){}

	// Line location parameters
	GLuint 	mShader;			// shader for bars
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixPVW;			// Shader Perspective * View * World
	GLuint  mTPos;				// Translation position
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a bar.
	GLuint	mLength;			// Shader Line Length (line scales up 0..LineLength)
	GLuint 	mVertexbuffer[3];		// Vertex buffer for Axis aligned lines. (world)

	virtual void init();
};

/**
 * \brief TextShader class
 */
struct TextShader : public Shader
{
	TextShader() {}
	virtual ~TextShader(){}

	GLuint 	mShader;			// shader for Text
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixPVW;			// Shader Perspective Projection
	GLuint  mTPos;				// Translation position
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a Text.
	GLuint	mTexture;			// Texture for the font

	virtual void init();
};

// Helper functions for common use.
/**
 * \brief checkGLError, checks basic gl error message.
 * @param where, input string from the system
 */
void checkGLError(const char* where);									// Common GL error handling

/**
 * \brief loadShader, load shader function
 * @param shaderSrc
 * @param type
 * @return
 */
GLuint loadShader(const char *shaderSrc, GLenum type);					// Loads one type of shader

/**
 * \brief loadShaders, mother function that handles both vertex and fragment shader.
 * @param shader_vtx
 * @param shader_frg
 * @return
 */
GLuint loadShaders(const char *shader_vtx, const char *shader_frg);		// Loads both vertex and fragment shader


#endif /* SHADERS_H_ */
