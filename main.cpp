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

class MyGLMoblet: public GLMoblet
{
private:
	// Bars location parameters
	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	MoGraph::Graph	mGraph;
	BMFont			mFont;

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

	void init()
	{
		lprintfln("init2");
		mWidth 	= EXTENT_X(maGetScrSize());
		mHeight = EXTENT_Y(maGetScrSize());
		int grid 	= 30;	// 70 ish as most.
		lprintfln("mGrid: %i", grid);

		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);
		mFont.Init(R_BOX_FNT, fontTexArray);
		lprintfln("Init RenderText w=%d h=%d\n",mWidth,mHeight);

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);
		if (!mGraph.init(grid,grid,gridLines,gridStepY,true,&mFont,mWidth,mHeight))
			maPanic(1,"Failed to initiate Graph");

		mGraph.setBKColor(bkcolor);
	}

	void draw()
	{
		mGraph.draw();
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
