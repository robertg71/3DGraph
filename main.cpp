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
#include "RenderText.h"
#include "MAHeaders.h"


using namespace MAUtil;



bool TestHashTable()
{
	bool failed = false;
	std::hash_map<int,std::string> table;
	std::hash_map<int,std::string>::iterator it;
	std::string test = "taste this ";
	lprintfln("Test %s",test.c_str());
	char val[2] = "a";
	for(int i=0; i<1000; i++)
	{
		if (i%100==0)
		{
			val[0]++;
			test = std::string(val);
		}
		else
			test+=std::string(val);

		table[i] = test;
	}

	lprintfln("Testing Hash_map iterator (%d)",table.size());
	lprintfln("Testing Hash_map iterator (%d)",table.size());
	lprintfln("Testing Hash_map iterator (%d)",table.size());
	int i=0;
	for(it = table.begin();it!=table.end();it++,i++)
	{
		lprintfln("%d. table[%d] = \'%s\' it->second=\'%s\'\n",i,i,table[i].c_str(),it->second.c_str());
	}
	if (i == 0)
	{
		lprintfln("Hash map iterator failed expected %d entries got zero\n",table.size());
		failed = true;
	}
	else
		lprintfln("Hash map iterator OK\n");

	lprintfln("Testing hash_map[] array lookup\n");
	for(i=0; i<table.size(); i++)
	{
		lprintfln("%d. table[%d] = \'%s\' \n",i,i,table.find(i)->second.c_str());
	}


	if (i == 0)
	{
		lprintfln("Hash map iterator failed expected %d entries got zero\n",table.size());
		failed = true;
	}
	else
		lprintfln("Hash map look up OK\n");

	return failed;
}








class MyGLMoblet: public GLMoblet
{
private:
	// Bars location parameters
	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	int  			mGrid;				// Grid of X,Z
	Graph::Scene 	mScene;
	BMFont			mFont;
	Graph::RenderText		mRenderText;

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

	int initShaderText()
	{
		//mScene.getTextMgr().init();
		return TRUE;
	}

	int initGL()
	{
		// Set up common gl options
		glViewport(0, 0, mWidth, mHeight);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Enable back face culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		// set up clear color
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);		// why alpha 2.0f ?

		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);
		mFont.Init(R_BOX_FNT, fontTexArray);
		lprintfln("Init RenderText w=%d h=%d\n",mWidth,mHeight);
		mRenderText.Init(mWidth,mHeight,&mFont);

		// create a braph with grid times grid
		initShaderBars();
		initShaderLines();
		initShaderText();



		return TRUE;
	}

	int mStartTime;

	void init()
	{
		// TESTING HASHTABLE
/*		if (TestHashTable() == true)
			return;
*/


		lprintfln("init2");
		mWidth 	= EXTENT_X(maGetScrSize());
		mHeight = EXTENT_Y(maGetScrSize());
		mGrid 	= 10;	// 70 ish as most.
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
//		mScene.getTextMgr().draw();
		glm::vec4 rgba(1.0f,1.0f,1.0f,1.0f);
		std::string str = "Robert Hajduk";
		lprintfln("Render text \"%s\"\n",str.c_str());
		mRenderText.DrawText(str.c_str(),0.0f,0.0f, rgba, mScene);
	}

	void draw()
	{
		static int cnt = 0;
		cnt++;
		glViewport(0, 0, mWidth, mHeight);
			checkGLError("glViewport");
		lprintfln("%d. draw()::glViewport w=%d h=%d\n",cnt,mWidth,mHeight);

		float tick = (maGetMilliSecondCount() - mStartTime) * 0.001f;
		mScene.setTick(tick);
		// Set the viewport
		lprintfln("draw()::glViewport w=%d h=%d\n",mWidth,mHeight);


		// Clear the color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		checkGLError("glClear");

		lprintfln("HEJ! 1\n");

		glm::vec3 axis(0.1f,0.5f,1.0f);
		// Create a rotation matrix.
		glm::mat4 m = glm::rotate(20.0f*tick,axis);

		lprintfln("HEJ! 2\n");

		mScene.setWorldMat( m );

		lprintfln("HEJ! 3\n");

	//	drawBars(tick);
		drawAxis(tick);

		lprintfln("HEJ! 4\n");
/*
		glm::mat4 m2 = glm::rotate(180.0f+20.0f*tick,axis);	// make an other rot matrix rotated 180 deg. for Axis needs to be drawn twice.. if all items should be clamped by a grid
		mScene.setWorldMat( m2 );

		lprintfln("HEJ! 5\n");

		drawAxis(tick);

		lprintfln("HEJ! 6\n");
*/
		mScene.setWorldMat( m );	// set up ordinary world matrix for the text.
		drawText(tick);

		lprintfln("HEJ! 7\n");

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
