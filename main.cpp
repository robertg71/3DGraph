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
#include <stdlib.h>
#include "MAHeaders.h"
#include "MoGraph.h"

using namespace MAUtil;

#define PI 3.14159265359

class MyGLMoblet: public GLMoblet //, public OrientationListener
{
private:

	// ================== Members ==================

	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	MoGraph::IGraph	*mGraph;			// interface/Base class to MoGraph
	IFont			*mFont;				// interface/Base class to Font
	glm::vec4 		*mColors;			// array pointing towards a color table
	float 			*mTables;			// array pointing towards a float table
	DTime 			mDTime;				// delta time between each tick! (update) used for fps timer
	Time  			mTime;				// elapsed time since creation of this class,
	RenderText 		mText;				// generic render text class. (openGL)
	float			mOmega;				// used for sin and cos


public:

	// ================== Constructor ==================

	/**
	 * Constructor for the main app class
	 */
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mGraph(0), mFont(0), mColors(0), mTables(0), mOmega(0.0)
	{
	//	addOrientationListener(this);
	}

	// ================== Destructor ==================

	/**
	 * Destructor
	 */
	virtual ~MyGLMoblet()
	{
	//	removeOrientationListener(this);
		delete [] mTables;
		delete [] mColors;
		delete mGraph;					// Delete Graph
		delete mFont;					// Deleta Font
	}

	// ================== Callback ==================

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
	* Called after the screen has finished rotating.
	* \param 'screenOrientation' One of the
	* \link #MA_SCREEN_ORIENTATION_PORTRAIT MA_SCREEN_ORIENTATION \endlink codes.
	*/
/*	virtual void orientationChanged(int orientation)
	{
		lprintfln("Orienttion Changed = %d",orientation);
	}*/
	/**
	* Send by current screen just before it begins rotating.
	* Note: available only on iOS platform.
	*/
/*	virtual void orientationWillChange()
	{
		lprintfln("Orienttion will Changed");
	}*/
	/**
	 * init call backed from GLMoblet
	 */
	void init()
	{
		/*
		 * Graph object needs to be allocated and then initiated,
		 * Font is a separate system but is required in the graph for rendering text in 3D
		 * RenterText is an independent text renderer flat on screen. but very same class is being used in Graph as well
		 * can handle both orthogonal projection see drawText and drawText3D
		 */

		mWidth 		= (int)(EXTENT_X(maGetScrSize()));
		mHeight 	= (int)(EXTENT_Y(maGetScrSize()));
		mGraph 		= new MoGraph::Graph();			// Create MoGraph::Graph class
		mFont 		= new BMFont();					// Create Font class

		// Initiate the RenderText system that will be used in Graph
		std::vector<MAHandle> fontTexArray;
		fontTexArray.push_back(R_BOX_TEXTURE);

		mFont->Init(R_BOX_FNT, fontTexArray);		// Initiate font where to get its resources (.fnt) file generated from BMFont and the bitmap texture that contains the aphabet
		mText.init(mWidth,mHeight,mFont);			// initiate the text system by setting a Font & Screen dimensions

		mDTime.setDesiredFps(50.0f);				// set up the DTime used for calculating FPS
		setPreferredFramesPerSecond(50);			// set preferred fps for the Moblet

		// initiate Graph by setting up a grid sz in X & Z ,
		// also grid in Y with grid step,
		// additional info like fit to screen, screen resolutions.

		MoGraph::GraphDesc desc;
		desc.scrWidth 		= mWidth;
		desc.scrHeight 		= mHeight;
		desc.gridX 			= 30;
		desc.gridZ 			= 30;
		desc.gridYLines 	= 10;
		desc.gridStepYLines = 0.5f;
		desc.gridStepValue 	= 0.5f;
		desc.gridDecimals 	= 1;
		desc.gridOffsetStartLine = -1;			// requires OFFSET_GRIDS to be set.
		desc.gridOffsetStartValue = -2.0f;
		desc.bFitScreen 	= true;
		desc.flagGridLines 	= MoGraph::DEFAULT_GRIDS; // /* MoGraph::OFFSET_GRIDS; //*/ MoGraph::MIRRORED_GRIDS;
		desc.bUseGridValue 	= true;
		desc.font 			= mFont;

		if (!mGraph->init(&desc))
			maPanic(1,"Failed to initiate Graph");

		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);
		mGraph->setBGColor(bkcolor);				// additional set background color

