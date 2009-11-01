//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////


#include "RenderingAPIs/OpenGL/SPK_GLRenderer.h"

namespace SPK
{
namespace GL
{
	GLint GLRenderer::statesSaved = -1;

	GLRenderer::GLRenderer() :
		Renderer(),
		blendingEnabled(false),
		srcBlending(GL_SRC_ALPHA),
		destBlending(GL_ONE_MINUS_SRC_ALPHA),
		textureBlending(GL_MODULATE)
	{}

	GLRenderer::~GLRenderer() {}

	void GLRenderer::setBlending(BlendingMode blendMode)
	{
		switch(blendMode)
		{
		case BLENDING_NONE :
			srcBlending = GL_ONE;
			destBlending = GL_ZERO;
			blendingEnabled = false;
			break;

		case BLENDING_ADD :
			srcBlending = GL_SRC_ALPHA;
			destBlending = GL_ONE;
			blendingEnabled = true;
			break;

		case BLENDING_ALPHA :
			srcBlending = GL_SRC_ALPHA;
			destBlending = GL_ONE_MINUS_SRC_ALPHA;
			blendingEnabled = true;
			break;
		}
	}

	void GLRenderer::saveGLStates()
	{
		// If some states are already saved, we restore the states
		if (statesSaved != -1)
			restoreGLStates();

		GLint currentDepth;
		GLint maxDepth;
		glGetIntegerv(GL_ATTRIB_STACK_DEPTH,&currentDepth);
		glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH,&maxDepth);

		// If the attrib stack is full we return
		if (currentDepth == maxDepth)
			return;

		glPushAttrib(GL_POINT_BIT |
			GL_LINE_BIT |
			GL_ENABLE_BIT |
			GL_COLOR_BUFFER_BIT |
			GL_CURRENT_BIT |
			GL_TEXTURE_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_LIGHTING_BIT |
			GL_POLYGON_BIT);

		statesSaved = currentDepth;
	}

	void GLRenderer::restoreGLStates()
	{
		if (statesSaved == -1)
			return;

		GLint currentDepth;
		glGetIntegerv(GL_ATTRIB_STACK_DEPTH,&currentDepth);

		// Pops until we reach the saved depth
		while (currentDepth > statesSaved)
		{
			glPopAttrib();
			--currentDepth;
		}

		statesSaved = -1;
	}
}}
