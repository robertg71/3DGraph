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
#include <mavsprintf.h>

#include <MAUtil/GLMoblet.h>
#include <GLES2/gl2.h>

//#define GLM_FORCE_INLINE
//#include <limits>
//#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <maheap.h>
#include <conprint.h>
#include "graph.h"
#include "Shaders.h"

using namespace MAUtil;



class MyGLMoblet: public GLMoblet {
private:
	// Bars location parameters


	int 	mWidth, mHeight;	// Screen resolution in ABS form e.g. 640,480
	int  	mGrid;				// Grid of X,Z
	Graph::Scene 				mScene;

public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2)
	{
	}

	/**
	 * This method is called when a key is pressed.
	 */
	void keyPressEvent(int keyCode, int nativeCode)
	{
		switch (keyCode)
		{
			// Close the application if the back key, key 0,
			// or the left "softkey" is pressed (the left
			// softkey is usually located at the top left of
			// the keypad of phones that have a keypad, and
			// is often used as a back navigation key).
			case MAK_BACK:
			case MAK_0:
			case MAK_SOFTLEFT:
				// Exit the application.
				maExit(0);
				break;
		}
	}

	int initShaderLines()
	{
		// set up all locations for each variable in the shaders..
		mScene.getAxisMgr().init();
		return TRUE;
	}

	int initShaderBars()
	{
		mScene.getBarMgr().init();
		return TRUE;
	}

	int initGL()
	{
		// Set up common gl options

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Enable back face culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		// set up clear color
		glClearColor(0.0f, 0.0f, 0.0f, 2.0f);		// why alpha 2.0f ?

		// create a braph with grid times grid
		initShaderBars();
		initShaderLines();
		return TRUE;
	}

	int mStartTime;

	void init()
	{
		lprintfln("init2");
		mWidth 	= EXTENT_X(maGetScrSize());
		mHeight = EXTENT_Y(maGetScrSize());
		mGrid 	= 30;	// 70 ish as most.
		lprintfln("mGrid: %i", mGrid);
		mScene.create(mGrid,mGrid);
		mScene.setWidth(mWidth);
		mScene.setHeight(mHeight);

		if(!initGL())
			maPanic(1, "Failed to initialize!");

		mStartTime = maGetMilliSecondCount();
	}

	void drawBars(float tick)
	{
		mScene.getBarMgr().draw();

	}

	void drawAxis(float tick)
	{
		mScene.getAxisMgr().draw();
	}

	void drawText(float tick)
	{

	}

	void draw()
	{
		float tick = (maGetMilliSecondCount() - mStartTime) * 0.001f;
		mScene.setTick(tick);
		// Set the viewport
		glViewport(0, 0, mWidth, mHeight);
		checkGLError("glViewport");

		// Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGLError("glClear");

		// Create a rotation matrix.
		glm::mat4 m = glm::rotate(20.0f*tick,0.0f,1.0f,0.0f);
		mScene.setWorldMat( m );

		drawBars(tick);
		drawAxis(tick);
		glm::mat4 m2 = glm::rotate(180.0f+20.0f*tick,0.0f,1.0f,0.0f);
		mScene.setWorldMat( m2 );
		drawAxis(tick);

		mScene.setWorldMat( m );
		drawText(tick);
	}
};

/**
 * Entry point of the program. The MAMain function
 * needs to be declared as extern "C".
 */
extern "C" int MAMain() {
	Moblet::run(new MyGLMoblet());
	return 0;
}
