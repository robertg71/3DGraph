/*
 * Shaders.h
 *
 *  Created on: Jan 25, 2013
 *      Author: CodeArt
 */

#ifndef SHADERS_H_
#define SHADERS_H_

#include <GLES2/gl2.h>

class Shader
{
public:
	virtual ~Shader(){}

	virtual void init() 		= 0;	// initiate loads shader and setsup each parameter locations
};

struct BarShader : public Shader
{
	BarShader() {}				// NOTE common problem do not initiate in constr.
	virtual ~BarShader(){}

	GLuint 	mShader;			// shader for bars
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixP;			// Shader Perspective Projection
	GLuint 	mMatrixV;			// Shader Camera View
	GLuint 	mMatrixM;			// Shader World
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a bar.
	GLuint 	mElementbuffer;		// Element buffer holding the index buffer for bars
	GLuint 	mVertexbuffer;		// Vertex buffer for bars. (cube)

	virtual void init();
};

struct LineShader : public Shader
{
	LineShader() {}
	virtual ~LineShader(){}

	// Line location parameters
	GLuint 	mShader;			// shader for bars
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixP;			// Shader Perspective Projection
	GLuint 	mMatrixV;			// Shader Camera View
	GLuint 	mMatrixM;			// Shader World
	GLuint	mLength;			// Shader Line Length (line scales up 0..LineLength)
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a bar.
	GLuint 	mVertexbuffer[3];		// Vertex buffer for Axis aligned lines. (world)

	virtual void init();
};

struct TextShader : public Shader
{
	TextShader() {}
	virtual ~TextShader(){}

	GLuint 	mShader;			// shader for Text
	GLuint 	mAttribVtxLoc;		// Attribute to the vertex shader of vpos location
	GLuint 	mTimeLoc;			// time tick variable for shaders (fragment)
	GLuint 	mResolutionLoc;		// screen resulution
	GLuint 	mMatrixP;			// Shader Perspective Projection
	GLuint 	mMatrixV;			// Shader Camera View
	GLuint 	mMatrixM;			// Shader World
	GLuint 	mScaleV;			// Scale vector of bars. its height...
	GLuint	mColor;				// individual color of a Text.
	GLuint	mTexture;			// Texture for the font

	virtual void init();
};

// Helper functions for common use.
void checkGLError(const char* where);									// Common GL error handling
GLuint loadShader(const char *shaderSrc, GLenum type);					// Loads one type of shader
GLuint loadShaders(const char *shader_vtx, const char *shader_frg);		// Loads both vertex and fragment shader


#endif /* SHADERS_H_ */
