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


class MyGLMoblet: public GLMoblet
{
private:
	// Bars location parameters
	int 			mWidth;
	int				mHeight;			// Screen resolution in ABS form e.g. 640,480
	MoGraph::IGraph	*mGraph;			// interface/Base class to MoGraph
	IFont			*mFont;				// interface/Base class to Font
	std::hash_map<int, Touch> mTouch;	// table of touch
	int				mTouchActive;
	glm::vec2 mRotSpeed;
	glm::vec2 mRotPos;
	glm::vec2 mScaleSpeed[2];
	glm::vec4 *mColors;
	float *mTables;
	float mScalePos;
	float mScaleOldPos;
	float mDelta;
	glm::mat4 mScaleMatrix;

public:
	MyGLMoblet() :
		GLMoblet(GLMoblet::GL2) , mGraph(0), mFont(0), mTouchActive(0), mColors(0), mTables(0), mScaleMatrix(1.0f)
	{
		mScaleSpeed[0] = glm::vec2(0.0f,0.0f);
		mScaleSpeed[1] = glm::vec2(0.0f,0.0f);
		mScaleOldPos = 1.0f;
		mScalePos = 1.0f;
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

		float gridStepY = 0.5f;
		int gridLines 	= 5;
		glm::vec4 bkcolor(0.0f, 0.0f, 0.0f, 1.0f);

		// Prepare for text



		if (!mGraph->init(grid,grid,gridLines,gridStepY,true,mFont,mWidth,mHeight))	// initiate Graph with parameters as GridX, GridY, amount of gridLines in Y, stepping for each grid line in Y, fit to screen, Font class, screen width and screen height
			maPanic(1,"Failed to initiate Graph");

		mGraph->setBKColor(bkcolor);				// additional set background color
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
		int cnt=0;
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
//				mScaleOldPos 	= mScalePos;
				mScalePos 		= delta;
			}
			else
			{
				mScalePos = mScaleOldPos = delta;
				cnt = 1;
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
		scene.setScaleMat( mScaleMatrix );
		scene.updateMatrix();		// need to update the PVW Matrix, Projection * View * World.
		mScaleOldPos = mScalePos;

	}

	void draw()
	{
		updateMatrices();
		MoGraph::Scene &scene 	= mGraph->getScene();		// get scene information
		int iGridZ 				= scene.getGridZ();					// need to be able to read the grid size
		int iGridX 				= scene.getGridX();
		int k 					= 0;
		float tick 				= scene.getTick();					// Get tick time

		if (mTables == 0)
			mTables = new float[iGridZ*iGridX];		// allocate an array for set data to the Bars.
		if (mColors == 0)
			mColors = new glm::vec4[iGridZ*iGridX];

		for(int j=0; j<iGridZ; j++)						// Build the data
		{
			// if grid is even then extra add would be required
			k += 1-(iGridX&1);
			for(int i=0; i<iGridX; i++)
			{
				mTables[j*iGridX+i] 	= 1.1f+1.0f*(sin(j*0.3f+	1.3f*tick)+cos(i*0.3f+1.3f*tick));
				float c 			= 0.5f+0.5f*(float)(k&1);
				glm::vec4 col(1.0f-c,0.75f,c,1.0f);
				mColors[j*iGridX+i]	= col;
				k++;
			}
		}

		// Update data to graph
		mGraph->setValues(mTables,iGridX*iGridZ);	// set the value array to the Graph to read from
		mGraph->setColors(mColors,iGridX*iGridZ);	// set the color array to the Graph to read from
		mGraph->draw();								// Draw the whole graph system
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
