//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_RENDERER
#define H_SPK_RENDERER

#include "SPK_DEF.h"
#include "SPK_Registerable.h"
#include "SPK_DataHandler.h"

namespace SPK
{
	class Particle;
	class Group;
	class Vector3D;
	class RenderBuffer;

	enum Blending
	{
		BLENDING_NONE,
		BLENDING_ALPHA,
		BLENDING_ADD,
	};

	enum RenderingOption
	{
		RENDERING_OPTION_ALPHA_TEST = 1 << 0,
		RENDERING_OPTION_DEPTH_WRITE = 1 << 1,
	};

	class Renderer :	public Registerable,
						public DataHandler
	{
	friend class Group;

	public :

		////////////////
		// Destructor //
		////////////////

		virtual inline ~Renderer() {}

		/////////////
		// Setters //
		/////////////

		virtual inline void enableRenderingOption(RenderingOption option,bool enable);
		virtual inline void setAlphaTestThreshold(float alphaThreshold);
		inline void setActive(bool active);
		virtual void setBlending(Blending blendMode) = 0;
		
		/////////////
		// Getters //
		/////////////

		virtual inline bool isRenderingOptionEnabled(RenderingOption option) const;
		virtual inline float getAlphaTestThreshold() const;
		inline bool isActive() const;

	protected :

		/////////////////
		// Constructor //
		/////////////////

		inline Renderer(bool NEEDS_DATASET);

	private :

		bool active;

		int renderingOptionsMask;
		float alphaThreshold;

		virtual inline RenderBuffer* attachRenderBuffer(const Group& group) const;

		virtual inline void init(const Particle& particle,DataSet* dataSet) const {};
		virtual inline void update(const Group& group,DataSet* dataSet) const {};

		virtual void render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const = 0;
		virtual void computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const = 0;
	};

	inline Renderer::Renderer(bool NEEDS_DATASET) :
		Registerable(),
		DataHandler(NEEDS_DATASET),
		active(true),
		renderingOptionsMask(RENDERING_OPTION_DEPTH_WRITE),
		alphaThreshold(1.0f)
	{}

	inline void Renderer::enableRenderingOption(RenderingOption option,bool enable)
	{
		if (enable)
			renderingOptionsMask |= option;
		else
			renderingOptionsMask &= ~option;
	}

	inline void Renderer::setAlphaTestThreshold(float alphaThreshold)
	{
		this->alphaThreshold = alphaThreshold;
	}

	inline void Renderer::setActive(bool active)
	{
		this->active = active;
	}

	inline bool Renderer::isRenderingOptionEnabled(RenderingOption option) const
	{
		return (renderingOptionsMask & option) != 0;
	}

	inline float Renderer::getAlphaTestThreshold() const
	{
		return alphaThreshold;
	}

	inline bool Renderer::isActive() const
	{
		return active;
	}

	inline RenderBuffer* Renderer::attachRenderBuffer(const Group& group) const 
	{
		return NULL;
	}
}

#endif
