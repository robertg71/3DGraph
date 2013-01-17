/*
 * Graph.h
 *
 *  Created on: Jan 10, 2013
 *      Author: CodeArt
 */

#ifndef GRAPH_H_
#define GRAPH_H_
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
// during development I lake to keep all data public. cleanup will be done later.


/*
 * Helper class that split a string and parse its values to an array of floats.
 */
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




/*
 * Graph namespace , that contains all graph and input of data classes into the same namespace..
 */
namespace Graph
{

/*
 * Input class for Graphs handling Static or Dynamic or Stream of data.
 */
class Input
{
public:
	ParseInput m_input;

	// static

	// dynamic
protected:
};

/*
 * Screen class manageing widget size in absolute resolutions, info
 */
class Screen
{
public:
	// gui obj
	int m_width;
	int m_height;
	bool m_fullscreen;
	int m_type;

	// table should be an own class handling the range of data to be displayed, average out dependent on scale etc.
	std::vector<float> m_table;
	// input obj

protected:

};

/*
 * add multiple info / vertex if needed.
 */
struct VertexT
{
    glm::vec3 pos[3];
    glm::vec2 tex[2];
};

/*
 * Bar class handles one single bar for rendering
 */

// TODO split up obje part for object handling.
class Bar
{
protected:
	float mValue;
	std::vector<glm::vec3> mVertices;
	std::vector<unsigned short> mFaces;
	glm::vec4 mColor;
	glm::mat4 mWorld;		// Object in world space.

public:
	Bar() : mWorld(1.0f), mValue(1.0f) { createBar3D(); }
	void createBar3D();
	void setColor(float r,float g,float b, float a)		{ mColor.x = r; mColor.y = g; mColor.z = b; mColor.w = a; }
	void setValue(float val) {mValue = val;}
	float getValue() {return mValue;}
	glm::mat4x4 &getMatrix() {return mWorld;}
	void setMatrix(glm::mat4 &m) {mWorld = m;}
	std::vector<glm::vec3>& vertices() {return mVertices;}
	std::vector<unsigned short>& faces() {return mFaces;}
};

/*
 * BarMgr handles all the bars that are being displayed
 *
 */
class BarMgr
{
protected:
	std::vector<Bar> mBars;		// question absolute amount of bars or display fromc..to
	int	mEnd;					// incase displaying from .. to
	int mStart;
	float mStep;

public:
	void addBars(int n) {mBars.resize(n);}
	void addBar(Bar &b) {mBars.push_back(b);}
	std::vector<Bar> &getBars() {return mBars;}
	int size()	{mBars.size();}
	Bar &getBar(int i) {return mBars[i];}

};

/*
 * Handles the AXIS lines includeing grid marks to show the steps for a value.
 */
class Axis
{
public:
	float mAxisMax;
	float mAxisMin;
	float mGrid;
	float mStep;
	glm::vec2 mPoint;
	std::vector<glm::vec3> mVertices;
	std::vector<int> mFaces;		// optional how to draw the lines between the vector points
	int type;				// type of axis lines or more 3d? future ref
	std::string mText;		// Axis text
};

/*
 * AxisMgr class handles several Axis lines for the graph display
 */
class AxisMgr
{
public:
	Axis mAxis[3];	// value and time  can support up to 3D X,Y,Z
};

/*
 * Text class text display in graph
 */
class Text
{
public:
	std::string mText;
	glm::vec3 mPos;
	glm::vec3 mColor;
	int mFontType;		// reference for the font to use
	// need to add text data to render here.
};

/*
 * TextMgr class handle all texts that is needed for the graphs.
 */
class TextMgr
{
public:
	std::vector<Text> mTextMgr;
	// add some common properties
};

/*
 * Scene handling all object in scene.
 */
class Scene
{
public:
	AxisMgr mAxisMgr;
	BarMgr  mBarMgr;		// note bars have to be dynamically build in order to handle different type of graph displays
	TextMgr mTextMgr;
	// need other display element to use.
};

/*
 * Layout
 */
class Layout
{
public:
	Scene	mScene;
	// add layout properties
	int mGraphType;	// an enum table displaying what type of graph to use.
};


}
#endif /* GRAPH_H_ */
