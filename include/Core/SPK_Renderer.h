//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
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

namespace SPK
{
	class Particle;
	class Group;
	class Vector3D;

	enum BlendMode
	{
		BLEND_MODE_NONE,
		BLEND_MODE_ALPHA,
		BLEND_MODE_ADD,
	};

	enum RenderingOption
	{
		RENDERING_OPTION_ALPHA_TEST = 1 << 0,
		RENDERING_OPTION_DEPTH_WRITE = 1 << 1,
	};

	/** Constants to specify whether to compute stuff on GPU if possible */
	enum ShaderHint
	{
		SHADER_HINT_NONE,		/**< Dont use shaders */
		SHADER_HINT_VERTEX,		/**< Use vertex shader for computation when possible */
		SHADER_HINT_GEOMETRY,	/**< Use geometry shader for computation when possible (else fallback to vertex shader if possible) */
	};

	class SPK_PREFIX Renderer :	public SPKObject,
								public DataHandler
	{
	friend class Group;

	public :

		////////////////
		// Destructor //
		////////////////

		virtual ~Renderer() {}

		/////////////
		// Setters //
		/////////////

		/**
		* Specifies whether to use shader or not if possible
		* @param hint : the shader hint
		*/
		static void useShaderHint(ShaderHint hint);

		/**
		* Specifies whether to use vbo to transfer data to GPU if possible
		* @param hint : the vbo hint
		*/
		static void useVBOHint(bool hint);

		inline virtual void enableRenderingOption(RenderingOption option,bool enable);
		inline virtual void setAlphaTestThreshold(float alphaThreshold);
		inline void setActive(bool active);
		virtual void setBlendMode(BlendMode blendMode) = 0;
		
		/////////////
		// Getters //
		/////////////

		/**
		* Gets the shader hint
		* @return : the shader hint
		*/
		inline static ShaderHint getShaderHint();

		/**
		* Gets the vbo hint
		* @return : the vbo hint
		*/
		inline static bool getVBOHint();

		inline virtual bool isRenderingOptionEnabled(RenderingOption option) const;
		inline virtual float getAlphaTestThreshold() const;
		inline bool isActive() const;

	public : /// TODO: make this private
		inline void setRenderingOptions(unsigned int r);
		inline unsigned int getRenderingOptions() const;

	public :
		spark_description(Renderer, SPKObject)
		(
			spk_attribute(bool, active, setActive, isActive);
			//spk_attribute(float, alphaThreshold, setAlphaTestThreshold, getAlphaTestThreshold);
			spk_attribute(unsigned int, renderingOptions, setRenderingOptions, getRenderingOptions);
		);

	protected :

		/////////////////
		// Constructor //
		/////////////////

		Renderer(bool NEEDS_DATASET);

	private :
		// Rendering hints
		static ShaderHint shaderHint;
		static bool vboHint;

		bool active;

		int renderingOptionsMask;
		float alphaThreshold;

		inline virtual  RenderBuffer* attachRenderBuffer(const Group& group) const;

		virtual  void init(const Particle& particle,DataSet* dataSet) const {};
		virtual  void update(const Group& group,DataSet* dataSet) const {};

		virtual void render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const = 0;
		virtual void computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const = 0;
	};

	inline Renderer::Renderer(bool NEEDS_DATASET) :
		SPKObject(),
		DataHandler(NEEDS_DATASET),
		active(true),
		renderingOptionsMask(RENDERING_OPTION_DEPTH_WRITE),
		alphaThreshold(1.0f)
	{}

	inline void Renderer::setRenderingOptions(unsigned int r)
	{
		renderingOptionsMask = r;
	}

	inline unsigned int Renderer::getRenderingOptions() const
	{
		return renderingOptionsMask;
	}

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

	inline ShaderHint Renderer::getShaderHint()
	{
		return shaderHint;
	}

	inline bool Renderer::getVBOHint()
	{
		return vboHint;
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
