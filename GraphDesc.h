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
	 * \note extDesc ptr has been reserved for the future in case the interface grows.
	 * it is set to zero and must not be used until provided with further info.
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
			gridDecimals(1),
			gridYLines(5),
			gridStepYLines(5.0f),
			gridStepValue(0.5f),
			font(0),
			bFitScreen(true),
			bNegGridLines(false),
			bUseGridValue(true),
			bUseOffsetStartLine(false),
			extDesc(0)
		{
		}

		int 	scrWidth;			// Screen height
		int 	scrHeight;			// Screen width
		int 	gridX;				// Grid size of Z for graph
		int 	gridZ;				// Grid size on X for graph
		int		gridDecimals;		// set amount of decimals to be displayed as height data. 1 = 0.0 , 2 = 0.00 , 3 = 0.000 etc
		int 	gridYLines;			// Amount of grid lines
		float 	gridStepYLines;		// The steps in height for each line
		float	gridStepValue;		// Grid step value (step=10 => 10,20,30,40,50)
		float	gridOffsetStartValue;	// Lowest bound of Value to start from
		float	gridOffsetStartLine;	// Lowest bound of line to start grid from
		IFont* 	font;				// ptr to the Font system
		signed  bFitScreen:1, bNegGridLines:1, bUseGridValue:1, bUseOffsetStartLine:1;
		void *	extDesc;			// extended parameter struct, reserved for future reference.
	};
}

#endif /* GRAPHDESC_H_ */
