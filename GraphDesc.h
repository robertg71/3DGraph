/*
 * GraphDesc.h
 *
 *  Created on: Mar 18, 2013
 *      Author: CodeArt
 */

#ifndef GRAPHDESC_H_
#define GRAPHDESC_H_

class IFont;

namespace MoGraph
{
	/**
	 * \brief GraphDesc. Graph Description used for its input data to set up the graph
	 */
	struct GraphDesc
	{
		/**
		 * \brief GraphDesc, Constructor.
		 */
		GraphDesc() :
			scrWidth(0),
			scrHeight(0),
			gridX(10),
			gridZ(10),
			gridYLines(5),
			gridStepYLines(5.0f),
			gridStepValue(0.5f),
			bFitScreen(true),
			bNegGridLines(false),
			bUseGridValue(true),
			font(0)
		{
		}

		int 	scrWidth;			// Screen height
		int 	scrHeight;			// Screen width
		int 	gridX;				// Grid size of Z for graph
		int 	gridZ;				// Grid size on X for graph
		int 	gridYLines;			// Amount of grid lines
		float 	gridStepYLines;		// The steps in height for each line
		float	gridStepValue;		// Grid step value (step=10 => 10,20,30,40,50)
		bool 	bFitScreen;			// Fit to screen flag
		bool	bNegGridLines;		// Enable grid lines on the negatice axis in Y (mirrored)
		bool	bUseGridValue;		// Enable values for grid lines
		IFont* 	font;				// ptr to the Font system
	};
}

#endif /* GRAPHDESC_H_ */
