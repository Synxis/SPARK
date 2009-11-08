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


#include "RenderingAPIs/DX9/SPK_DX9LineRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"

#include "RenderingAPIs/DX9/SPK_DX9IndexBuffer.h"
#include "RenderingAPIs/DX9/SPK_DX9VertexBuffer.h"

namespace SPK
{
namespace DX9
{
	const std::string DX9LineRenderer::GPU_BUFFER_NAME("SPK_DX9LineRenderer_GPU");
	const std::string DX9LineRenderer::INDEX_BUFFER_NAME("SPK_DX9LineRenderer_Index");

	LPDIRECT3DVERTEXBUFFER9 DX9LineRenderer::gpuBuffer = NULL;
	LineVertex* DX9LineRenderer::gpuIterator = NULL;
	LPDIRECT3DINDEXBUFFER9 DX9LineRenderer::indexBuffer = NULL;
	short* DX9LineRenderer::indexIterator = NULL;

	DX9LineRenderer::DX9LineRenderer(float length, float width) :
		DX9Renderer(),
		length(length),
		width(width)
	{}

	bool DX9LineRenderer::checkBuffers(const Group& group)
	{
		if( reinit == true )
		{
			reinit = false;
			return false;
		}

		DX9VertexBuffer<LineVertex>* lvBuffer = NULL;
		if ((lvBuffer = dynamic_cast<DX9VertexBuffer<LineVertex>*>(group.getBuffer(GPU_BUFFER_NAME))) == NULL)
		{
			gpuBuffer = NULL;
			return false;
		}
		gpuBuffer = lvBuffer->getData();

		DX9IndexBuffer<short>* ibIndexBuffer = NULL;
		if ((ibIndexBuffer = dynamic_cast<DX9IndexBuffer<short>*>(group.getBuffer(INDEX_BUFFER_NAME))) == NULL)
		{
			ibIndexBuffer = NULL;
			return false;
		}
		indexBuffer = ibIndexBuffer->getData();

		return true;
	}

	void DX9LineRenderer::createBuffers(const Group& group)
	{	
		//DX9VertexBuffer<LineVertex>* lvBuffer = dynamic_cast<DX9VertexBuffer<LineVertex>*>(group.createBuffer(GPU_BUFFER_NAME,DX9VertexBufferCreator<LineVertex>((D3DFVF_XYZ|D3DFVF_DIFFUSE), 2),0,false));
		DX9VertexBuffer<LineVertex>* lvBuffer = dynamic_cast<DX9VertexBuffer<LineVertex>*>(group.createBuffer(GPU_BUFFER_NAME,DX9VertexBufferCreator<LineVertex>((D3DFVF_XYZ|D3DFVF_DIFFUSE), 4),0,false));
		gpuBuffer = lvBuffer->getData();

		DX9IndexBuffer<short>* ibIndexBuffer  = dynamic_cast<DX9IndexBuffer<short>*>(group.createBuffer(INDEX_BUFFER_NAME, DX9IndexBufferCreator<short>(D3DFMT_INDEX16, 6),0,false));
		indexBuffer = ibIndexBuffer->getData();

		int offsetIndex = 0;

		// initialisation de l'index buffer
		indexBuffer->Lock(0, group.getParticles().getNbReserved()*6*sizeof(short), (void**)&indexIterator, 0);

		for(size_t i = 0; i < group.getParticles().getNbReserved(); i++)
		{
//#define _DX9LINERENDERER_CLOCKWISE_
#ifdef _DX9LINERENDERER_CLOCKWISE_
			*(indexIterator++) = 0 + offsetIndex;
			*(indexIterator++) = 1 + offsetIndex;
			*(indexIterator++) = 2 + offsetIndex;
			*(indexIterator++) = 0 + offsetIndex;
			*(indexIterator++) = 2 + offsetIndex;
			*(indexIterator++) = 3 + offsetIndex;
#else
			*(indexIterator++) = 0 + offsetIndex;
			*(indexIterator++) = 2 + offsetIndex;
			*(indexIterator++) = 1 + offsetIndex;
			*(indexIterator++) = 0 + offsetIndex;
			*(indexIterator++) = 3 + offsetIndex;
			*(indexIterator++) = 2 + offsetIndex;
#endif
			offsetIndex += 4;
		}
		indexBuffer->Unlock();
		offsetIndex = 0;
	}

