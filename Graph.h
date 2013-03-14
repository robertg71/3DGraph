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

#ifndef GRAPH_H_
#define GRAPH_H_
#include "IGraph.h"
#include "GFont.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Shaders.h"
#include "RenderText.h"
#include "DTime.h"
#include "Touch.h"
#include "IRender.h"
#include "AxisMgr.h"
#include "BarMgr.h"
#include "TextMgr.h"
// during development I like to keep all data public. cleanup will be done later.


/*
 * Helper class that split a string and parse its values to an array of floats.
 */
/*
class ParseInput
{
public:
	// static data
	// dynamic data
	ParseInput(){}
	ParseInput(char *data,int len = -1) : m_data(data) {}
	~ParseInput() {}

	void set(char *data,int len = -1)
	{
		m_data = data;
		m_len = len;
		if (m_len < 0)
			m_len = strlen(data);
	}

	int parse2d(std::vector<float>&table, char delim=',')
	{
		// Need a join function... check where...

		// simple join func
		if (m_len < 0)
			m_len = strlen(m_data);

		if (m_len <= 0)
			return -1;

		for(int i=0; i < m_len;)
		{
			char *pos = strchr(&m_data[i], (int)delim);
			if (pos == 0)
			{
				// whole string is one data entry or error
				float f = atof(&m_data[i]);
				table.push_back(f);
				break;
			}
			int len = pos - &m_data[i];
			i += len+1;
		}
		return 0;
	}

protected:

	char *m_data;		// input data stream or static.
	int  m_len;
};
*/



/**
 * \brief MoGraph namespace
 * contains all graph and input of data classes into the same namespace..
 */

namespace MoGraph
{

class Scene;



/*
 * Input class for Graphs handling Static or Dynamic or Stream of data.
 */
/*
class Input
{
public:
	ParseInput m_input;

	// static

	// dynamic
protected:
};
*/
/*
 * Screen class manageing widget size in absolute resolutions, info
 */
/*
class Screen
{
public:
	// gui obj
	int 				m_width;
	int 				m_height;
	bool 				m_fullscreen;
	int 				m_type;
	// table should be an own class handling the range of data to be displayed, average out dependent on scale etc.
	std::vector<float> 	m_table;
	// input obj

protected:

};
*/


/**
 * \brief Scene handling all object in scene.
 * holds scene matrices like Perspective Projection, View and World Matrices
 */

class Scene
{
protected:
	bool 		mFitToScreen;		// fit to screen or use a std fixed view matrix.
	int 		mGridX;				// Bar grid X
	int 		mGridZ;				// Bar grid Z note mGridX*mGridZ = total amount of bars being used.
	glm::vec3	mGrid;				// floating point version of the Grid
	int			mWidth;
	int			mHeight;
	float 		mElapsedTime;		// Elapsed time in ms
	float 		mDistToCam;			// internally calculated for fit to screen.

	glm::mat4 	mProjection;		// Projection Matrix
	glm::mat4 	mView;				// View Matrix
	glm::mat4 	mWorld;				// World Matrix
	glm::mat4 	mPVW;				// Projection * View * World Matrix

	AxisMgr 	mAxisMgr;			// Axis mangaer contains all Axis and Grid lineups dependent on graph type
	BarMgr  	mBarMgr;			// Bar manager contains all bars and vertices common for them
	TextMgr 	mTextMgr;			// Text manager handles fonts for text printouts.

	float		*mValues;			// Bar values
	int			mValuesSz;

	glm::vec4 	*mColors;			// Color values
	int			mColorsSz;

	glm::vec4 	mDefaultBarColor;			// default green color

	std::vector<unsigned short> mIndices;	// Index list of faces

public:

	/**
	 * \brief Scene, Construction
	 */
	Scene();

	/**
	 * \brief create, whole scene by using Axis,Bars,Text
	 * @param gridX	,create amount of bars in X
	 * @param gridZ	,create amount of bars in Y
	 * @param lines	,amount lines in Y used for grid.
	 * @param step	,grid steps in Y
	 * @param bFitToScreen	,fit to screen flag, whole bars.
	 */
	void 	create(int gridX,int gridZ, int lines = 5, float stepY = 1.0f, bool bFitToScreen = true);


	BarMgr 	&getBarMgr() 			{return mBarMgr;}
	AxisMgr &getAxisMgr()			{return mAxisMgr;}
	TextMgr &getTextMgr()			{return mTextMgr;}
	int 	getGridX()				{return mGridX;}
	int		getGridZ() 				{return mGridZ;}
	glm::vec3 &getGrid()			{return mGrid;}
	float	getCx()					{return -mGrid.x*0.5f;}	// GridX & GridZ needs a float type due to its conversion
	float	getCz()					{return mGrid.z*0.5f;}
	glm::mat4 & getProjectionMat() 	{return mProjection;}
	glm::mat4 & getViewMat()		{return mView;}
	glm::mat4 & getWorldMat()		{return mWorld;}
	glm::mat4 & getPVWMat()			{return mPVW;}
	void setWorldMat(glm::mat4 &m) 	{mWorld = m;}
	std::vector<unsigned short> &getIndices() {return mIndices;}
	void	setElapsedTime(float time)		{mElapsedTime = time;}
	float	getElapsedTime()		{return mElapsedTime;}
	void	setWidth(int w)			{mWidth = w;}
	int		getWidth()				{return mWidth;}
	void	setHeight(int h)		{mHeight = h;}
	int		getHeight()				{return mHeight;}
	void 	setDefaultBarColor(glm::vec4 color) {mDefaultBarColor = color;}

	/**
	 * \brief updateMatrix, prepares a Perspective View World = PVW matrix used for the vertex shader.
	 */
	void updateMatrix();

