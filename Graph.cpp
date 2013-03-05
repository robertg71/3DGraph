/*
 * Graph.cpp
 *
 *  Created on: Jan 14, 2013
 *      Author: CodeArt
 */
#include <mavsprintf.h>
#include "graph.h"
#include "GFont.h"
#include "RenderText.h"
#include "MAHeaders.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// Name space Graph
namespace MoGraph
{
	/*
	 * helper function to set the vertex array index array
	 * Creates default vertex buffer
	 */
	void BarMgr::create3D()
	{
			// Define quad vertices. origo Y = 0
		 const float Vertices[] = {
				1.0f, 	0.0f, 	1.0f,
				1.0f, 	1.0f, 	1.0f,
				-1.0f, 	1.0f, 	1.0f,
				-1.0f, 	0.0f, 	1.0f,
				1.0f, 	0.0f, 	-1.0f,
				1.0f, 	1.0f, 	-1.0f,
				-1.0f, 	1.0f, 	-1.0f,
				-1.0f, 	0.0f, 	-1.0f,
		};

		const unsigned short Indices[] = {
		    // Front
		    0, 1, 2,
		    2, 3, 0,
		    // Back
		    4, 6, 5,
		    4, 7, 6,
		    // Left
		    2, 7, 3,
		    6, 7, 2,
		    // Right
		    0, 4, 1,
		    1, 4, 5,
		    // Top
		    6, 2, 1,
		    5, 6, 1,
		    // Bottom
		    0, 3, 7,
		    0, 7, 4
		};

		for(int i=0; i<8*3; i+=3)
		{
			mVertices.push_back(glm::vec3(Vertices[i],Vertices[i+1],Vertices[i+2]));
		}

		for(int i=0; i<12*3; i++)
		{
			mFaces.push_back(Indices[i]);
		}

	}

	Scene::Scene() :
			mFitToScreen(true),
			mGridX(1),
			mGridZ(1),
			mWidth(1),
			mHeight(1),
			mDistToCam(1.0f),
			mAxisMgr(*this),
			mBarMgr(*this),
			mTextMgr(*this),
			mDefaultBarColor(0.25f,1.0f,0.25f,1.0f)
	{
	}

	void Scene::updateMatrix()
	{
		mPVW =  mProjection * mView * mWorld;
	}

	// Create whole scene by using Axis,Bars,Text
	void Scene::create(int gridX, int gridZ, int lines, float step ,bool bFitToScreen)
	{
		mFitToScreen 		= bFitToScreen;
		mGridX 				= gridX;
		mGridZ 				= gridZ;
		mGrid.x				= gridX;
		mGrid.y				= 1.0f;
		mGrid.z				= gridZ;
		int newSize 		= gridX*gridZ;

		lprintfln("Scene::create: %i*%i=%i",mGridX,mGridZ,newSize);
		mBarMgr.addBars(newSize);
		lprintfln("vector<Bar> bars, size() = %i == %i", mBarMgr.size(),newSize);

		int axis = ((gridX>1)&&(gridZ>1))?3:2;	// how many axis should be displayed 2 or 3 dependent on layout... 2d => 2 => 3d => 3
		mAxisMgr.addAxis(axis);
		mAxisMgr.setGridLines(lines);
		mAxisMgr.setGridStep(step);
		mWorld = glm::mat4(1.0f);				// set up an identity matrix
		updateCamera(1.0f);
	}

	void Scene::updateCamera(float scale)
	{
		const float width 		= EXTENT_X(maGetScrSize());
		const float height 		= EXTENT_Y(maGetScrSize());
		const float aspect 	= width/height;		// Net to calculate this this is HardCoded. 2.0f/3.0f;//1.0f/
		const float cx 		= getCx();
		const float cz 		= getCz();
		const float res 	= 1.0f/scale * glm::sqrt(cx*cx+cz*cz);
		mDistToCam 			= res;
		mProjection 		= glm::perspective(45.0f, aspect, 0.1f, res*5.0f); 		// Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units

		if (mFitToScreen == true)
		{												// Camera matrix
			mView       = glm::lookAt(
			glm::vec3(0.0f,res*0.5f,res*2.0f*(1.0f/aspect)), 	// Camera is at (0,x/2,x*(1/aspec)), in World Space
			glm::vec3(0,0,0), 							// and looks at the origin
			glm::vec3(0,1,0)  							// Head is up (set to 0,-1,0 to look upside-down)
			);
		}
		else											// set up a default camera matrix
		{
			mView       = glm::lookAt(
			glm::vec3(0.0f,10.0f,20.0f), 				// Camera is at (0,10,20), in World Space
			glm::vec3(0,0,0), 							// and looks at the origin
			glm::vec3(0,1,0)  							// Head is up (set to 0,-1,0 to look upside-down)
			);
		}
	}

