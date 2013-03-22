/*
 * TextMgr.cpp
 *
 *  Created on: Mar 14, 2013
 *      Author: CodeArt
 */

#include "graph.h"
#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <mavsprintf.h>

namespace MoGraph
{

	const char *getFormatDecimalStr(int d)
	{
		switch (d)
		{
			case 0:	return "%.0f";
			case 1:	return "%.1f";
			case 2:	return "%.2f";
			case 3:	return "%.3f";
			case 4:	return "%.4f";
			case 5:	return "%.5f";
			default: return "%.0f";
		}
		return "%.0f";
	}

	/**
	 * \brief TextMgr::draw() OBSOLETE..
	 * \note not in use due to RenderText class used by Graph does this job
	 */
	void TextMgr::draw()
	{

	}

	/**
	 * \brief TextMgr::init,	initiate all text in 3D graph
	 * \note there is defaulted text array of 3 items
	 * description of "X-Axis" "Y-AXIS" ("Y-AXIS" optional)
	 * if needed more text just push_back new text entries for mText
	 * and fill out the struct as desired
	 */
	void TextMgr::init()
	{
		// to do, align text with axis..
		const GraphDesc &desc = mScene->getGraphDesc();

		Text t;
		t.mRotate 			= glm::vec3(0.0f,0.0f,0.0f);
		t.mTextFlag			= Text::NO_ACTION;

//		float gridX 		= mScene->getGridX();
		float gridZ 		= mScene->getGridZ();
		float centerX		= mScene->getCx();
		float centerZ		= mScene->getCz();
		float orgScale 		= mScene->getGridX()/500.0f;
		float scale			= orgScale;
		float bound			= mScene->getBoundScale();
		float dcenterX		= centerX*bound;
		float dcenterZ		= centerZ*bound;
		bool bMirror		= (desc.bNegGridLines != 0);
		glm::vec2 scaleXZ(scale,scale);
		glm::vec3 pos(dcenterX, 0.0f,dcenterZ);
		glm::vec4 color(1.0f,1.0f,1.0f,1.0f);

		t.mColor 	= color;
		t.mText		= "MoSync 3D Graph Library 0.8 Beta";		// Subtitle
		t.mScale	= scaleXZ;
		t.mPos		= pos;
		if (bMirror)
		{
			t.mPos.y	= -(desc.gridYLines * desc.gridStepYLines);
			mTextArray.push_back(t);
			t.mPos.y	= pos.y;	// restore
		}
		else
		{
			mTextArray.push_back(t);
		}

		scaleXZ.x 	*= 0.8f;
		scaleXZ.y   *= 0.8f;

		float totGridHeight = desc.gridStepYLines * desc.gridYLines + desc.gridStepYLines;

		t.mPos.y	+= totGridHeight;
		t.mText		= "Y-Axis";		// Subtitle
		t.mScale	= scaleXZ;
		mTextArray.push_back(t);

		t.mText		= "X-Axis";		// Subtitle
		t.mPos 		= glm::vec3(-dcenterX, 0.0f,dcenterZ);
		t.mTextFlag = Text::CENTER_RIGHT;
		mTextArray.push_back(t);


		if (desc.bUseGridValue)
		{
			char buf[64];
			const char *formatDecimals = getFormatDecimalStr(desc.gridDecimals);
			float step 			= desc.gridStepValue;
			float value 		= 0.0f;
			float yPosOffset 	= desc.gridStepYLines*0.5f;
			float sc 			= orgScale * desc.gridStepYLines * 0.75f;
			int	i 				= 0;

			if (bMirror)
			{
				value 		= -step*desc.gridYLines+step;
				yPosOffset 	= -desc.gridStepYLines*desc.gridYLines+desc.gridStepYLines*0.75f;
				i			= -desc.gridYLines+1;
			}

			glm::vec3 pos(dcenterX*1.01f, yPosOffset, dcenterZ*1.01f);
			t.mPos 		= pos;
			t.mScale 	= glm::vec2(sc,sc);

			for(; i<desc.gridYLines; i++)
			{
				// let user provide formatting string.
				sprintf(buf,formatDecimals,value);

				t.mText 	= buf;
//				lprintfln("Bar Value %f=>%d and %s => %s" ,value,(int)value,formatDecimals,buf);   			// error not declared.

				t.mPos.y 	+= desc.gridStepYLines;
				t.mTextFlag = Text::CENTER_RIGHT;
				mTextArray.push_back(t);
				value 		+= step;
			}
		}


		if (gridZ > 1)		// Check if there is a z-Axis at all
		{
			// set up text for Z-Axis
			t.mScale	= scaleXZ;
			t.mTextFlag = Text::NO_ACTION;
			t.mText 	= "Z-axis";
			t.mPos 		= glm::vec3(-dcenterZ, 0.0f,-dcenterX);
			t.mRotate	= glm::vec3(0.0f,-90.0f,0.0f);
			mTextArray.push_back(t);
		}

	}

	/**
	 * TextMgr Constructor
	 * @param scene adding the scene class for back ref this obj.
	 */
	TextMgr::TextMgr(Scene *scene) : Render(scene)
	{
	}

}


