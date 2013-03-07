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
	Render(Scene &scene) : mScene(scene) {}
	virtual~Render() 		{};
	virtual void init() 	{};
	virtual void draw() 	{};
	Scene &getScene() 		{return mScene;}

protected:
	Scene &mScene;
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
	Bar() : mValue(1.0f), mColor(1.0f,1.0f,1.0f,1.0f)	{}
	void setColor(float r,float g,float b, float a)		{mColor.x = r; mColor.y = g; mColor.z = b; mColor.w = a;}
	void setColor(glm::vec4 &col)						{mColor = col;}
	void setValue(float val) 							{mValue = val;}
	float getValue() 									{return mValue;}
	glm::vec4 & getColor() 								{return mColor;}
};


/**
 * \brief BarMgr handles all the bars that are being displayed
 *	using Render base class for the render calls init() & draw()
 */

class BarMgr : public Render
{
protected:
	std::vector<Bar> 			mBars;		// question absolute amount of bars or display fromc..to
	std::vector<glm::vec3> 		mVertices;
	std::vector<unsigned short> mFaces;
	BarShader					mShader;

	void create3D();			// create 3D obj. one instance of a cube with origo at bottom with indices
public:


	BarMgr(Scene &scene) : Render(scene) 	{create3D();}
	virtual ~BarMgr()						{}

	void addBars(int n) 					{mBars.resize(n);}
	void addBar(Bar &b) 					{mBars.push_back(b);}
	std::vector<Bar> &getBars() 			{return mBars;}
	int size()								{return mBars.size();}
	Bar &getBar(int i) 						{return mBars[i];}
	std::vector<glm::vec3>& vertices() 		{return mVertices;}
	std::vector<unsigned short>& faces() 	{return mFaces;}
	BarShader &getShader() 					{return mShader;}

	// virtuals from render class
	virtual void init();
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
	std::vector<glm::vec3>& vertices()	{return mVertices;}
	int size() 							{return mVertices.size();}
};


/**
 * \brief AxisMgr class handles several Axis lines for the graph display
 */

class AxisMgr : public Render
{
protected:
	std::vector<Axis> 	mAxis;		// value and time  can support up to 3D X,Y,Z
	LineShader			mShader; 	// Shader to be used for rendering the lines.
	float				mGridStep;	// Step of each grid bars in height. (note total height is amount*step)
	int 				mGridLines;	// Grid lines for graph bar in Y... stepped with mStep, to get height grid,
	void create3D();
public:
	AxisMgr(Scene &scene) : Render(scene), mGridStep(1.0f), mGridLines(2) {}
	virtual ~AxisMgr() {}

	enum {AXIS_X,AXIS_Y,AXIS_Z};
	Axis &getAxis(int i) 				{return mAxis[i];}
	std::vector<Axis>& getAxisArray() 	{return mAxis;}
	int size() 							{return mAxis.size();}
	LineShader &getShader() 			{return mShader;}
	void addAxis(int axis);
	void setGridStep(float step)		{mGridStep  = step;}
	void setGridLines(int nLines)		{mGridLines = nLines;}
	float getGridStep()					{return mGridStep;}
	int	getGridLines()					{return mGridLines;}
	// virtuals from render class
	virtual void init();
	virtual void draw();
};


/**
 * \brief Text class text display in graph
 */

struct Text
{
	enum TextEnum{ NO_ACTION, CENTER_LEFT, CENTER, CENTER_RIGHT };

	std::string 	mText;
	TextEnum		mTextFlag;
	glm::vec3 		mPos;
	glm::vec4 		mColor;
	glm::vec2		mScale;
	glm::vec3		mRotate;
//	int 			mFontType;		// Optional not implemented reference for the font to use
	// need to add text data to render here.
};


/**
 * \bief TextMgr class handle all texts that is needed for the graphs.
 */

class TextMgr : public Render
{
protected:
	std::vector<Text> mText;
	// add some common properties
	// Text Shader using point sprites ?
public:
	TextMgr(Scene &scene);
	std::vector<Text>	&getTextArray()		{return mText;}
	Text &getText(int i) 					{return mText[i];}
	int size() 								{return mText.size();}

	virtual ~TextMgr()						{}
	virtual void init();
	virtual void draw();
};


/**
 * \brief Scene handling all object in scene.
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

	glm::vec4 	mDefaultBarColor;		// default green color

	std::vector<unsigned short> mIndices;	// Index list of faces

public:
	Scene(); // : mFitToScreen(true), mGridX(1), mGridZ(1), mWidth(1), mHeight(1), mDistToCam(1.0f), mAxisMgr(*this), mBarMgr(*this), mTextMgr(*this), mDefaultBarColor(0.25f,1.0f,0.25f,1.0f){}
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
	void updateMatrix();
	void updateCamera(float scale);
	void setValues(float *values,int sz) 		{ mValues = values; mValuesSz = sz;}
	void setColors(glm::vec4 *colors, int sz) 	{mColors = colors; mColorsSz = sz;}
	float getValue(int i) 						{return (mValues)? mValues[i]: 1.0f;}
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
	Scene 	mScene;
	IFont 	*mFont;
	int 	mWidth;
	int 	mHeight;
	int		mGridX;
	int 	mGridZ;
	// todo move them outside Graph..
	DTime	mDeltaTime;		// Delta time class (handles time in between ticks)
	Time	mTime;			// Elapsed time class (accumulated time since constr)
	glm::vec4 mBKColor;
	glm::vec2 mViewPos;		// View Port Position for view port. \note glViewPort uses this and mWidth mHeigh for positioning the view.
	TouchInput mTouch;


	RenderText		mRenderText;
	void drawBars(float tick)	{mScene.getBarMgr().draw();}
	void drawAxis(float tick)	{mScene.getAxisMgr().draw();}
	void drawText(float tick);

	void initShaderLines()		{mScene.getAxisMgr().init();}
	void initShaderBars()		{mScene.getBarMgr().init();}
	void initShaderText()		{mScene.getTextMgr().init();}
	int  initGL();

public:
	Graph() :
		mFont(0),
		mWidth(1),
		mHeight(1),
		mGridX(1),
		mGridZ(1),
		mBKColor(0.0f,0.0f,0.0f,1.0f),
		mViewPos(0.0f,0.0f)
	{}
	virtual ~Graph() {}

	/**
	 * \brief Graph::init initiate the whole graph system
	 * @param x				, grid in X
	 * @param z				, grid in Z
	 * @param gridLines		, amount of Grids in Y
	 * @param step			, step between Y grids
	 * @param bFitScreen	, flag to fit screen or not
	 * @param font			, font to use for rendering text in Graph
	 * @param width			, width of screen
	 * @param height		, height of screen
	 * @return int if successfull or not.
	 */
	virtual int init(int x,int z, int gridLines, float step, bool bFitScreen, IFont* font,int width,int height);
	virtual void draw();

	virtual void setValues(float *values,int sz) 			{mScene.setValues(values,sz);}
	virtual void setColors(glm::vec4 *colors, int sz) 		{mScene.setColors(colors,sz);}
	virtual void setBKColor(glm::vec4 &color) 				{mBKColor = color;}
	virtual Scene &getScene() 								{return mScene;}
	virtual TouchInput &getTouch()							{return mTouch;}
};

}
#endif /* GRAPH_H_ */
