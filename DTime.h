/*
 * DTime.h
 *
 *  Created on: Feb 28, 2013
 *      Author: CodeArt
 */

#ifndef DTIME_H_
#define DTIME_H_

#include <maapi.h>

/*
 * Time class, generic class that handles time from x and measuring elapsed time generic usage.
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

/*
 *	Delta time class more specific usage for fps time elapsed time since last frame.
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
	float	mDeltaMs;			// Float delta time in ms elapsed since last frame
	float	mDelta;			// delta in frames

};


#endif /* DTIME_H_ */
