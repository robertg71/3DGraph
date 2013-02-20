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
	MoGraph::IGraph	*mGraph;			// interface/Base class to MoGraph
	IFont			*mFont;				// interface/Base class to Font

public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mGraph(0), mFont(0)
	{
	}

	virtual ~MyGLMoblet()
	{
		delete mGraph;					// Delete Graph
		delete mFont;					// Deleta Font
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
		lprintfln("Init APP");

		mGraph 		= new MoGraph::Graph();			// Create MoGraph::Graph class
		mFont 		= new BMFont();					// Create Font class
		mWidth 		= EXTENT_X(maGetScrSize());
		mHeight 	= EXTENT_Y(maGetScrSize());
		int grid 	= 33;							// set up a desired grid for the graph in X & Z.
		lprintfln("mGrid: %i", grid);

		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);
		mFont->Init(R_BOX_FNT, fontTexArray);		// Initiate font where to get its resources (.fnt) file generated from BMFont and the bitmap texture that contains the aphabet
		lprintfln("Init RenderText w=%d h=%d\n",mWidth,mHeight);

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);
		if (!mGraph->init(grid,grid,gridLines,gridStepY,true,mFont,mWidth,mHeight))	// initiate Graph with parameters as GridX, GridY, amount of gridLines in Y, stepping for each grid line in Y, fit to screen, Font class, screen width and screen height
			maPanic(1,"Failed to initiate Graph");

		mGraph->setBKColor(bkcolor);				// additional set background color
	}

	// Draw callback function
	void draw()
	{
		MoGraph::Scene &scene = mGraph->getScene();		// get scene information
		int iGridZ = scene.getGridZ();					// need to be able to read the grid size
		int iGridX = scene.getGridX();
		int k = 0;

		float tick = scene.getTick();					// Get tick time
		float *values = new float[iGridZ*iGridX];		// allocate an array for set data to the Bars.
		glm::vec4 *colors = new glm::vec4[iGridZ*iGridX];	// allocate an array for the colors for the bar. in not provided just use null and default will be used
		for(int j=0; j<iGridZ; j++)						// Build the data
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

		// Update data to graph
		mGraph->setValues(values,iGridX*iGridZ);	// set the value array to the Graph to read from
		mGraph->setColors(colors,iGridX*iGridZ);	// set the color array to the Graph to read from
		mGraph->draw();								// Draw the whole graph system

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
