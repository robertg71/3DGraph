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


	/**
	 * Graph::draw,	render whole scene.
	 */
	void Graph::draw()
	{
		static int cnt = 0;
		cnt++;

		glViewport((int)mViewPos.x, (int)mViewPos.y, mWidth, mHeight);
			checkGLError("glViewport");
//		lprintfln("%d. draw()::glViewport w=%d h=%d\n",cnt,mWidth,mHeight);
		mDeltaTime.tick();

		float tick = static_cast<float>(mTime.update()) * 0.001f;
		mScene.setElapsedTime(tick);

		// Clear the color buffer
		glClearColor(mBGColor.r,mBGColor.g,mBGColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGLError("glClear");

		mTouch.update();
		mScene.updateCamera(mTouch.getScale());

		const glm::vec2 rotPos = mTouch.getAngularOrientation();
		glm::vec3 axisY(0.0,1.0f,0.0f);
		glm::mat4 mY = glm::rotate(rotPos.x*30.0f,axisY);
		glm::vec3 axisX(1.0,0.0f,0.0f);
		glm::mat4 mX = glm::rotate(rotPos.y*30.0f,axisX);
		glm::mat4 m = mY*mX;
		mScene.setWorldMat( m );
		mScene.updateMatrix();		// need to update the PVW Matrix, Projection * View * World.

		drawBars(tick);
		drawAxis(tick);
		drawText(tick);

	}

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
		mTouch.init(mWidth,mHeight);
		return initGL();
	}

	/**
	 * Graph::initGL,	initiates rendering system for the graph
	 * @return TRUE, TODO fix error check
	 */
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
		glClearColor(mBGColor.r,mBGColor.g,mBGColor.b, 1.0f);
		mRenderText.init(mWidth,mHeight,mFont);

		// create a braph with grid times grid
		initShaderBars();
		initShaderLines();
		initShaderText();

		return TRUE;
	}

/**
 * \brief Graph::drawText,	Text rendering for the Graph using TextMgr
 * @param tick, 	elapsed time since start in ms
 */
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












