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


#include "RenderingAPIs/DX9/SPK_DX9LineTrailRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_ArrayBuffer.h"

namespace SPK
{
namespace DX9
{
	const std::string DX9LineTrailRenderer::VERTEX_BUFFER_NAME("SPK_DX9LineTrailRenderer_Vertex");
	const std::string DX9LineTrailRenderer::COLOR_BUFFER_NAME("SPK_DX9LineTrailRenderer_Color");
	const std::string DX9LineTrailRenderer::VALUE_BUFFER_NAME("SPK_DX9LineTrailRenderer_Value");

	D3DXVECTOR3* DX9LineTrailRenderer::vertexBuffer = NULL;
	D3DXVECTOR3* DX9LineTrailRenderer::vertexIterator = NULL;
	D3DCOLOR* DX9LineTrailRenderer::colorBuffer = NULL;
	D3DCOLOR* DX9LineTrailRenderer::colorIterator = NULL;
	float* DX9LineTrailRenderer::valueBuffer = NULL;
	float* DX9LineTrailRenderer::valueIterator = NULL;

	LPDIRECT3DVERTEXBUFFER9 DX9LineTrailRenderer::DX9VertexBuffer = NULL;
	LPDIRECT3DVERTEXBUFFER9 DX9LineTrailRenderer::DX9ColorBuffer = NULL;

	LPDIRECT3DVERTEXDECLARATION9 DX9LineTrailRenderer::pVertexDecl = NULL;

	DX9LineTrailRenderer::DX9LineTrailRenderer() :
		DX9Renderer(),
		nbSamples(8),
		width(1.0f),
		duration(1.0f),
		degeneratedR(0.0f),
		degeneratedG(0.0f),
		degeneratedB(0.0f),
		degeneratedA(0.0f)
	{
		enableBlending(true);	
	}

	DX9LineTrailRenderer::~DX9LineTrailRenderer()
	{
		SAFE_RELEASE( pVertexDecl );
	}

	void DX9LineTrailRenderer::setDegeneratedLines(float r,float g,float b,float a)
	{
		degeneratedR = r;
		degeneratedG = g;
		degeneratedB = b;
		degeneratedA = a;
	}

