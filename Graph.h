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
 * 	\brief Render
 *  Base class used for all render objects like Bars, Text, Lines
 *	contains a reference to the scene,
 *	the scene objects has all relevant information about the scene
 */

class Render
{
public:
	/**
	 * \brief Render , Constructor
	 * @param scene , input parameter for reference store to scene
	 */
	Render(Scene &scene) : mScene(scene) {}

	/**
	 * \brief ~Render , Destructor
	 */
	virtual~Render() 		{};

	/**
	 * \brief init(),	generic init function for all Renderable objects
	 * that inherits Render class
	 */
	virtual void init() 	{};

	/**
	 * \brief draw(),	generic draw function for all Renderable objects
	 * that inherits Render class
	 */
	virtual void draw() 	{};

	/**
	 * \brief getScene(),	get the Scene class reference
	 * which this objects belongs to, (parent)
	 * @return Scene
	 */
	Scene &getScene() 		{return mScene;}

protected:
	Scene &mScene;		// Scene reference
};


/**
 * \brief Bar class handles one single bar used by BarMgr for rendering
 */

class Bar
{
protected:
	float 		mValue;
	glm::vec4 	mColor;

public:
	/**
	 * \brief Bar Constructor
	 */
	Bar() : mValue(1.0f), mColor(1.0f,1.0f,1.0f,1.0f)	{}

	/**
	 * \brief setColor,	sets the r,g,b,a as float color component for the bar.
	 * @param r
	 * @param g
	 * @param b
	 * @param a
	 */
	void setColor(float r,float g,float b, float a)		{mColor.x = r; mColor.y = g; mColor.z = b; mColor.w = a;}

	/**
	 * \brief setColor, with glm::vec4 as input.
	 * @param col
	 */
	void setColor(glm::vec4 &col)						{mColor = col;}

	/**
	 * \brief setValue, bar height value in float
	 * @param val
	 */
	void setValue(float val) 							{mValue = val;}

	/**
	 * \brief getValue, get the bar height value
	 * @return output height value
	 */
	float getValue() 									{return mValue;}

	/**
	 * \brief getColor, get the bar color value
	 * @return output the bar color
	 */
	glm::vec4 & getColor() 								{return mColor;}
};


/**
 * \brief BarMgr handles all the bars that are being displayed
 *	using Render base class for the render calls init() & draw()
 */

class BarMgr : public Render
{
protected:
	std::vector<Bar> 			mBarArray;		// question absolute amount of bars or display fromc..to
	std::vector<glm::vec3> 		mVertices;
	std::vector<unsigned short> mFaces;
	BarShader					mShader;

	/**
	 * \brief create3D
	 * helper function to set the vertex array & index array
	 * Creates default vertex buffer & index buffer
	 */

	void create3D();			// create 3D obj. one instance of a cube with origo at bottom with indices
public:

	/**
	 * \brief BarMgr Constructor
	 * @param scene input reference for the scene
	 */
	BarMgr(Scene &scene) : Render(scene) 	{create3D();}

	/**
	 * \brief ~BarMgr Destructor
	 */
	virtual ~BarMgr()						{}

	/**
	 * \brief addBars, 	set up amount of bars that shall be used for the graph
	 * @param sz		input amount of bars
	 */
	void addBars(int sz) 					{mBarArray.resize(sz);}

	/**
	 * \brief addbar,	add one bar to the table
	 * @param bar , adds this bar to the mBarArray table as last entry
	 */
	void addBar(Bar &bar) 					{mBarArray.push_back(bar);}

	/**
	 * \brief getBars,	get the table of all bars.
	 * @return
	 */
	std::vector<Bar> &getBars() 			{return mBarArray;}

	/**
	 * \brief size,	get the bars table size
	 * @return size of table
	 */
	int size()								{return mBarArray.size();}

	/**
	 * \brief getBar, get a element from the Bars array
	 * @param i,	element index
	 * @return Bar entry
	 */
	Bar &getBar(int i) 						{return mBarArray[i];}

	/**
	 * \brief common vertices used for all bars
	 * @return vector of vertices
	 */
	std::vector<glm::vec3>& vertices() 		{return mVertices;}

	/**
	 * \brief common index table of bars
	 * @return vector of faces
	 */
	std::vector<unsigned short>& faces() 	{return mFaces;}

	/**
	 * \brief getShader,
	 * @return
	 */
	BarShader &getShader() 					{return mShader;}

	// virtuals from render class
	/**
	 * \brief init, initiate bar manager class to set up the graph bars
	 * \note see virtuals from render class
	 */
	virtual void init();

	/**
	 * \brief draw, render bars on screen
	 * \note see virtuals from render class
	 */
	virtual void draw();
};


/**
 * \brief Handles the AXIS lines includeing grid marks to show the steps for a value.
 *
 */
class Axis
{
protected:

	std::vector<glm::vec3> mVertices;
public:

	/**
	 * \brief vertices, contains 2 points to form one line.
	 * @return reference of vertices,
	 */
	std::vector<glm::vec3>& vertices()	{return mVertices;}

	/**
	 * \brief size, size of vertex array
	 * @return size
	 */
	int size() 							{return mVertices.size();}
};


/**
 * \brief AxisMgr class handles several Axis lines for the graph display
 */

class AxisMgr : public Render
{
protected:
	std::vector<Axis> 	mAxisArray;		// value and time  can support up to 3D X,Y,Z
	LineShader			mShader; 	// Shader to be used for rendering the lines.
	float				mGridStep;	// Step of each grid bars in height. (note total height is amount*step)
	int 				mGridLines;	// Grid lines for graph bar in Y... stepped with mStep, to get height grid,
	void create3D();
public:
	enum {AXIS_X,AXIS_Y,AXIS_Z};

	/**
	 * \brief AxisMgr, Constructor
	 * @param scene, input reference to its scene
	 */
	AxisMgr(Scene &scene) : Render(scene), mGridStep(1.0f), mGridLines(2) {}

	/**
	 * \brief AxisMgr, Destructor
	 */
	virtual ~AxisMgr() {}

	/**
	 * \brief getAxis
	 * @param i,	input index entry
	 * @return Axis output element
	 */
	Axis &getAxis(int i) 				{return mAxisArray[i];}

	/**
	 * \brief getAxisArray
	 * @return	reference of Axis array
	 */
	std::vector<Axis>& getAxisArray() 	{return mAxisArray;}

	/**
	 * \brief size, size of AxisArray
	 * @return size
	 */
	int size() 							{return mAxisArray.size();}

	/**
	 * \brief getShader , get Shader of Axis lines
	 * @return LineShader
	 */
	LineShader &getShader() 			{return mShader;}

	/**
	 * \brief addAxis, add Axis x,y,z to axis array, see enumeration for order
	 * @param axis input enumaration of AXIS_X ..
	 */
	void addAxis(int axis);

	/**
	 * \brief setGridStep, sets the line grid step in Y
	 * @param step, input float
	 */
	void setGridStep(float step)		{mGridStep  = step;}

	/**
	 * \brief setGridLines,	set the amount of grid lines i Y
	 * @param nLines
	 */
	void setGridLines(int nLines)		{mGridLines = nLines;}

	/**
	 * \brief getGridStep, gets the grid step for Y between lines.
	 * @return grid step in Y
	 */
	float getGridStep()					{return mGridStep;}

	/**
	 * \brief getGridLines,	get the amount of lines in Y grid
	 * @return
	 */
	int	getGridLines()					{return mGridLines;}

	/**
	 * \brief init, initiate AxisMgr,
	 * \note using virtuals from Render class
	 */
	virtual void init();

	/**
	 * \brief draw, render all grid lines and axis lines.
	 * \note using virtuals from Render class
	 */
	virtual void draw();
};


/**
 * \brief Text struct text display in graph
 */

struct Text
{
	/**
	 * \brief TextEnum
	 * where the origo is relative inside the text string should be.
	 */
	enum TextEnum{ NO_ACTION, CENTER_LEFT, CENTER, CENTER_RIGHT };

	std::string 	mText;			// Output text string
	TextEnum		mTextFlag;		// Enumeration flag where origo shall be
	glm::vec3 		mPos;			// Text position in 3d space
	glm::vec4 		mColor;			// Text color
	glm::vec2		mScale;			// Text scale
	glm::vec3		mRotate;		// Rotation in degrees. prio Yaw Pitch Roll
//	int 			mFontType;		// Optional not implemented reference for the font to use
};


/**
 * \bief TextMgr class handle all texts that is needed for the graphs.
 */

class TextMgr : public Render
{
protected:
	std::vector<Text> mTextArray;
	// add some common properties
	// Text Shader using point sprites ?
public:
	/**
	 * \brief TextMgr	, Text Manager Constructor
	 * @param scene ,input reference to scene
	 */
	TextMgr(Scene &scene);

	/**
	 * \brief getTextArray,
	 * @return
	 */
	std::vector<Text>	&getTextArray()	{return mTextArray;}

	/**
	 * \brief getTextArray , gets the vector array for texts.
	 * @param i , input array index of text
	 * @return text struct reference
	 */
	Text &getText(int i) 					{return mTextArray[i];}

	/**
	 * \brief size	get sizeof TextArray
	 * @return size
	 */
	int size() 								{return mTextArray.size();}

	/**
	 * \brief ~TextMgr, Destructor
	 */
	virtual ~TextMgr()						{}

	/**
	 * \brief init, initiate TextMgr,
	 * \note using virtuals from Render class
	 */
	virtual void init();

	/**
	 * \brief draw, render all Text Strings in 3D space.
	 * \note using virtuals from Render class
	 */
	virtual void draw();
};


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
