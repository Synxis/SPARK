//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//                           matthieu foulon - stardeath@wanadoo.fr				//
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


#ifndef H_SPK_DX9LINETRAILRENDERER
#define H_SPK_DX9LINETRAILRENDERER

#include "RenderingAPIs/DX9/SPK_DX9Renderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Model.h"


namespace SPK
{
namespace DX9
{
	const D3DVERTEXELEMENT9 LineTrailVertexDecl[3] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};
	
	class SPK_DX9_PREFIX DX9LineTrailRenderer : public DX9Renderer
	{
		SPK_IMPLEMENT_REGISTERABLE(DX9LineTrailRenderer)

	public :

		DX9LineTrailRenderer();

		virtual ~DX9LineTrailRenderer();

		static DX9LineTrailRenderer* create();


		void setNbSamples(size_t nbSamples);

		void setWidth(float width);

		void setDuration(float duration);

		void setDegeneratedLines(float r,float g,float b,float a);

		virtual void enableBlending(bool blendingEnabled);

		size_t getNbSamples() const;

		float getWidth() const;

		float getDuration() const;

		virtual void createBuffers(const Group& group);
		virtual void destroyBuffers(const Group& group);

		virtual bool DX9CreateBuffers(const Group& group);
		virtual bool DX9DestroyBuffers(const Group& group);

		void init(const Group& group);

		virtual void render(const Group& group);

	protected :

		virtual bool checkBuffers(const Group& group);

		virtual bool DX9CheckBuffers(const Group& group);

	private :

		size_t nbSamples;

		float width;
		float duration;

		float degeneratedR;
		float degeneratedG;
		float degeneratedB;
		float degeneratedA;

		// vertex buffers and iterators
		static D3DXVECTOR3* vertexBuffer;
		static D3DXVECTOR3* vertexIterator;
		static D3DCOLOR* colorBuffer;
		static D3DCOLOR* colorIterator;
		static float* valueBuffer;
		static float* valueIterator;

		static LPDIRECT3DVERTEXBUFFER9 DX9VertexBuffer;
		static LPDIRECT3DVERTEXBUFFER9 DX9ColorBuffer;

		// buffers names
		static const std::string VERTEX_BUFFER_NAME;
		static const std::string COLOR_BUFFER_NAME;
		static const std::string VALUE_BUFFER_NAME;

		// vertex declaration
		static LPDIRECT3DVERTEXDECLARATION9 pVertexDecl;

		void init(const Particle& particle,float age) const;
	};


	inline DX9LineTrailRenderer* DX9LineTrailRenderer::create()
	{
		DX9LineTrailRenderer* obj = new DX9LineTrailRenderer;
		registerObject(obj);
		DX9Info::DX9RegisterRenderer(obj);
		return obj;
	}
	
	inline void DX9LineTrailRenderer::enableBlending(bool blendingEnabled)
	{
		DX9Renderer::enableBlending(true);
	}

	inline void DX9LineTrailRenderer::setNbSamples(size_t nbSamples)
	{
		this->nbSamples = nbSamples;
	}

	inline void DX9LineTrailRenderer::setWidth(float width)
	{
		this->width = width;
	}

	inline void DX9LineTrailRenderer::setDuration(float duration)
	{
		this->duration = duration;
	}

	inline size_t DX9LineTrailRenderer::getNbSamples() const
	{
		return nbSamples;
	}

	inline float DX9LineTrailRenderer::getWidth() const
	{
		return width;
	}

	inline float DX9LineTrailRenderer::getDuration() const
	{
		return duration;
	}

	inline void DX9LineTrailRenderer::init(const Particle& particle,float age) const
	{
		// Gets the particle's values
		const Vector3D& pos = particle.position();
		float r = particle.getR();
		float g = particle.getG();
		float b = particle.getB();
		float a = particle.getParamCurrentValue(PARAM_ALPHA);

		// Inits position
		for (size_t i = 0; i < nbSamples + 2; ++i)
		{
			*(vertexIterator++) = D3DXVECTOR3(pos.x, pos.y, pos.z);
		}
		
		// Inits color
		// degenerate pre vertex
		*(colorIterator++) = D3DCOLOR_COLORVALUE(degeneratedR, degeneratedG, degeneratedB, degeneratedA);

		for (size_t i = 0; i < nbSamples; ++i)
		{
			*(colorIterator++) = D3DCOLOR_COLORVALUE(r, g, b, a);
		}

		// degenerate post vertex
		*(colorIterator++) = D3DCOLOR_COLORVALUE(degeneratedR, degeneratedG, degeneratedB, degeneratedA);

		// Inits age
		for (size_t i = 0; i < nbSamples; ++i)
			*(valueIterator++) = duration * (i / static_cast<float>(nbSamples - 1) - 1.0f) + age;
	}
}}

#endif