		// TEXT MANIPULATION IN GRAPH
		// Text strings in the graph has a default setup.
		// everything is built upon the Text structs that are pushed to an
		// std::vector<Text>   Text array
		// We can change the existing setup by changing the strings...
		std::vector<MoGraph::Text> &textArray = mGraph->getScene().getTextMgr().getTextArray();

		// and we can also push new ones to the table for more text in the graph...
		// you can add as many as you please... fps issue in the end.

		float scale = mGraph->getScene().getGridX()/500.0f;
		// create additional example text

		MoGraph::Text text;
		text.mColor 	= glm::vec4(1.0f,0.0f,0.0f,1.0f);
		text.mPos 		= glm::vec3(0.0f,7.0f,0.0f);
		text.mRotate 	= glm::vec3(0.0f,0.0f,0.0f);		// Rotation in degrees
		text.mScale 	= glm::vec2(scale,scale);
		text.mTextFlagX = MoGraph::Text::CENTER_X;
		text.mText 		= "MoGraph DEMO Beta";
		textArray.push_back(text);

		// just add more. here

		// colors are static so we only need to build them once.
		const int iGridZ 		= desc.gridZ;		// need to be able to read the grid size
		const int iGridX 		= desc.gridX;
		const int sz			= iGridX * iGridZ;


		if (mTables == 0)					// check if array already is allocated
			mTables = new float[sz];		// allocate an array for set data to the Bars.
		if (mColors == 0)					// check if array already is allocated
			mColors = new glm::vec4[sz];	// allocate an array for color table

		for(int j=0; j<iGridZ; j++)			// Build the data arrays for colors and for tables.
		{
			for(int i=0; i<iGridX; i++)
			{
				const int id 	= j*iGridX+i;
				mColors[id]		= glm::vec4((float)i/(float)iGridX,0.0f,(float)j/(float)iGridZ,1.0f);			// set color gradients
			}
		}
	}

	/**
	 *  \brief Draw, Call back from the GLMoblet.
	 */
	void draw()
	{
		mDTime.tick();									// update delta time ticker. our fps timer (resets for every tick call)
		MoGraph::Scene &scene 	= mGraph->getScene();	// get scene information
		const float width 		= static_cast<float>(scene.getWidth());
	//	const float height 		= static_cast<float>(scene.getHeight());
		const int iGridZ 		= scene.getGridZ();		// need to be able to read the grid size
		const int iGridX 		= scene.getGridX();
		const int sz			= iGridX * iGridZ;

		float dt = static_cast<float>(mDTime.getElapsed())*0.001f;	// calculate a delta time factor for omega
		mOmega += dt;						// accumulate the omega used for sin/cos func

		if (mOmega > (PI*2.0f))				// for high precision make sure omega 0..2*PI
			mOmega -= PI*2.0f;				// wrapps also faster not to use large values.

		for(int j=0; j<iGridZ; j++)			// Build the data arrays for colors and for tables.
		{
			const float jcos = cos(j*0.3f+mOmega);	// calculate the Depth Wave with cos
			for(int i=0; i<iGridX; i++)
			{
				const int id 	= j*iGridX+i;
				mTables[id] 	= 2.0f+1.0f*(sin(i*0.3f+mOmega) + jcos); // generate a sine wave and add depth wave
			}
		}

		mGraph->setValues(mTables,sz);	// set the value array to the Graph to read from
		mGraph->setColors(mColors,sz);	// set the color array to the Graph to read from

		// Update data to graph
		mGraph->draw();						// Draw the whole graph system

		// DRAW ADDITIONAL TEXT ON SCREEN (Orthogonal projections)
		glm::vec4 col(1.0f,1.0f,1.0f,1.0f);	// White color
		glm::vec3 pos(0.0f,0.0f,10.0f);		// set screen position upper left corner 0,0.. note need z depth value for order.

		float sy = 0.6f;
		sy = (width*sy) * 1.0f/320.0f;		// use 320 as a reference size, and scale towards that and the font will keep same size even on different resolutions
		mText.setScale(sy,sy);

		char buf[64];						// create text string
		sprintf ( buf, "FPS=%.2f ms=%d",mDTime.currentFps(),mDTime.getElapsed());
		mText.drawText(buf,pos,col);		// call drawText
	}

};

// ================== Start up ==================

/**
 * Entry point of the program. The MAMain function
 * needs to be declared as extern "C".
 */
extern "C" int MAMain() {
	Moblet::run(new MyGLMoblet());
	return 0;
}
