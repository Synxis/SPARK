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

#include "Rendering/OpenGL/SPK_GL_PointRenderer.h"
#include "Core/SPK_Logger.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_Vector3D.h"
#include "Core/SPK_Iterator.h"

namespace SPK
{
namespace GL
{
	bool GLPointRenderer::setType(PointType type)
	{
		if ((type == POINT_TYPE_SPRITE)&&(!loadGLExtPointSprite()))
		{
			SPK_LOG_WARNING("GLPointRenderer::setType(PointType) - POINT_TYPE_SPRITE is not available on the hardware");
			return false;
		}

		this->type = type;
		return true;
	}

	bool GLPointRenderer::enableWorldSize(bool worldSizeEnabled)
	{
		worldSize = (worldSizeEnabled && loadGLExtPointParameter());
		if (worldSize != worldSizeEnabled)
			SPK_LOG_WARNING("GLPointRenderer::enableWorldSize(true) - World size for points is not available on the hardware");
		return worldSize;
	}

	void GLPointRenderer::render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const
	{
		// Sets the different states for rendering
		initBlending();
		initRenderingOptions();

		switch(type)
		{
		case POINT_TYPE_SQUARE :
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_POINT_SMOOTH);
			if (getPointSpriteGLExt() == SUPPORTED)
				disablePointSpriteGLExt();
			break;

		case POINT_TYPE_SPRITE :
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,textureIndex);
			enablePointSpriteGLExt();
			break;

		case POINT_TYPE_CIRCLE :
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_POINT_SMOOTH);
			if (getPointSpriteGLExt() == SUPPORTED)
				disablePointSpriteGLExt();
			break;
		}

		if (worldSize)
			enablePointParameterGLExt(group.getRadius(),true);
		else
		{
			glPointSize(screenSize);

			if (getPointParameterGLExt() == SUPPORTED)
				disablePointParameterGLExt();
		}

		// Sends the data to the GPU
		// RenderBuffer is not used as the data are already well organized for rendering
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3,GL_FLOAT,0,group.getPositionAddress());
		glColorPointer(4,GL_UNSIGNED_BYTE,0,group.getColorAddress());

		glDrawArrays(GL_POINTS,0,group.getNbParticles());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	void GLPointRenderer::computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const
	{
		if (isWorldSizeEnabled())
		{
			float radius = group.getRadius();
			for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
			{
				AABBMin.setMin(particleIt->position() - radius);
				AABBMax.setMax(particleIt->position() + radius);
			}
		}
		else
			for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
			{
				AABBMin.setMin(particleIt->position());
				AABBMax.setMax(particleIt->position());
			}
	}
}}