	bool DX9LineTrailRenderer::checkBuffers(const Group& group)
	{
		ArrayBuffer<D3DXVECTOR3>* fVertexBuffer = NULL;
		ArrayBuffer<D3DCOLOR>* fColorBuffer = NULL;
		FloatBuffer* fValueBuffer = NULL;

		if ((fVertexBuffer = dynamic_cast<ArrayBuffer<D3DXVECTOR3>*>(group.getBuffer(VERTEX_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		if ((fColorBuffer = dynamic_cast<ArrayBuffer<D3DCOLOR>*>(group.getBuffer(COLOR_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		if ((fValueBuffer = dynamic_cast<FloatBuffer*>(group.getBuffer(VALUE_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		vertexIterator = vertexBuffer = fVertexBuffer->getData();
		colorIterator = colorBuffer = fColorBuffer->getData();
		valueIterator = valueBuffer = fValueBuffer->getData();

		return true;
	}

	void DX9LineTrailRenderer::createBuffers(const Group& group)
	{
		DX9Info::getDevice()->CreateVertexDeclaration(LineTrailVertexDecl, &pVertexDecl);

		ArrayBuffer<D3DXVECTOR3>* fVertexBuffer = dynamic_cast<ArrayBuffer<D3DXVECTOR3>*>(group.createBuffer(VERTEX_BUFFER_NAME,ArrayBufferCreator<D3DXVECTOR3>(nbSamples + 2),nbSamples,true));
		ArrayBuffer<D3DCOLOR>* fColorBuffer = dynamic_cast<ArrayBuffer<D3DCOLOR>*>(group.createBuffer(COLOR_BUFFER_NAME,ArrayBufferCreator<D3DCOLOR>(nbSamples + 2),nbSamples,true));
		FloatBuffer* fValueBuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(VALUE_BUFFER_NAME,FloatBufferCreator(nbSamples),nbSamples,true));

		vertexIterator = vertexBuffer = fVertexBuffer->getData();
		colorIterator = colorBuffer = fColorBuffer->getData();
		valueIterator = valueBuffer = fValueBuffer->getData();

		// Fills the buffers with correct values
		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			const Particle& particle = group.getParticle(i);
			init(particle,particle.getAge());
		}

		// Resets the iterators at the beginning after the init
		vertexIterator = vertexBuffer;
		colorIterator = colorBuffer;
		valueIterator = valueBuffer;
	}

	void DX9LineTrailRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(VERTEX_BUFFER_NAME);
		group.destroyBuffer(COLOR_BUFFER_NAME);
		group.destroyBuffer(VALUE_BUFFER_NAME);
	}

	void DX9LineTrailRenderer::init(const Group& group)
	{
		if (!prepareBuffers(group))
			return;

		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			const Particle& particle = group.getParticle(i);
			init(particle,particle.getAge());
		}
	}

	void DX9LineTrailRenderer::render(const Group& group)
	{
		if( !DX9PrepareBuffers(group) )
			return;

		if( !prepareBuffers(group) )
			return;

		initBlending();
		initRenderingHints();

		//if( group.getNbParticles() == 0 ) return;

		for(size_t i = 0; i < group.getNbParticles(); ++i)
		{
			const Particle& particle = group.getParticle(i);
			float age = particle.getAge();

			if ((age == 0.0f)||(age < valueIterator[nbSamples - 1])) // If the particle is new, buffers for it are reinitialized
				init(particle,0.0f);
			else
			{
				// skips pre degenerated vertex
				vertexIterator += 1; 
				colorIterator += 1;

				int count = 0;

				// Counts the number of end points to update
				while((age - valueIterator[count] > duration)&&(count < static_cast<int>(nbSamples) - 1))
					++count;

				if (count > 0)
				{
					--count;

					float ratio1 = (age - duration - valueIterator[count]) / (valueIterator[count + 1] - valueIterator[count]);
					float ratio0 = 1.0f - ratio1;

					int vertexDelta = count;
					int colorDelta = count;

					// Interpolates
					vertexIterator[vertexDelta] *= ratio0;
					vertexIterator[vertexDelta] += vertexIterator[vertexDelta + 1] * ratio1;

					D3DXCOLOR c0(colorIterator[colorDelta]);
					D3DXCOLOR c1(colorIterator[colorDelta + 1]);
					c0 *= ratio0;
					c1 *= ratio1;
					c0 += c1;
					colorIterator[colorDelta] = c0;

					valueIterator[count] = age - duration;

					if (count > 0) // shifts the data by one
					{
						memmove(vertexIterator + vertexDelta - 1, vertexIterator + vertexDelta, (nbSamples - count) * sizeof(D3DXVECTOR3));
						memmove(colorIterator + colorDelta - 1, colorIterator + colorDelta, (nbSamples - count) * sizeof(D3DCOLOR));
						memmove(valueIterator + count - 1, valueIterator + count, (nbSamples - count) * sizeof(float));

						--count;
						if (count > 0) // confounds points (not supposed to happen often)
							for (size_t i = 0; i < static_cast<size_t>(count); ++i)
							{
								memcpy(vertexIterator + i, vertexIterator + count, sizeof(D3DXVECTOR3));
								memcpy(colorIterator + i, colorIterator + count, sizeof(D3DCOLOR));
								memcpy(valueIterator + i, valueIterator + count, sizeof(float));
							}
					}
				}

				for (size_t i = 0; i < nbSamples - 1; ++i)
				{
					D3DXCOLOR c(colorIterator[i]);
					c.a *= 1.0f - (age - valueIterator[nbSamples - 1]) / (valueIterator[i] + duration - valueIterator[nbSamples - 1]);
					//c.a *= 2.0f;
					//c.a = (c.a > 1.0f) ? (1.0f) : (c.a);
					colorIterator[i] = c;
				}

				// pre degenerated vertex
				memcpy(vertexIterator - 1, vertexIterator, sizeof(D3DXVECTOR3));
				
				// Positions pseudo iterators to the current point
				vertexIterator += (nbSamples - 1);
				colorIterator += (nbSamples - 1);
				valueIterator += nbSamples - 1;

				// Updates renderers's data with the particle
				Assign(*(vertexIterator++), particle.position());
				vertexIterator += 1;	// skips post degenerated vertex

				*(colorIterator++) = D3DCOLOR_COLORVALUE(particle.getR(), particle.getG(), particle.getB(), particle.getParamCurrentValue(PARAM_ALPHA));
				colorIterator += 1;	// skips post degenerated vertex

				*(valueIterator++) = age;

				// post degenerated vertex
				memcpy(vertexIterator - 1, vertexIterator - 2, sizeof(D3DXVECTOR3));
			}
		}

		void *ptr;

		DX9VertexBuffer->Lock(0, 0, &ptr, 0);
		memcpy(ptr, vertexBuffer, group.getNbParticles() * (nbSamples + 2) * sizeof(D3DXVECTOR3));
		DX9VertexBuffer->Unlock();

		DX9ColorBuffer->Lock(0, 0, &ptr, 0);
		memcpy(ptr, colorBuffer, group.getNbParticles() * (nbSamples + 2) * sizeof(D3DCOLOR));
		DX9ColorBuffer->Unlock();

		LPDIRECT3DDEVICE9 device = DX9Info::getDevice();
		device->SetVertexDeclaration(pVertexDecl);
		device->SetStreamSource(0, DX9VertexBuffer, 0, sizeof(D3DXVECTOR3));
		device->SetStreamSource(1, DX9ColorBuffer, 0, sizeof(D3DCOLOR));
		device->DrawPrimitive(D3DPT_LINESTRIP, 0, group.getNbParticles() * (nbSamples + 2));
	}

	bool DX9LineTrailRenderer::DX9CheckBuffers(const Group& group)
	{
		if( !DX9Bind(group, DX9_VERTEX_BUFFER_KEY, (void**)&DX9VertexBuffer) )
		{
			DX9VertexBuffer = DX9ColorBuffer = NULL;
			return false;
		}
		if( !DX9Bind(group, DX9_COLOR_BUFFER_KEY, (void**)&DX9ColorBuffer) )
		{
			DX9VertexBuffer = DX9ColorBuffer = NULL;
			return false;
		}

		return true;
	}

	bool DX9LineTrailRenderer::DX9CreateBuffers(const Group& group)
	{
		if( DX9Info::getDevice() == NULL ) return false;

		if( DX9Buffers.size() == 0 )
		{
			SAFE_RELEASE( pVertexDecl );
			DX9Info::getDevice()->CreateVertexDeclaration(LineTrailVertexDecl, &pVertexDecl);
		}

		LPDIRECT3DVERTEXBUFFER9 vb;

		if( DX9Info::getDevice()->CreateVertexBuffer(group.getParticles().getNbReserved() * (nbSamples + 2) * sizeof(D3DXVECTOR3), 0, D3DFVF_XYZ, D3DPOOL_DEFAULT, &vb, NULL) != S_OK ) return false;
		std::pair<const Group *, int> key(&group, DX9_VERTEX_BUFFER_KEY);
		DX9Buffers[key] = vb;

		DX9VertexBuffer = vb;

		if( DX9Info::getDevice()->CreateVertexBuffer(group.getParticles().getNbReserved() * (nbSamples + 2) * sizeof(D3DCOLOR), 0, D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vb, NULL) != S_OK ) return false;
		key = std::pair<const Group *, int>(&group, DX9_COLOR_BUFFER_KEY);
		DX9Buffers[key] = vb;

		DX9ColorBuffer = vb;

		return true;
	}

	bool DX9LineTrailRenderer::DX9DestroyBuffers(const Group& group)
	{
		DX9Release(group, DX9_VERTEX_BUFFER_KEY);
		DX9Release(group, DX9_COLOR_BUFFER_KEY);

		DX9VertexBuffer = DX9ColorBuffer = NULL;

		return true;
	}
}}
