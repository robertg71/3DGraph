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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <stdlib.h>
#include <hash_map>
#include "graph.h"
#include "GFont.h"
#include "Shaders.h"
#include "RenderText.h"
#include "MAHeaders.h"
#include "DTime.h"
#include "Touch.h"
#include "GraphDesc.h"
using namespace MAUtil;


class MyGLMoblet: public GLMoblet
{
private:
	// Bars location parameters
	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	MoGraph::IGraph	*mGraph;			// interface/Base class to MoGraph
	IFont			*mFont;				// interface/Base class to Font
	glm::vec4 		*mColors;			// array pointing towards a color table
	float 			*mTables;			// array pointing towards a float table
	DTime 			mDTime;				// delta time between each tick! (update)
	Time  			mTime;				// elapsed time since creation of this class,
	RenderText 		mText;


public:

	/**
	 * Constructor for the main app class
	 */
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mGraph(0), mFont(0), mColors(0), mTables(0)
	{
	}

	/**
	 * Destructor
	 */
	virtual ~MyGLMoblet()
	{
		delete [] mTables;
		delete [] mColors;
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

	/**
	*  This function is called with a coordinate when a pointer is pressed.
	*/
	void multitouchPressEvent(MAPoint2d p, int touchId)
	{
		mGraph->getTouch().multitouchPressEvent(p,touchId);	// just forward function to the Graphs own touch handler
	}

	/**
	*  This function is called with a coordinate when a pointer is moved.
	*/
	void multitouchMoveEvent(MAPoint2d p, int touchId)
	{
		mGraph->getTouch().multitouchMoveEvent(p,touchId); // just forward function to the Graphs own touch handler
	}

	/**
	*  This function is called with a coordinate when a pointer is released.
	*/
	void multitouchReleaseEvent(MAPoint2d p, int touchId)
	{
		mGraph->getTouch().multitouchReleaseEvent(p,touchId); // just forward function to the Graphs own touch handler
	}

	/**
	 * init call backed from GLMoblet
	 */
	void init()
	{
		/*
		 * *Graph object needs to be allocated and then initiated,
		 * *Font is a separate system but is required in the graph for rendering text in 3D
		 * *RenterText is an independent text renderer flat on screen. but very same class is being used in Graph aswell
		 * can handle both orthogonal projection see drawText and drawText3D
		 */

		lprintfln("Init APP");
		mWidth 		= (int)(1.0f*EXTENT_X(maGetScrSize()));
		mHeight 	= (int)(1.0f*EXTENT_Y(maGetScrSize()));

		mGraph 		= new MoGraph::Graph();			// Create MoGraph::Graph class
		mFont 		= new BMFont();					// Create Font class

		int grid 	= 33;							// set up a desired grid for the graph in X & Z.
		lprintfln("mGrid: %i", grid);

		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);

		mFont->Init(R_BOX_FNT, fontTexArray);		// Initiate font where to get its resources (.fnt) file generated from BMFont and the bitmap texture that contains the aphabet
		lprintfln("Init RenderText w=%d h=%d\n",mWidth,mHeight);

		mText.init(mWidth,mHeight,mFont);			// initiate the text system by setting a Font & Screen dimensions

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);

		mDTime.setDesiredFps(50.0f);				// set up the DTime used for calculating FPS
		setPreferredFramesPerSecond(50);			// set preferred fps for the Moblet

		// initiate Graph by setting up a grid sz in X & Z , also grid in Y with grid step, additional info like fit to screen, screen resolutions.
		MoGraph::GraphDesc desc;
		desc.scrWidth = mWidth;
		desc.scrHeight = mHeight;
		desc.gridX = grid;
		desc.gridZ = grid;
		desc.gridYLines = gridLines;
		desc.gridStepYLines = gridStepY;
		desc.gridStepValue = 0.5f;
		desc.gridDecimals = 0;
		desc.bFitScreen = true;
		desc.bNegGridLines = true;
		desc.bUseGridValue = true;
		desc.font = mFont;

//		if (!mGraph->init(grid,grid,gridLines,gridStepY,true,mFont,mWidth,mHeight))	// initiate Graph with parameters as GridX, GridY, amount of gridLines in Y, stepping for each grid line in Y, fit to screen, Font class, screen width and screen height
		if (!mGraph->init(&desc))	// initiate Graph with parameters as GridX, GridY, amount of gridLines in Y, stepping for each grid line in Y, fit to screen, Font class, screen width and screen height
			maPanic(1,"Failed to initiate Graph");

