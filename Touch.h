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

#ifndef TOUCH_H_
#define TOUCH_H_

#include <MAUtil/Environment.h>
#include <glm/glm.hpp>
#include <hash_map>

namespace MoGraph
{
	/**
	 * \brief TouchState event
	 */
	enum TouchState{ Idle, Pressed, Moving, Released };

	/**
	 * \brief Touch is a sturct that contains all info around a touch event.
	 */
	struct Touch
	{
		Touch() : mPos(glm::vec2(0.0f,0.0f)),mOldPos(glm::vec2(0.0f,0.0f)), mId(0), mState(Idle) {}
		Touch(MAPoint2d &p, int id, TouchState state) : mPos(glm::vec2((float)p.x,(float)p.y)),mOldPos(glm::vec2((float)p.x,(float)p.y)), mId(id), mState(state) {}


		glm::vec2 	mPos;			// Position of a touch
		glm::vec2	mOldPos;		// previous position of a touch
		int			mId;			// touch index
		TouchState 	mState;			// state if event
	};

	typedef std::pair<int, Touch> TouchPair;

	/**
	 * \brief TouchInput class used for handling the swipes for rotation and double swipe for scaling
	 */
	class TouchInput : public MAUtil::PointerListener
	{
	protected:
		int				mTouchActive;
		std::hash_map<int, Touch> mTouch;	// table of touch
		glm::vec2 		mRotSpeed;			// calculated rotation speed
		glm::vec2 		mRotPos;			// calculated rotation position
		glm::vec2 		mScaleSpeed[2];		// Swipe scale (req two touches)
		float 			mScalePos;			// current position of scale
		float 			mScaleOldPos;		// previous frame position of scale
		float 			mDelta;				// temp diff value for ui swipe
		int 			mWidth;
		int				mHeight;			// Screen resolution in ABS form e.g. 640,480

		glm::vec2 getSpeed(Touch &t, glm::vec2 &speed);

	public:
		TouchInput();
		virtual ~TouchInput(){}
		virtual void init(int mWidth, int mHeight);
		virtual void update();
		virtual float getScale() {return mDelta;}
		virtual glm::vec2 getAngularOrientation() {return mRotPos;}

		// optional multi touch is in use here
		virtual void multitouchPressEvent(MAPoint2d p, int touchId);
		virtual void multitouchMoveEvent(MAPoint2d p, int touchId);
		virtual void multitouchReleaseEvent(MAPoint2d p, int touchId);
		// optional input handler pointers not in use
		virtual void pointerPressEvent(MAPoint2d p) {};
		virtual void pointerMoveEvent(MAPoint2d p) {};
		virtual void pointerReleaseEvent(MAPoint2d p) {};
	};

}

#endif /* TOUCH_H_ */
