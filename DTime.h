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

#ifndef DTIME_H_
#define DTIME_H_

#include <maapi.h>

/**
 * \brief Time class,
 * generic class that handles time from x and measuring elapsed time generic usage.
 */

class Time
{
	unsigned int mStart;
	unsigned int mElapsed;
public:
	Time() {start();}
	virtual ~Time() {}

	void start() {mStart = maGetMilliSecondCount();}
	float update() {return mElapsed = maGetMilliSecondCount() - mStart;}
	unsigned int getElapsed() {return mElapsed;}
	unsigned int getStart() {return mStart;}
};

/**
 *	\brief Delta time class
 *	for more specific usage of fps time elapsed time since last frame.
 */

class DTime : public Time
{
public:
	DTime() : mDesiredFps(50.0f),mFps(0.0f),mDeltaMs(0.0f),mDelta(1.0f){start();}
	virtual ~DTime() {}

	void 	tick();					// update time used as a tick called once every frame
	float 	getDeltaTime() 				{return mDeltaMs;}	//get elapsed time since last tick (in our case frame)
	void 	setDesiredFps(float fps)	{mDesiredFps = fps;}
	float 	currentFps()				{return mFps;}
	float 	getDesiredFps()				{return mDesiredFps;}

protected:
	float	mDesiredFps;	// Desired fps
	float	mFps;			// Current fps.
	float	mDeltaMs;		// Float delta time in ms elapsed since last frame
	float	mDelta;			// delta in frames

};


#endif /* DTIME_H_ */
