
#include "DTime.h"

void DTime::tick()
{
	float currentDelta = update();				// get elapsed time since last frame
	start();									// reset counter and start counting from here! affects mStart.
	mFps 			= 1000.0f/currentDelta;		// calculate current frame rate.
	mDelta 			= mDesiredFps/mFps;			// calculate scale from desired fps. that will be used as delta / frame
}
