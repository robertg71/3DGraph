/*
 * Shaders.cpp
 *
 *  Created on: Jan 25, 2013
 *      Author: CodeArt
 */
#include <mavsprintf.h>
#include <maheap.h>

#include "Shaders.h"

#define STRINGIFY(A)  #A

//--------------------------------------------------------------------------------------
// BARS SHADERS
//--------------------------------------------------------------------------------------

// BARS FRAGMENT SHADER
	char fragmentShaderBars[]=STRINGIFY(
		precision lowp 	float;
		varying vec4 	v_color;
		uniform vec2 	resolution;
		uniform float 	time;
		void main( void )
		{
			gl_FragColor = vec4(v_color.x,v_color.y,v_color.z,v_color.w);
		}
	);

	// BARS VERTEX SHADER
	char vertexShaderBars[]=STRINGIFY(
		attribute vec4 vPosition;
		uniform mat4 Projection;
		uniform mat4 View;
		uniform mat4 World;
		uniform vec3 ScaleV;
		uniform vec4 Color;
		varying vec4 v_color;
		void main( void )
		{
			mat4 sm = mat4(1.0);
			sm[0][0] = ScaleV.x;
			sm[1][1] = ScaleV.y;
			sm[2][2] = ScaleV.z;

			v_color = vec4(Color.x*vPosition.y, Color.y*vPosition.y, Color.z*vPosition.y, Color.w);
			gl_Position = (Projection * View * sm * World) * vPosition;
		}
	);

//--------------------------------------------------------------------------------------
// LINES SHADERS
//--------------------------------------------------------------------------------------
	// LINES FRAGMENT SHADER
	char fragmentShaderLines[]=STRINGIFY(
		precision lowp 	float;
		varying vec4 	v_color;
		uniform vec2 	resolution;
		uniform float 	time;
		void main( void )
		{
			gl_FragColor = v_color;
		}
	);

	// LINES VERTEX SHADER
	char vertexShaderLines[]=STRINGIFY(
		attribute vec4 vPosition;
		uniform mat4 Projection;
		uniform mat4 View;
		uniform mat4 World;
		uniform vec3 Length;
		uniform vec4 Color;
		uniform vec3 ScaleV;
		varying vec4 v_color;
		void main( void )
		{
			mat4 sm = mat4(1.0);
			sm[0][0] = ScaleV.x;
			sm[1][1] = ScaleV.y;
			sm[2][2] = ScaleV.z;

			v_color = Color;
			gl_Position = (Projection * View * sm * World) * (vPosition * vec4(Length.xyz,1.0));
		}
	);


	// HELPER FUNCTIONS FOR LOADING SHADER & Error detection
	//----------------------------------------------------------------
	void checkGLError(const char* where)
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			lprintfln("%s: glGetError returned %x", where, err);
		}
	}


	///
	// Create a shader object, load the shader source, and
	// compile the shader.
	//
	GLuint loadShader(const char *shaderSrc, GLenum type) {
		lprintfln("loading shader: %s", shaderSrc);
		checkGLError("begin loading shader");

		GLuint shader;
		GLint compiled;
		// Create the shader object
		shader = glCreateShader(type);
		checkGLError("glCreateShader");

		if (shader == 0) {
			lprintfln("Invalid shader handle!");
			return 0;
		}
		// Load the shader source
		glShaderSource(shader, 1, &shaderSrc, NULL);
		checkGLError("glShaderSource");

		// Compile the shader
		glCompileShader(shader);
		checkGLError("glCompileShader");

		// Check the compile status
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		checkGLError("glGetShaderiv");

		if (!compiled) {
			lprintfln("Error compiling shader");
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen == 0) // android bug.
				infoLen = 1024;
			if (infoLen > 1) {
				char* infoLog = (char*) malloc(sizeof(char) * infoLen);
				glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
				//printf("Error compiling shader:\n%s\n", infoLog);
				lprintfln("Error compiling shader:\n%s\n", infoLog);
				free(infoLog);
			}
			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	///
	// Loads both Vertex and Fragment Shader into one shader setup
	//
	GLuint loadShaders(const char *shader_vtx, const char *shader_frg)
	{
		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint programObject;
		GLint linked;


		// Load the vertex/fragment shaders
		vertexShader = loadShader(shader_vtx, GL_VERTEX_SHADER);
		checkGLError("Load vertex shader");

		fragmentShader = loadShader(shader_frg, GL_FRAGMENT_SHADER);
		checkGLError("Load fragment shader");

		// Create the program object
		programObject = glCreateProgram();
		if (programObject == 0) {
			lprintfln("Could not create program!");
			return FALSE;
		}
		checkGLError("Create program");

		glAttachShader(programObject, vertexShader);
		checkGLError("Attach vertex shader");

		glAttachShader(programObject, fragmentShader);
		checkGLError("Attach fragment shader");

		// Bind vPosition to attribute 0
		glBindAttribLocation(programObject, 0, "vPosition");
		checkGLError("Bind vPosition to vertex shader");


		// Link the program
		glLinkProgram(programObject);

		// Check the link status
		glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
		if (!linked) {
			lprintfln("Failed to link shader!");
			GLint infoLen = 0;
			glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen == 0) // android bug.
				infoLen = 1024;
			if (infoLen > 1) {
				char* infoLog = (char*) malloc(sizeof(char) * infoLen);
				glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
				//esLogMessage("Error linking program:\n%s\n", infoLog);
				lprintfln("Error linking program:\n%s\n", infoLog);
				free(infoLog);
			}
			glDeleteProgram(programObject);
			return FALSE;
		}
		// Store the program object
		return programObject;
	}

///
// LineShader class, contains all indexed references to locations of parameters.
// init() initiates by loading vertex & fragment shader.
//
void LineShader::init()
{
	mShader 		= loadShaders(vertexShaderLines,fragmentShaderLines);
	mTimeLoc 		= glGetUniformLocation(mShader, "time");
	mResolutionLoc 	= glGetUniformLocation(mShader, "resolution");
	mMatrixP 		= glGetUniformLocation(mShader, "Projection");
	mMatrixV 		= glGetUniformLocation(mShader, "View");
	mMatrixM 		= glGetUniformLocation(mShader, "World");
	mLength			= glGetUniformLocation(mShader, "Length");
	mScaleV			= glGetUniformLocation(mShader, "ScaleV");
	mColor			= glGetUniformLocation(mShader, "Color");			// Color of line (vertex shader)
	mAttribVtxLoc	= glGetAttribLocation( mShader, "vPosition");
}


///
// LineShader class, contains all indexed references to locations of parameters.
// init() initiates by loading vertex & fragment shader.
//
void BarShader::init()
{
	mShader 		= loadShaders(vertexShaderBars,fragmentShaderBars);
	mTimeLoc 		= glGetUniformLocation(mShader, "time");			// time tick variable (fragment)
	mResolutionLoc 	= glGetUniformLocation(mShader, "resolution");		// constant resolution of screen (fragment)
	mMatrixP 		= glGetUniformLocation(mShader, "Projection");		// Projection Matrix
	mMatrixV 		= glGetUniformLocation(mShader, "View");			// View Matrix
	mMatrixM 		= glGetUniformLocation(mShader, "World");			// World Matrix
	mScaleV			= glGetUniformLocation(mShader, "ScaleV");			// scale vector (height of bar)
	mColor			= glGetUniformLocation(mShader, "Color");			// Color of one bar (vertex shader)
	mAttribVtxLoc	= glGetAttribLocation( mShader, "vPosition");		// input vertex attrib
}