		mGraph->setBGColor(bkcolor);				// additional set background color


		// TEXT MANIPULATION IN GRAPH

		// Text strings in the graph has a default setup. everything is built upon the Text structs that are pushed to an std::vector<Text>   Text array
		// We can change the existing setup by changeing the strings...
		std::vector<MoGraph::Text> &textArray = mGraph->getScene().getTextMgr().getTextArray();

		std::vector<std::string> strArray;
/*
		strArray.push_back("Sinus table ala Carte");
		strArray.push_back("Y-Axel");
		strArray.push_back("X-Axel");
		strArray.push_back("Z-Axel");
		for (size_t i = 0; i<textArray.size(); i++)
		{
			textArray[i].mText = strArray[i];
		}
*/
		// and we can also push new ones to the table for more text in the graph... you can add as many as you please... fps issue in the end.
		float scale = mGraph->getScene().getGridX()/500.0f;
		// create additional example text
		MoGraph::Text text;
		text.mColor = glm::vec4(1.0f,0.0f,0.0f,1.0f);
		text.mPos = glm::vec3(0.0f,5.0f,0.0f);
		text.mRotate = glm::vec3(0.0f,0.0f,0.0f);		// Rotation in degrees
		text.mScale = glm::vec2(scale,scale);
		text.mTextFlag = MoGraph::Text::CENTER;
		text.mText = "MoGraph DEMO Beta";
		textArray.push_back(text);

		// just add more. here
	}

	/**
	 *  \brief Draw, callebacked from the GLMoblet.
	 */
	void draw()
	{
		mDTime.tick();
		MoGraph::Scene &scene 	= mGraph->getScene();	// get scene information
		const int iGridZ 		= scene.getGridZ();		// need to be able to read the grid size
		const int iGridX 		= scene.getGridX();
		const int sz			= iGridX * iGridZ;
		const float tick 		= static_cast<float>(mTime.update())*0.001f;
		int k 					= 0;

		if (mTables == 0)								// check if array already is allocated
			mTables = new float[sz];			// allocate an array for set data to the Bars.
		if (mColors == 0)								// check if array already is allocated
			mColors = new glm::vec4[sz];		// allocate an array for color table

		for(int j=0; j<iGridZ; j++)						// Build the data
		{
			k += 1-(iGridX&1);	// if grid is even then extra add would be required
			for(int i=0; i<iGridX; i++)
			{
				const int id = j*iGridX+i;
				mTables[id] 	= 1.1f+1.0f*(sin(j*0.3f+	1.3f*tick)+cos(i*0.3f+1.3f*tick));
		//		float c 			= 0.5f+0.5f*(float)(k&1);
		//		glm::vec4 col(1.0f-c,0.75f,c,1.0f);
				glm::vec4 col(1.0f/iGridX*i,0.0f,1.0f/iGridZ*j,1.0f);
				mColors[id]	= col;
				k++;
			}
		}

		glEnable(GL_DEPTH_TEST);

		// Update data to graph
		mGraph->setValues(mTables,sz);	// set the value array to the Graph to read from
		mGraph->setColors(mColors,sz);	// set the color array to the Graph to read from
		mGraph->draw();								// Draw the whole graph system
/*
		glm::vec3 pos(100.0f,100.0f,100.0f);
		glm::vec4 col(1.0f,1.0f,1.0f,1.0f);
		mText.setScale(1.0f,1.0f);

		mText.drawText("Testing 123!",pos,col);
*/
		glm::vec4 col(1.0f,1.0f,1.0f,1.0f);
		glm::vec3 pos;
		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 10.0f;

		for(int i=0; i<1; i+=1)
		{

			const float scale = 0.6f;
			const float sy = scale/**(1.0f+0.5f*sin(tick+(i*0.03f)))*/;
			mText.setScale(sy,sy);		// note text needs to be written up side down due to orthogonal  matrix 0,0 upper corner.. 640,480 lower right corner.
			pos.x = 0.0f; //static_cast<float>(50.0f+50.0f*sy);
			pos.y = 0.0f; //static_cast<float>(i*20*sy);

			char buf[64];
			sprintf ( buf, "%d FPS=%.2f ms=%d",i,mDTime.currentFps(),mDTime.getElapsed());
			mText.drawText(buf,pos,col);
		}
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
