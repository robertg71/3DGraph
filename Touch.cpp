/*
 * Copyright (C) 2013 MoSync AB

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.
*/

#include "Touch.h"
#include <GLES2/gl2.h>
#include <mavsprintf.h>

namespace MoGraph
{

	TouchInput::TouchInput()
	{
		glm::vec2 clr(0.0f,0.0f);
		mScaleSpeed[0] = clr;
		mScaleSpeed[1] = clr;
		mScaleOldPos = 1.0f;
		mScalePos = 1.0f;
		mRotPos = clr;
		mRotSpeed = clr;
		mDelta = 1.0f;	// must have the same size as the initial set up. 1.0f.
		mWidth = 640;
		mHeight = 480;
	}

	/**
	 * \brief, 	multitouchPressEvent, Optional, This fwd function is called with a coordinate when a pointer is pressed.
	 *
	 * @param p,		touch pointer
	 * @param touchId,	touch id (finger id)
	 */
	void TouchInput::multitouchPressEvent(MAPoint2d p, int touchId)
	{
//		lprintfln("multitouchPressEvent=(%d,%d) id=%d",p.x,p.y,touchId);
		Touch t(p,touchId,Pressed);
		mTouch[touchId] = t;		// creates or overwrites.
		mTouchActive++;
	}

	/**
	 * \brief, multitouchMoveEvent, Optional, This fwd function is called with a coordinate when a pointer is moved.
	 * @param p,		touch pointer
	 * @param touchId,	touch id (finger id)
	 */
	void TouchInput::multitouchMoveEvent(MAPoint2d p, int touchId)
	{
//		lprintfln("multitouchMoveEvent=(%d,%d) id=%d",p.x,p.y,touchId);
		Touch &t = mTouch[touchId];
		t.mOldPos = t.mPos;
		t.mPos = glm::vec2(p.x,p.y);
		t.mId = touchId;
		t.mState = Moving;
	}

	/**
	 * \brief, multitouchReleaseEvent, Optional, This fwd function is called with a coordinate when a pointer is released.
	 * @param p,		touch pointer
	 * @param touchId,	touch id (finger id)
	 */
	void TouchInput::multitouchReleaseEvent(MAPoint2d p, int touchId)
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

	glm::vec2 TouchInput::getSpeed(Touch &t, glm::vec2 &speed)
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

	void TouchInput::init(int width,int height)
	{
		mWidth 		= width;
		mHeight 	= height;
	}

	void TouchInput::update()
	{
//		int cnt=0;
//		MoGraph::Scene &scene = mGraph->getScene();
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

			if (mTouch[0].mState == MoGraph::Moving && mTouch[1].mState == MoGraph::Moving)
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

	//		scene.updateCamera( mDelta );
		}
		else
		{
			mScalePos = mScaleOldPos = 1.0f;

			mRotSpeed = getSpeed(mTouch[0],mRotSpeed);
			mRotPos += mRotSpeed;
			// Create a rotation matrix.
		}

		mScaleOldPos = mScalePos;

	/*
		glm::vec3 axisY(0.0,1.0f,0.0f);
		glm::mat4 mY = glm::rotate(mRotPos.x*30.0f,axisY);
		glm::vec3 axisX(1.0,0.0f,0.0f);
		glm::mat4 mX = glm::rotate(mRotPos.y*30.0f,axisX);

		glm::mat4 m = mY*mX;
		scene.setWorldMat( m );
		scene.updateMatrix();		// need to update the PVW Matrix, Projection * View * World.
*/
	}

}