	/**
	 * \brief updateCamera , update the ViewMatrix and PerspectiveProjection
	 * with new values like distance to the Camera
	 * @param scale
	 */
	void updateCamera(float scale);

	/**
	 * \brief setValues, set the height values pointer to an array
	 * \note data shall be contained according the the grid set up
	 * e.g. gridX = 10 gridZ=2 => first 10 entries are first row.
	 * and last 10 entries are second row in the graph like striding data
	 * gridX*z+x = index  (stride in width is same as gridX)
	 * @param values , graph height values
	 * @param sz , input size of array
	 */
	void setValues(float *values,int sz) 		{ mValues = values; mValuesSz = sz;}

	/**
	 * \brief setColors, set the color data for each bar.
	 * \note see setValues how data shall be stored.
	 * @param colors
	 * @param sz
	 */
	void setColors(glm::vec4 *colors, int sz) 	{mColors = colors; mColorsSz = sz;}

	/**
	 * \brief getValue, get a specific height value from the table
	 * @param i , input index of the values to get.
	 * @return value, height of bar
	 */
	float getValue(int i) 						{return (mValues)? mValues[i]: 1.0f;}

	/**
	 * \brief getColor, get a color from a specific bar
	 * @param i, input index entry for the bar to get the color from
	 * @return color object
	 */
	glm::vec4 & getColor(int i) 				{return (mColors)? mColors[i]: mDefaultBarColor;}

};


// TODO use a screen class. for handling view port and offset positions so that we can support sub windows for rendering multiple views. (graphs)

/**
 * \brief Graph
 * This is the main interfaced class for the Graph lib.
 */

class Graph : public IGraph /* should include a interface here. */
{
protected:
	Scene 	mScene;				// Scene class, contains all scene objects and math
	IFont 	*mFont;				// Font to be used for text rendering
	int 	mWidth;				// screen width
	int 	mHeight;			// screen height
	int		mGridX;				// grid in X (graph area)
	int 	mGridZ;				// grid in Z (graph area)
	DTime	mDeltaTime;			// Delta time class (handles time in between ticks)
	Time	mTime;				// Elapsed time class (accumulated time since constr)
	glm::vec4 mBGColor;			// background color for graph
	glm::vec2 mViewPos;			// View Port Position for view port. \note glViewPort uses this and mWidth mHeigh for positioning the view.
	TouchInput mTouch;			// helper class for handling touch swipes and scales.
	RenderText	mRenderText;	// Text Render class thats fixes texts..

	/**
	 * \brief drawBars, draw all graph bars in scene
	 * @param tick, elapsed time tick
	 */
	void drawBars(float tick)	{mScene.getBarMgr().draw();}

	/**
	 * \brief drawAxis, draw all grid lines and axis in scene
	 * @param tick, elapsed time tick
	 */
	void drawAxis(float tick)	{mScene.getAxisMgr().draw();}

	/**
	 * \brief drawText, render all text in the scene
	 * @param tick, elapsed time tick
	 */
	void drawText(float tick);

	/**
	 * \brief initShaderLines, initiate shader for lines
	 */
	void initShaderLines()		{mScene.getAxisMgr().init();}
	/**
	 * \brief initShaderBars,	initiate shader for bars
	 */
	void initShaderBars()		{mScene.getBarMgr().init();}
	/**
	 * \brief initShaderText,	initiate shader for Text
	 */
	void initShaderText()		{mScene.getTextMgr().init();}
	/**
	 * \brief initGL,	initiate OpenGL basic stuff that required for MoGraph
	 * @return
	 */
	int  initGL();

public:
	/**
	 * \brief Graph,	Constructor
	 */
	Graph() :
		mFont(0),
		mWidth(1),
		mHeight(1),
		mGridX(1),
		mGridZ(1),
		mBGColor(0.0f,0.0f,0.0f,1.0f),
		mViewPos(0.0f,0.0f)
	{}

	/**
	 * \brief ~Graph,	Destructor
	 */
	virtual ~Graph() {}

	/**
	 * \brief Graph::init initiate the whole graph system
	 * @param xGridSz		, grid in X
	 * @param zGridSz		, grid in Z
	 * @param gridLinesY	, amount of Grids in Y
	 * @param stepY			, step between Y grids
	 * @param bFitScreen	, flag to fit screen or not
	 * @param font			, font to use for rendering text in Graph
	 * @param scrWidth		, width of screen
	 * @param scrHeight		, height of screen
	 * @return int if successfull or not.
	 */
	virtual int init(int xGridSz,int zGridSz, int gridLinesY, float stepY, bool bFitScreen, IFont* font,int scrWidth,int scrHeight);

	/**
	 * \brief draw,	render entire scene
	 */
	virtual void draw();

	/**
	 * \brief setValues,	set the bar height value array
	 * @param values,	input pointer to a float array
	 * @param sz,	input size of array
	 */
	virtual void setValues(float *values,int sz) 			{mScene.setValues(values,sz);}

	/**
	 * \brief setColor,	set the bar color array
	 * @param colors,	input pointer to a glm::vec4 color array
	 * @param sz,	input size of color array
	 */
	virtual void setColors(glm::vec4 *colors, int sz) 		{mScene.setColors(colors,sz);}

	/**
	 * \brief setBGColor
	 * @param color
	 */
	virtual void setBGColor(glm::vec4 &color) 				{mBGColor = color;}

	/**
	 * \brief getScene,	get the reference to the scene class
	 * @return	scene
	 */
	virtual Scene &getScene() 								{return mScene;}

	/**
	 * \brief getTouch,	get the reference to the TouchInput helper class
	 * @return touch
	 */
	virtual TouchInput &getTouch()							{return mTouch;}
};

}
#endif /* GRAPH_H_ */
