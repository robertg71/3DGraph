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
#include "GFont.h"
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
	MoGraph::IGraph	*mGraph;
	IFont			*mFont;

public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mGraph(0), mFont(0)
	{
	}

	~MyGLMoblet()
	{
		delete mGraph;
		delete mFont;
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
		mGraph 	= new MoGraph::Graph();
		mFont 	= new BMFont();

		lprintfln("init2");
		mWidth 	= EXTENT_X(maGetScrSize());
		mHeight = EXTENT_Y(maGetScrSize());
		int grid 	= 33;	// 70 ish as most.
		lprintfln("mGrid: %i", grid);

		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);
		mFont->Init(R_BOX_FNT, fontTexArray);
		lprintfln("Init RenderText w=%d h=%d\n",mWidth,mHeight);

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);
		if (!mGraph->init(grid,grid,gridLines,gridStepY,true,mFont,mWidth,mHeight))
			maPanic(1,"Failed to initiate Graph");

		mGraph->setBKColor(bkcolor);
	}

	void draw()
	{
		MoGraph::Scene &scene = mGraph->getScene();
		int iGridZ = scene.getGridZ();	// need to be able to read the grid size
		int iGridX = scene.getGridX();
		int k = 0;

		float tick = scene.getTick();
		float *values = new float[iGridZ*iGridX];
		glm::vec4 *colors = new glm::vec4[iGridZ*iGridX];
		for(int j=0; j<iGridZ; j++)
		{
			// if grid is even then extra add would be required
			k += 1-(iGridX&1);
			for(int i=0; i<iGridX; i++)
			{
				values[j*iGridX+i] = 1.1f+1.0f*(sin(j*0.3f+	1.3f*tick)+cos(i*0.3f+1.3f*tick));
				float c = 0.5f+0.5f*(float)(k&1);
				glm::vec4 col(1.0f-c,0.75f,c,1.0f);
				colors[j*iGridX+i]	= col;
				k++;
			}
		}

		mGraph->setValues(values,iGridX*iGridZ);
		mGraph->setColors(colors,iGridX*iGridZ);
		mGraph->draw();

		delete [] values;
		delete [] colors;
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
