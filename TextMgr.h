/*
 * TextMgr.h
 *
 *  Created on: Mar 14, 2013
 *      Author: CodeArt
 */

#ifndef TEXTMGR_H_
#define TEXTMGR_H_


#include <GLES2/gl2.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "IRender.h"
#include "Shaders.h"

namespace MoGraph{


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
		TextMgr(Scene *scene);

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
}


#endif /* TEXTMGR_H_ */
