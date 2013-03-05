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
#include "DTime.h"
#include <stdlib.h>


using namespace MAUtil;

enum TouchState{ Idle, Pressed, Moving, Released };

struct Touch
{
	Touch() : mPos(glm::vec2(0.0f,0.0f)),mOldPos(glm::vec2(0.0f,0.0f)), mId(0), mState(Idle) {}
	Touch(MAPoint2d &p, int id, TouchState state) : mPos(glm::vec2((float)p.x,(float)p.y)),mOldPos(glm::vec2((float)p.x,(float)p.y)), mId(id), mState(state) {}


	glm::vec2 	mPos;
	glm::vec2	mOldPos;
	int			mId;
	TouchState 	mState;
};

typedef std::pair<int, Touch> TouchPair;

// TODO MOVE UI INSIDE MOGRAPH CLASSES.
class MyGLMoblet: public GLMoblet
{
private:
	// Bars location parameters
	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	int				mTouchActive;
	MoGraph::IGraph	*mGraph;			// interface/Base class to MoGraph
	IFont			*mFont;				// interface/Base class to Font
	std::hash_map<int, Touch> mTouch;	// table of touch
	glm::vec2 		mRotSpeed;			// calculated rotation speed
	glm::vec2 		mRotPos;			// calculated rotation position
	glm::vec2 		mScaleSpeed[2];		// Swipe scale (req two touches)
	glm::vec4 		*mColors;			// array pointing towards a color table
	float 			*mTables;			// array pointing towards a float table
	float 			mScalePos;			// current position of scale
	float 			mScaleOldPos;		// previous frame position of scale
	float 			mDelta;				// temp diff value for ui swipe
	DTime 			mDTime;				// delta time between each tick! (update)
	Time  			mTime;				// elapsed time since creation of this class,
	RenderText 		mText;


public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mTouchActive(0), mGraph(0), mFont(0), mColors(0), mTables(0)
	{
		glm::vec2 clr(0.0f,0.0f);
		mScaleSpeed[0] = clr;
		mScaleSpeed[1] = clr;
		mScaleOldPos = 1.0f;
		mScalePos = 1.0f;
		mRotPos = clr;
		mRotSpeed = clr;
		mDelta = 1.0f;	// must have the same size as the initial set up. 1.0f.
	}

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
//		lprintfln("multitouchPressEvent=(%d,%d) id=%d",p.x,p.y,touchId);
		Touch t(p,touchId,Pressed);
		mTouch[touchId] = t;		// creates or overwrites.
		mTouchActive++;
	}

	/**
	*  This function is called with a coordinate when a pointer is moved.
	*/
	void multitouchMoveEvent(MAPoint2d p, int touchId)
	{
//		lprintfln("multitouchMoveEvent=(%d,%d) id=%d",p.x,p.y,touchId);
		Touch &t = mTouch[touchId];
		t.mOldPos = t.mPos;
		t.mPos = glm::vec2(p.x,p.y);
		t.mId = touchId;
		t.mState = Moving;
	}

	/**
	*  This function is called with a coordinate when a pointer is released.
	*/
	void multitouchReleaseEvent(MAPoint2d p, int touchId)
	{
//		lprintfln("multitouchReleaseEvent=(%d,%d) id=%d",p.x,p.y,touchId);
//		mTouch.erase(touchId);							// could keep the info. with a different state.
		Touch &t = mTouch[touchId];
		t.mOldPos = t.mPos;
		t.mPos = glm::vec2(p.x,p.y);
		t.mId = touchId;
		t.mState = Released;
		mTouchActive--;
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

		mText.init(mWidth,mHeight,mFont);

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);


		mDTime.setDesiredFps(50.0f);
		setPreferredFramesPerSecond(50);



		if (!mGraph->init(grid,grid,gridLines,gridStepY,true,mFont,mWidth,mHeight))	// initiate Graph with parameters as GridX, GridY, amount of gridLines in Y, stepping for each grid line in Y, fit to screen, Font class, screen width and screen height
			maPanic(1,"Failed to initiate Graph");

		mGraph->setBKColor(bkcolor);				// additional set background color

		// Text strings in the graph has a default setup. everything is built upon the Text structs that are pushed to an std::vector<Text>   Text array
		// We can change the existing setup by changeing the strings...
		std::vector<MoGraph::Text> &textArray = mGraph->getScene().getTextMgr().getTextArray();

		std::vector<std::string> strArray;

		strArray.push_back("Sinus table ala Carte");
		strArray.push_back("Y-Axel");
		strArray.push_back("X-Axel");
		strArray.push_back("Z-Axel");
		for (size_t i = 0; i<textArray.size(); i++)
		{
			textArray[i].mText = strArray[i];
		}

		// and we can also push new ones to the table for more text in the graph... you can add as many as you please... fps issue in the end.
		float scale = mGraph->getScene().getGridX()/500.0f;
		// create additional example text
		MoGraph::Text text;
		text.mColor = glm::vec4(1.0f,0.0f,0.0f,1.0f);
		text.mPos = glm::vec3(0.0f,5.0f,0.0f);
		text.mRotate = glm::vec3(0.0f,0.0f,0.0f);
		text.mScale = glm::vec2(scale,scale);
		text.mTextFlag = MoGraph::Text::CENTER;
		text.mText = "MoGraph DEMO Beta";
		textArray.push_back(text);

		// just add more. here
	}


	glm::vec2 getSpeed(Touch &t, glm::vec2 &speed)
	{

		glm::vec2 p,op;
		// Single touch.
		if (mTouchActive == 0)
		{
			p.x = 0.0f;
			p.y = 0.0f;
			speed *= 0.98f;
		}
		else if (mTouchActive == 1)
		{
			glm::vec2 scale((float)mWidth,(float)mHeight);

			p 	= mTouch[0].mPos/scale;
			op 	= mTouch[0].mOldPos/scale;
			speed = p - op;
		}

		if (speed.x > 1.0f)
			speed.x = 1.0f;

		if (speed.x < -1.0f)
			speed.x = -1.0f;

		if (speed.y > 1.0f)
			speed.y = 1.0f;

		if (speed.y < -1.0f)
			speed.y = -1.0f;

		return speed;
	}

	/// TODO need time delta handling here.

	// Draw callback function
	void updateMatrices()
	{
//		int cnt=0;
		MoGraph::Scene &scene = mGraph->getScene();

		if (mTouchActive>1)
		{
			if (mTouchActive > 2)
			{
				lprintfln("Multi x>2 touch not supported yet! = %d",mTouchActive);
				return;
			}

			// double touch scale
			glm::vec2 scale((float)mWidth,(float)mHeight);
			mScaleSpeed[0] = mTouch[0].mPos/scale;//getSpeed(mTouch[0],mScaleSpeed[0]);
			mScaleSpeed[1] = mTouch[1].mPos/scale;//getSpeed(mTouch[1],mScaleSpeed[1]);
			glm::vec2 diff = mScaleSpeed[0] - mScaleSpeed[1];

			float delta = glm::length(diff);//diff.length();

			delta = (delta < 0.0f)? -delta: delta;
			delta = (delta < 0.1f)? 0.1f: delta;
			delta = (delta > 2.0f)? 2.0f: delta;

			if (mTouch[0].mState == Moving && mTouch[1].mState == Moving)
			{
				mScalePos 		= delta;
			}
			else
			{
				mScalePos = mScaleOldPos = delta;
			}
			mDelta -= mScaleOldPos - mScalePos;
			if (mDelta > 2.0f)
				mDelta = 2.0f;
			else if (mDelta < 0.1f)
				mDelta = 0.1f;

	//		lprintfln("%d. Delta = %f = %f - %f",cnt, mDelta,mScaleOldPos,mScalePos);

			scene.updateCamera( mDelta );
		}
		else
		{
			mScalePos = mScaleOldPos = 1.0f;

			mRotSpeed = getSpeed(mTouch[0],mRotSpeed);
			mRotPos += mRotSpeed;
			// Create a rotation matrix.
		}
		glm::vec3 axisY(0.0,1.0f,0.0f);
		glm::mat4 mY = glm::rotate(mRotPos.x*30.0f,axisY);
		glm::vec3 axisX(1.0,0.0f,0.0f);
		glm::mat4 mX = glm::rotate(mRotPos.y*30.0f,axisX);

		glm::mat4 m = mY*mX;
		scene.setWorldMat( m );
		scene.updateMatrix();		// need to update the PVW Matrix, Projection * View * World.
		mScaleOldPos = mScalePos;

	}

	void draw()
	{
		mDTime.tick();
		updateMatrices();
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

	//	glm::mat4 projection 		= glm::perspective(45.0f, static_cast<float>(mHeight/mWidth), 0.001f, 1.0f); 		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

		glm::vec3 pos(100.0f,100.0f,100.0f);
		glm::vec4 col(1.0f,1.0f,1.0f,1.0f);
//		mText.setScale(1.0f/mWidth,1.0f/mHeight);
		mText.setScale(1.0f,1.0f);

		mText.drawText("Testing 123!",pos,col);

		pos.x = 0.0f;
		pos.y = 0.0f;
		pos.z = 10.0f;

		for(int i=0; i<10; i+=1)
		{

			const float scale = 0.6f;
			const float sy = scale*(1.0f+0.5f*sin(tick+(i*0.03f)));
	//		float sx = scale*(1.0f+0.5f*cos(tick+(i*0.03f)));
			mText.setScale(sy,sy);		// note text needs to be written up side down due to orthogonal  matrix 0,0 upper corner.. 640,480 lower right corner.
			pos.x = static_cast<float>(50.0f+50.0f*sy);
			pos.y = static_cast<float>(i*20*sy);
//			pos.z = static_cast<float>(i*20*scale);

			char buf[64];
			sprintf ( buf, "%d FPS=%.2f ms=%d",i,mDTime.currentFps(),mDTime.getElapsed());
			mText.drawText(buf,pos,col);

			// it could be good to have both function paint the text in the same way!
//			glm::mat4 m = glm::ortho(0.0f,static_cast<float>(mWidth),static_cast<float>(mHeight), 0.0f, -200.0f, 1000.0f );
//			m = mGraph->getScene().getPVWMat() * m;
//			mText.drawText3D(buf,pos,col,/*mGraph->getScene().getPVWMat()*/m);
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