	void DX9LineRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(GPU_BUFFER_NAME);
		group.destroyBuffer(INDEX_BUFFER_NAME);
	}
/*
	void DX9LineRenderer::render(const Group& group)
	{
		HRESULT hr;
		
		if (!prepareBuffers(group))
			return;

		initBlending();

		if( group.getNbParticles() != 0 )
		{
			hr = gpuBuffer->Lock(0, group.getNbParticles()*2*sizeof(LineVertex), (void**)&gpuIterator, 0);

			for (size_t i = 0; i < group.getNbParticles(); ++i)
			{
				const Particle& particle = group.getParticle(i);

				D3DCOLOR c = D3DCOLOR_COLORVALUE(particle.getR(), particle.getG(), particle.getB(), particle.getParamCurrentValue(PARAM_ALPHA));

				Assign((gpuIterator)->position, particle.position());

				(gpuIterator++)->color = c;

				Assign((gpuIterator)->position, (particle.position() + particle.velocity() * length));

				(gpuIterator++)->color = c;
			}

			gpuBuffer->Unlock();

			DX9Info::getDevice()->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			DX9Info::getDevice()->SetStreamSource(0, gpuBuffer, 0, sizeof(LineVertex));
			DX9Info::getDevice()->DrawPrimitive(D3DPT_LINELIST, 0, group.getNbParticles());
		}
	}
*/
	void DX9LineRenderer::render(const Group& group)
	{
		HRESULT hr;

		if (!prepareBuffers(group))
			return;

		initBlending();

		D3DXMATRIX view;
		DX9Info::getDevice()->GetTransform(D3DTS_VIEW, &view);
		D3DXVECTOR3 r(-view._11, view._12, view._13);
		//D3DXVECTOR3 d(view._31, view._32, view._33);

		D3DXVECTOR3 right;
		//D3DXVECTOR3 direction;

		float w2 = this->getWidth() / 20.0f;

		if( group.getNbParticles() != 0 )
		{
			hr = gpuBuffer->Lock(0, group.getNbParticles()*4*sizeof(LineVertex), (void**)&gpuIterator, 0);

			for (size_t i = 0; i < group.getNbParticles(); ++i)
			{
				const Particle& particle = group.getParticle(i);

				D3DCOLOR c = D3DCOLOR_COLORVALUE(particle.getR(), particle.getG(), particle.getB(), particle.getParamCurrentValue(PARAM_ALPHA));

				D3DXVECTOR3 vel;
				Assign(vel, particle.velocity());

				D3DXVec3ProjectNormal(&right, &r, &vel);

				Assign((gpuIterator)->position, particle.position());
				(gpuIterator)->position += right * w2;
				(gpuIterator++)->color = c;

				Assign((gpuIterator)->position, particle.position());
				(gpuIterator)->position -= right * w2;
				(gpuIterator++)->color = c;

				Assign((gpuIterator)->position, (particle.position() + particle.velocity() * length));
				(gpuIterator)->position -= right * w2;
				(gpuIterator++)->color = c;

				Assign((gpuIterator)->position, (particle.position() + particle.velocity() * length));
				(gpuIterator)->position += right * w2;
				(gpuIterator++)->color = c;
			}

			gpuBuffer->Unlock();

			DX9Info::getDevice()->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			DX9Info::getDevice()->SetStreamSource(0, gpuBuffer, 0, sizeof(LineVertex));
			DX9Info::getDevice()->SetIndices(indexBuffer);
			DX9Info::getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, group.getNbParticles()*4, 0, group.getNbParticles()*2);
		}
	}

	HRESULT DX9LineRenderer::OnD3D9CreateDevice()
	{
		return S_OK;
	}
}}