	// AxisMgr::create3D  creates default vertex buffer
	void AxisMgr::create3D()
	{
		const float v[] =
		{
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		};
		// set up 2D axis X and Y
		mAxis[0].vertices().push_back(glm::vec3(v[0],v[1],v[2]));
		mAxis[0].vertices().push_back(glm::vec3(v[3],v[4],v[5]));
		mAxis[1].vertices().push_back(glm::vec3(v[6],v[7],v[8]));
		mAxis[1].vertices().push_back(glm::vec3(v[9],v[10],v[11]));

		if(mAxis.size()>2)		// 3D Graph use a 3rd axis in Z.
		{
			mAxis[2].vertices().push_back(glm::vec3(v[12],v[13],v[14]));
			mAxis[2].vertices().push_back(glm::vec3(v[15],v[16],v[17]));
		}
		else
		{
			lprintfln("AxisMgr::create3d: ERROR Axis system got Graph bars can either be 2D or 3D hence 2 or 3 as input");   			// error not declared.
		}
	}

	void AxisMgr::addAxis(int n)
	{
		mAxis.resize(n);
		create3D();
	}

	void AxisMgr::init()
	{
		mShader.init();
		for (size_t i=0; i<mAxis.size();i++)
		{
			Axis &axis = mAxis[i];								// get reference of obj.
			glGenBuffers(1, &mShader.mVertexbuffer[i]);					// Generate a vertex buffer for all axis (line)
			glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer[i]);
			glBufferData(GL_ARRAY_BUFFER, axis.size() * sizeof(glm::vec3), &axis.vertices()[0], GL_STATIC_DRAW);
		}
	}

	void AxisMgr::draw()
	{
		const float tick 			= mScene.getElapsedTime();
		LineShader &shader 	= getShader();
		glDisable(GL_CULL_FACE);
		glUseProgram(shader.mShader);
		checkGLError("glUseProgram");

		glm::vec3 sv(1.0f,1.0f,1.0f);

		// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
		// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
		glUniform1f(shader.mTimeLoc, tick);
		checkGLError("glUniform1f");
		glUniform2f(shader.mResolutionLoc, 1.0f/(float)mScene.getWidth(), 1.0f/(float)mScene.getHeight());
		checkGLError("glUniform2f");
		glUniformMatrix4fv(shader.mMatrixPVW, 1, GL_FALSE, &mScene.getPVWMat()[0][0]);
		checkGLError("glUniformMatrix4fv");
		glUniform3fv(shader.mScaleV,1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
		checkGLError("glUniform3fv");

		const float centerX = mScene.getCx()*1.01f;
		const float centerZ = mScene.getCz()*1.01f;

		for(size_t i=0; i<mAxis.size(); i++)
		{
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(shader.mAttribVtxLoc);
			glBindBuffer(GL_ARRAY_BUFFER, shader.mVertexbuffer[i]);
			glVertexAttribPointer(
				shader.mAttribVtxLoc,      // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			checkGLError("glEnableVertexAttribArray");
			glLineWidth(2);

			glm::vec4 col(0.5f,0.5f,0.5f,1.0f);
			glm::vec3 llength(-centerX*2.0f,5.0f,-centerZ*2.0f);	// length is always abs
			glm::vec4 tpos(centerX, 0.0f,centerZ, 1.0f);
			glUniform4fv(shader.mTPos,1, (float *)&tpos.x);
			glUniform4fv(shader.mColor,1, (float *)&col.x);
			glUniform3fv(shader.mLength,1, (float *)&llength.x);				// mScale location => variable "ScaleV" in vertex shader

			glDrawArrays(GL_LINES, 0, 2);

			// Set up grid lines on height for X-Axis
			if (i == 0)
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);
				for (int l=1;l<mGridLines;l++)
				{
					float gridY = static_cast<float>(l) * mGridStep;
					glm::vec4 tpos(centerX, gridY, centerZ, 1.0f);
					glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
					glDrawArrays(GL_LINES, 0, 2);
				}
			}
			else if (i==2)	// Set up grid lines in height for Z-Axis
			{
				glLineWidth(1);
				glm::vec4 col(0.25f,0.25f,0.25f,1.0f);
				glUniform4fv(shader.mColor,1, (float *)&col.x);
				for (int l=1;l<mGridLines;l++)
				{
					float gridY = static_cast<float>(l) * mGridStep;
					glm::vec4 tpos(centerX, gridY, centerZ, 1.0f);
					glUniform4fv(shader.mTPos, 1, (float *)&tpos.x);
					glDrawArrays(GL_LINES, 0, 2);
				}
			}
			glDisableVertexAttribArray(shader.mAttribVtxLoc);
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		// Clean-up
		glUseProgram(0);
	}

	void BarMgr::init()
	{
		std::vector<unsigned short> & indices = getScene().getIndices();	// Index list of faces

		// make a copy for now because we might need to add multiple indicies to this obj (there is only one index list)
		for(size_t i=0;i<mFaces.size();i++)
		{
			indices.push_back(mFaces[i]);
		}

		mShader.init();

		// Generate a buffer for the vertices
		glGenBuffers(1, &mShader.mVertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mShader.mVertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3), &mVertices[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices
		glGenBuffers(1, &mShader.mElementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mShader.mElementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	}

	void BarMgr::draw()
	{
		float tick = mScene.getElapsedTime();

		glEnable(GL_CULL_FACE);
		BarShader &shader = getShader();
		std::vector<unsigned short> &indices = mScene.getIndices();	// Index list of faces

		// Use the program object   shader and its specific arguments
		glUseProgram(shader.mShader);
		checkGLError("glUseProgram");

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(shader.mAttribVtxLoc);
		glBindBuffer(GL_ARRAY_BUFFER, shader.mVertexbuffer);
		glVertexAttribPointer(
			shader.mAttribVtxLoc,     	// attribute
			3,                  		// size
			GL_FLOAT,           		// type
			GL_FALSE,           		// normalized?
			0,                  		// stride
			(void*)0            		// array buffer offset
		);

		checkGLError("glEnableVertexAttribArray");

		// bind the Index buffer with vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader.mElementbuffer);

		// Update variables to the shader, that is only updated commonly for all bars once per frame such as ParojactionMatrix, ViewMatrix, should be World Matrix aswell
		// projectionMatrix and viewMatrix tick time, resolution constants for pixel shader that are identical trough out the obj calls. hence update only once.
		glUniform1f(shader.mTimeLoc, tick);
		checkGLError("glUniform1f");
		glUniform2f(shader.mResolutionLoc, 1.0f/(float)mScene.getWidth(), 1.0f/(float)mScene.getHeight());
		checkGLError("glUniform2f");
		glUniformMatrix4fv(shader.mMatrixPVW, 1, GL_FALSE, &mScene.getPVWMat()[0][0]);

		// setting up a 2D grid.  prepare const variable for a tight loop
		const int iGridX	= mScene.getGridX();
		const int iGridZ	= mScene.getGridZ();
		const float centerX = mScene.getCx();
		const float centerZ = mScene.getCz();
		glm::vec3 sv(0.5f,0.5f,0.5f);
		glm::vec4 tpos(0.0f,0.0f,0.0f,1.0f);
		for(int j=0; j<iGridZ; j++)
		{
			for(int i=0; i<iGridX; i++)
			{
				const int id = j*iGridX+i;
				Bar &bar = getBar(id);
				tpos.x 	= (centerX+i)+0.5f;
				tpos.z 	= (centerZ-j)-0.5f;
				sv.y 	= bar.getValue();
				bar.setColor(mScene.getColor(id));				//	bar.setColor(i/30.0f,0.0f,j/30.0f,1.0f);
				bar.setValue(mScene.getValue(id));

				// upload our obj matrix to the vertex shader.
				glUniform4fv(shader.mTPos,		1, (float *)&tpos.x);
				glUniform3fv(shader.mScaleV,	1, (float *)&sv.x);				// mScale location => variable "ScaleV" in vertex shader
				glUniform4fv(shader.mColor,		1, (float *)&bar.getColor().x);
				glDrawElements(
					 GL_TRIANGLES,      	// mode
					 indices.size(),    	// count
					 GL_UNSIGNED_SHORT,   	// type
					 (void*)0           	// element array buffer offset
				 );
			}
		}

		// Clean-up
		glDisableVertexAttribArray(shader.mAttribVtxLoc);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	void TextMgr::draw()
	{

	}

	void TextMgr::init()
	{
		Text t;
		t.mRotate 			= glm::vec3(0.0f,0.0f,0.0f);
		t.mTextFlag			= Text::NO_ACTION;

//		float gridX 		= mScene.getGridX();
		float gridZ 		= mScene.getGridZ();
		float centerX		= mScene.getCx();
		float centerZ		= mScene.getCz();
		float scale 		= mScene.getGridX()/500.0f;

		glm::vec2 scaleXZ(scale,scale);
		glm::vec3 pos(centerX, 0.0f,centerZ);
		glm::vec4 color(1.0f,1.0f,1.0f,1.0f);

		t.mColor 	= color;
		t.mText		= "MoSync 3D Graph Library 0.7 Beta";		// Subtitle
		t.mScale	= scaleXZ;
		t.mPos		= pos;
		mText.push_back(t);

		scaleXZ.x 	*= 0.8f;
		scaleXZ.y   *= 0.8f;

		t.mPos.y	+= 5.5f;
		t.mText		= "Y-Axis";		// Subtitle
		t.mScale	= scaleXZ;
		mText.push_back(t);

		t.mText		= "X-Axis";		// Subtitle
		t.mPos 		= glm::vec3(-centerX, 0.0f,centerZ);
		t.mTextFlag = Text::CENTER_RIGHT;
		mText.push_back(t);

		if (gridZ > 1)		// Check if there is a z-Axis at all
		{
			// set up text for Z-Axis
			t.mTextFlag = Text::NO_ACTION;
			t.mText 	= "Z-axis";
			t.mPos 		= glm::vec3(centerX, 0.0f,centerZ);
			t.mRotate	= glm::vec3(0.0f,-90.0f,0.0f);
			mText.push_back(t);
		}
	}

	TextMgr::TextMgr(Scene &scene) : Render(scene)
	{
	}

	void Graph::draw()
	{
		static int cnt = 0;
		cnt++;

		glViewport(0, 0, mWidth, mHeight);
			checkGLError("glViewport");
//		lprintfln("%d. draw()::glViewport w=%d h=%d\n",cnt,mWidth,mHeight);
		mDeltaTime.tick();
		float tick = static_cast<float>(mTime.update()) * 0.001f;
		mScene.setElapsedTime(tick);

		// Clear the color buffer
		glClearColor(mBKColor.r,mBKColor.g,mBKColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGLError("glClear");

		drawBars(tick);
		drawAxis(tick);
		drawText(tick);

	}

	int Graph::init(int x,int z, int gridLines, float step, bool bFitScreen, IFont* font,int width,int height)
	{
		mWidth 	= width;
		mHeight = height;
		mGridX	= x;
		mGridZ	= z;
		mFont	= font;
		mScene.create(x,z,gridLines,step,bFitScreen);
		mScene.setWidth(width);
		mScene.setHeight(height);
		return initGL();
	}

	int Graph::initGL()
	{
		// Set up common gl options
		glViewport(0, 0, mWidth, mHeight);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Enable back face culling
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		// set up clear color
		glClearColor(mBKColor.r,mBKColor.g,mBKColor.b, 1.0f);
		mRenderText.init(mWidth,mHeight,mFont);

		// create a braph with grid times grid
		initShaderBars();
		initShaderLines();
		initShaderText();

		return TRUE;
	}


	// TODO a system to set up text in the graph, it might need Z sorting etc.
	void Graph::drawText(float tick)
	{
		TextMgr &textMgr = mScene.getTextMgr();

		for (int i=0; i<textMgr.size(); i++)
		{
			Text &text = textMgr.getText(i);
			mRenderText.setScale(text.mScale.x,text.mScale.y);
			glm::vec3 pos = text.mPos;
			switch (text.mTextFlag)
			{
				case Text::CENTER:
					pos.x -= 0.5f * mRenderText.getTextWidth(text.mText.c_str());
					break;
				case Text::CENTER_RIGHT:
					pos.x -= mRenderText.getTextWidth(text.mText.c_str());
					break;
				case Text::CENTER_LEFT:		// obsolete because it is by default
					break;
				default:
					break;
			}
			if (text.mRotate.x == 0.0f && text.mRotate.y == 0.0f && text.mRotate.z == 0.0f)
			{
				mRenderText.drawText3D(text.mText.c_str(), pos, text.mColor, mScene.getPVWMat(), mScene.getElapsedTime(), true);
			}
			else
			{
				glm::vec3 axis(text.mRotate.x?1.0f:0.0f, text.mRotate.y?1.0f:0.0f, text.mRotate.z?1.0f:0.0f);
				glm::mat4 m = mScene.getPVWMat() * glm::rotate(text.mRotate.y,axis);
				mRenderText.drawText3D(text.mText.c_str(), pos, text.mColor, m, mScene.getElapsedTime(), true);
			}
		}
	}
}












