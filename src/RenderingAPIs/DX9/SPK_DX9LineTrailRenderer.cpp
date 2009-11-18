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

	LPDIRECT3DVERTEXBUFFER9 DX9LineTrailRenderer::vertexBuffer = NULL;
	D3DXVECTOR3* DX9LineTrailRenderer::vertexIterator = NULL;
	LPDIRECT3DVERTEXBUFFER9 DX9LineTrailRenderer::colorBuffer = NULL;
	D3DCOLOR* DX9LineTrailRenderer::colorIterator = NULL;
	float* DX9LineTrailRenderer::valueBuffer = NULL;
	float* DX9LineTrailRenderer::valueIterator = NULL;

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

	void DX9LineTrailRenderer::setDegeneratedLines(float r,float g,float b,float a)
	{
		degeneratedR = r;
		degeneratedG = g;
		degeneratedB = b;
		degeneratedA = a;
	}

	bool DX9LineTrailRenderer::checkBuffers(const Group& group)
	{
		DX9VertexBuffer<D3DXVECTOR3>* fVertexBuffer;
		DX9VertexBuffer<D3DCOLOR>* fColorBuffer;
		FloatBuffer* fValueBuffer;

		if ((fVertexBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.getBuffer(VERTEX_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		if ((fColorBuffer = dynamic_cast<DX9VertexBuffer<D3DCOLOR>*>(group.getBuffer(COLOR_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		if ((fValueBuffer = dynamic_cast<FloatBuffer*>(group.getBuffer(VALUE_BUFFER_NAME,nbSamples))) == NULL)
			return false;

		vertexBuffer = fVertexBuffer->getData();
		colorBuffer = fColorBuffer->getData();
		valueIterator = valueBuffer = fValueBuffer->getData();

		return true;
	}

	void DX9LineTrailRenderer::createBuffers(const Group& group)
	{	
		DX9VertexBuffer<D3DXVECTOR3>* fVertexBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.createBuffer(VERTEX_BUFFER_NAME,DX9VertexBufferCreator<D3DXVECTOR3>(D3DFVF_XYZ, (nbSamples + 2)),nbSamples,true));
		DX9VertexBuffer<D3DCOLOR>* fColorBuffer = dynamic_cast<DX9VertexBuffer<D3DCOLOR>*>(group.createBuffer(COLOR_BUFFER_NAME,DX9VertexBufferCreator<D3DCOLOR>(D3DFVF_DIFFUSE, (nbSamples + 2)),nbSamples,true));
		FloatBuffer* fValueBuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(VALUE_BUFFER_NAME,FloatBufferCreator(nbSamples),nbSamples,true));

		vertexBuffer = fVertexBuffer->getData();
		colorBuffer = fColorBuffer->getData();
		valueIterator = valueBuffer = fValueBuffer->getData();

		// Fills the buffers with correct values
		vertexBuffer->Lock(0, 0, (void**)&vertexIterator, 0);
		colorBuffer->Lock(0, 0, (void**)&colorIterator, 0);
		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			const Particle& particle = group.getParticle(i);
			init(particle,particle.getAge());
		}
		vertexBuffer->Unlock();
		colorBuffer->Unlock();

		// Resets the iterators at the beginning after the init
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

		vertexBuffer->Lock(0, 0, (void**)&vertexIterator, 0);
		colorBuffer->Lock(0, 0, (void**)&colorIterator, 0);
		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			const Particle& particle = group.getParticle(i);
			init(particle,particle.getAge());
		}
		vertexBuffer->Unlock();
		colorBuffer->Unlock();
	}

	void DX9LineTrailRenderer::render(const Group& group)
	{	
		if (!prepareBuffers(group))
			return;

		initBlending();
		initRenderingHints();

		vertexBuffer->Lock(0, 0, (void**)&vertexIterator, 0);
		colorBuffer->Lock(0, 0, (void**)&colorIterator, 0);
		for (size_t i = 0; i < group.getNbParticles(); ++i)
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
		vertexBuffer->Unlock();
		colorBuffer->Unlock();

		LPDIRECT3DDEVICE9 device = DX9Info::getDevice();
		device->SetVertexDeclaration(pVertexDecl);
		device->SetStreamSource(0, vertexBuffer, 0, sizeof(D3DXVECTOR3));
		device->SetStreamSource(1, colorBuffer, 0, sizeof(D3DCOLOR));
		device->DrawPrimitive(D3DPT_LINESTRIP, 0, group.getNbParticles() * (nbSamples + 2));
	}

	HRESULT DX9LineTrailRenderer::OnD3D9CreateDevice()
	{
		DX9Info::getDevice()->CreateVertexDeclaration(LineTrailVertexDecl, &pVertexDecl);

		return S_OK;
	}
}}
