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


#include "RenderingAPIs/DX9/SPK_DX9QuadRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"

#include "RenderingAPIs/DX9/SPK_DX9IndexBuffer.h"
#include "RenderingAPIs/DX9/SPK_DX9VertexBuffer.h"

namespace SPK
{
namespace DX9
{
	const std::string DX9QuadRenderer::VERTEX_BUFFER_NAME("SPK_DX9QuadRenderer_Vertex");
	const std::string DX9QuadRenderer::COLOR_BUFFER_NAME("SPK_DX9QuadRenderer_Color");
	const std::string DX9QuadRenderer::TEXTURE_BUFFER_NAME("SPK_DX9QuadRenderer_Texture");
	const std::string DX9QuadRenderer::INDEX_BUFFER_NAME("SPK_DX9QuadRenderer_Index");

	LPDIRECT3DVERTEXBUFFER9 DX9QuadRenderer::vertexBuffer = NULL;
	D3DXVECTOR3* DX9QuadRenderer::vertexIterator = NULL;
	LPDIRECT3DVERTEXBUFFER9 DX9QuadRenderer::colorBuffer = NULL;
	DWORD* DX9QuadRenderer::colorIterator = NULL;
	LPDIRECT3DVERTEXBUFFER9 DX9QuadRenderer::textureBuffer = NULL;
	float* DX9QuadRenderer::textureIterator = NULL;
	LPDIRECT3DINDEXBUFFER9 DX9QuadRenderer::indexBuffer = NULL;
	short* DX9QuadRenderer::indexIterator = NULL;

	int DX9QuadRenderer::offsetIndex = 0;

	LPDIRECT3DVERTEXDECLARATION9 DX9QuadRenderer::pVertexDecl = NULL;
	LPDIRECT3DVERTEXDECLARATION9 DX9QuadRenderer::pVertexDecl2D = NULL;
	LPDIRECT3DVERTEXDECLARATION9 DX9QuadRenderer::pVertexDecl3D = NULL;

	void (DX9QuadRenderer::*DX9QuadRenderer::renderParticle)(const Particle&) const = NULL;

	DX9QuadRenderer::DX9QuadRenderer(float scaleX,float scaleY) :
		DX9Renderer(),
		QuadRendererInterface(scaleX,scaleY),
		Oriented3DRendererInterface(),
		textureIndex(0)
	{}

	DX9QuadRenderer::~DX9QuadRenderer()
	{
		SAFE_RELEASE( pVertexDecl );
		SAFE_RELEASE( pVertexDecl2D );
		SAFE_RELEASE( pVertexDecl3D );
	}

	bool DX9QuadRenderer::checkBuffers(const Group& group)
	{
		// if device reseted or destroyed, send message to recreate buffers
		if( reinit == true )
		{
			reinit = false;
			return false;
		}

		DX9VertexBuffer<D3DXVECTOR3>* pvbBuffer = NULL;
		DX9VertexBuffer<DWORD>* dwColorBuffer = NULL;
		DX9IndexBuffer<short>* sIndexBuffer = NULL;

		if ((pvbBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.getBuffer(VERTEX_BUFFER_NAME))) == NULL)
			return false;

		if ((dwColorBuffer = dynamic_cast<DX9VertexBuffer<DWORD>*>(group.getBuffer(COLOR_BUFFER_NAME))) == NULL)
			return false;

		if ((sIndexBuffer = dynamic_cast<DX9IndexBuffer<short>*>(group.getBuffer(INDEX_BUFFER_NAME))) == NULL)
			return false;

		switch( texturingMode )
		{
			case TEXTURE_2D:
				{
					DX9VertexBuffer<D3DXVECTOR2>* fTextureBuffer = NULL;
					if ((fTextureBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR2>*>(group.getBuffer(TEXTURE_BUFFER_NAME,texturingMode))) == NULL)
					{
						group.destroyBuffer(TEXTURE_BUFFER_NAME);
						textureBuffer = createTextureBuffer(group);
					}
					textureBuffer = (fTextureBuffer == NULL) ? (NULL) : (fTextureBuffer->getData());

				}
				break;

			case TEXTURE_3D:
				{
					DX9VertexBuffer<D3DXVECTOR3>* fTextureBuffer = NULL;
					if ((fTextureBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.getBuffer(TEXTURE_BUFFER_NAME,texturingMode))) == NULL)
					{
						group.destroyBuffer(TEXTURE_BUFFER_NAME);
						textureBuffer = createTextureBuffer(group);
					}
					textureBuffer = (fTextureBuffer == NULL) ? (NULL) : (fTextureBuffer->getData());
				}
				break;
		}

		vertexBuffer = pvbBuffer->getData();
		colorBuffer = dwColorBuffer->getData();
		indexBuffer = sIndexBuffer->getData();

		return true;
	}

	void DX9QuadRenderer::createBuffers(const Group& group)
	{
		DX9VertexBuffer<D3DXVECTOR3>* vbVertexBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.createBuffer(VERTEX_BUFFER_NAME, DX9VertexBufferCreator<D3DXVECTOR3>(D3DFVF_XYZ, 4),0,false));
		DX9VertexBuffer<DWORD>* vbColorBuffer = dynamic_cast<DX9VertexBuffer<DWORD>*>(group.createBuffer(COLOR_BUFFER_NAME, DX9VertexBufferCreator<DWORD>(D3DFVF_DIFFUSE, 4),0,false));
		DX9IndexBuffer<short>* ibIndexBuffer  = dynamic_cast<DX9IndexBuffer<short>*>(group.createBuffer(INDEX_BUFFER_NAME, DX9IndexBufferCreator<short>(D3DFMT_INDEX16, 6),0,false));
		vertexBuffer = vbVertexBuffer->getData();
		colorBuffer = vbColorBuffer->getData();
		indexBuffer = ibIndexBuffer->getData();
		textureBuffer = createTextureBuffer(group);

		offsetIndex = 0;

		// initialisation de l'index buffer
		indexBuffer->Lock(0, group.getParticles().getNbReserved()*6*sizeof(short), (void**)&indexIterator, 0);

		for(size_t i = 0; i < group.getParticles().getNbReserved(); i++)
		{
//#define _DX9QUADRENDERER_CLOCKWISE_
#ifdef _DX9QUADRENDERER_CLOCKWISE_
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

	void DX9QuadRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(VERTEX_BUFFER_NAME);
		group.destroyBuffer(COLOR_BUFFER_NAME);
		group.destroyBuffer(TEXTURE_BUFFER_NAME);
		group.destroyBuffer(INDEX_BUFFER_NAME);
		offsetIndex = 0;
	}

	LPDIRECT3DVERTEXBUFFER9 DX9QuadRenderer::createTextureBuffer(const Group& group) const
	{
		LPDIRECT3DVERTEXBUFFER9 texBuffer = NULL;
		void *ptr = NULL;

		switch(texturingMode)
		{
			case TEXTURE_2D :
				{
					DX9VertexBuffer<D3DXVECTOR2>* vbTextureBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR2>*>(group.createBuffer(TEXTURE_BUFFER_NAME, DX9VertexBufferCreator<D3DXVECTOR2>(D3DFVF_TEX1, 4),TEXTURE_2D,false));
					if ((!group.getModel()->isEnabled(PARAM_ANGLE))&&(!group.getModel()->isEnabled(PARAM_TEXTURE_INDEX)))
					{
						float t[8] = {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f};
						texBuffer = vbTextureBuffer->getData();
						texBuffer->Lock(0, group.getParticles().getNbReserved()*4*sizeof(D3DXVECTOR2), &ptr, 0);
						for (size_t i = 0; i < group.getParticles().getNbReserved() * 8; ++i)
							((float*)ptr)[i] = t[i & 7];
						texBuffer->Unlock();
					}
				}
				break;

			case TEXTURE_3D :
				{
					DX9VertexBuffer<D3DXVECTOR3>* vbTextureBuffer = dynamic_cast<DX9VertexBuffer<D3DXVECTOR3>*>(group.createBuffer(TEXTURE_BUFFER_NAME, DX9VertexBufferCreator<D3DXVECTOR3>((D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE1(3)), 4),TEXTURE_3D,false));
					if (!group.getModel()->isEnabled(PARAM_ANGLE))
					{
						float t[12] =  {1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f};
						texBuffer = vbTextureBuffer->getData();
						texBuffer->Lock(0, group.getParticles().getNbReserved()*4*sizeof(D3DXVECTOR3), &ptr, 0);
						for (size_t i = 0; i < group.getParticles().getNbReserved() * 12; ++i)
							((float*)ptr)[i] = t[i % 12];
						texBuffer->Unlock();
					}
				}
				break;

			default:
				return NULL;
		}

		return texBuffer;
	}

	bool DX9QuadRenderer::setTexturingMode(TexturingMode mode)
	{
		texturingMode = mode;
		return true;
	}

	void DX9QuadRenderer::render(const Group& group)
	{
		HRESULT hr = 0;
		int nb_part = group.getNbParticles();

		if (!DX9PrepareBuffers(group))
			return;

		D3DXMATRIX view;
		DX9Info::getDevice()->GetTransform(D3DTS_VIEW, &view);
		D3DXMatrixInverse((D3DXMATRIX*)&invModelView, NULL, &view);

		initBlending();

		DX9Info::getDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

#ifdef _DX9QUADRENDERER_CLOCKWISE_
		DX9Info::getDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
#else
		DX9Info::getDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
#endif

		switch(texturingMode)
		{
			case TEXTURE_2D :
				DX9Info::getDevice()->SetTexture(0, textureIndex);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLOROP, getTextureBlending());
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
				DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

				if (!group.getModel()->isEnabled(PARAM_TEXTURE_INDEX))
				{
					if (!group.getModel()->isEnabled(PARAM_ANGLE))
						renderParticle = &DX9QuadRenderer::render2D;
					else
						renderParticle = &DX9QuadRenderer::render2DRot;
				}
				else
				{
					if (!group.getModel()->isEnabled(PARAM_ANGLE))
						renderParticle = &DX9QuadRenderer::render2DAtlas;
					else
						renderParticle = &DX9QuadRenderer::render2DAtlasRot;
				}
				break;

			case TEXTURE_3D :
				DX9Info::getDevice()->SetTexture(0, textureIndex);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLOROP, getTextureBlending());
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
				DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

				if (!group.getModel()->isEnabled(PARAM_ANGLE))
					renderParticle = &DX9QuadRenderer::render3D;
				else
					renderParticle = &DX9QuadRenderer::render3DRot;
				break;

			case TEXTURE_NONE :
				DX9Info::getDevice()->SetRenderState(D3DRS_COLORVERTEX, true);
				if (!group.getModel()->isEnabled(PARAM_ANGLE))
					renderParticle = &DX9QuadRenderer::render2D;
				else
					renderParticle = &DX9QuadRenderer::render2DRot;
				break;
		}

		bool globalOrientation = precomputeOrientation3D(
			group,
			Vector3D(-invModelView[8],-invModelView[9],-invModelView[10]),
			Vector3D(invModelView[4],invModelView[5],invModelView[6]),
			Vector3D(invModelView[12],invModelView[13],invModelView[14]));


		// pose des verrous sur les buffers pour la mise à jour de ceux-ci
		hr = vertexBuffer->Lock(0, group.getNbParticles()*4*sizeof(D3DXVECTOR3), (void**)&vertexIterator, 0);
		hr = colorBuffer->Lock(0, group.getNbParticles()*4*sizeof(DWORD), (void**)&colorIterator, 0);
		//hr = indexBuffer->Lock(0, group.getNbParticles()*6*sizeof(short), (void**)&indexIterator, 0);

		switch( texturingMode )
		{
			case TEXTURE_2D:
				hr = textureBuffer->Lock(0, group.getNbParticles()*4*sizeof(D3DXVECTOR2), (void**)&textureIterator, 0);
				break;
			case TEXTURE_3D:
				hr = textureBuffer->Lock(0, group.getNbParticles()*4*sizeof(D3DXVECTOR3), (void**)&textureIterator, 0);
				break;
		}
		//---------------------------------------------------------------------

		if (globalOrientation)
		{
			computeGlobalOrientation3D();

			for (size_t i = 0; i < group.getNbParticles(); ++i)
				(this->*renderParticle)(group.getParticle(i));
		}
		else
		{
			for (size_t i = 0; i < group.getNbParticles(); ++i)
			{
				const Particle& particle = group.getParticle(i);
				computeSingleOrientation3D(particle);
				(this->*renderParticle)(particle);
			}
		}

		// enlève les verrous des buffers
		vertexBuffer->Unlock();
		colorBuffer->Unlock();
		//indexBuffer->Unlock();
		if( texturingMode != TEXTURE_NONE )
			textureBuffer->Unlock();
		//---------------------------------------------------------------------------
		
		
		// bind buffers and draw
		{
			switch( texturingMode )
			{
				case TEXTURE_2D:
					{
						DX9Info::getDevice()->SetStreamSource(2, textureBuffer, 0, sizeof(D3DXVECTOR2));
						DX9Info::getDevice()->SetVertexDeclaration(pVertexDecl2D);
					}
					break;

				case TEXTURE_3D:
					{
						DX9Info::getDevice()->SetStreamSource(2, textureBuffer, 0, sizeof(D3DXVECTOR3));
						DX9Info::getDevice()->SetVertexDeclaration(pVertexDecl3D);
					}
					break;

				case TEXTURE_NONE:
					{
						DX9Info::getDevice()->SetVertexDeclaration(pVertexDecl);
					}
					break;
			}

			DX9Info::getDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(D3DXVECTOR3));

			DX9Info::getDevice()->SetStreamSource(1, colorBuffer, 0, sizeof(DWORD));

			DX9Info::getDevice()->SetIndices(indexBuffer);
			this->offsetIndex = 0;
			DX9Info::getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nb_part<<2, 0, nb_part<<1);
		}
		//---------------------------------------------------------------------------
	}

	void DX9QuadRenderer::render2D(const Particle& particle) const
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
	}

	void DX9QuadRenderer::render2DRot(const Particle& particle) const
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
	}

	void DX9QuadRenderer::render3D(const Particle& particle) const
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
		DX9CallTexture3D(particle);
	}

	void DX9QuadRenderer::render3DRot(const Particle& particle) const
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
		DX9CallTexture3D(particle);
	}

	void DX9QuadRenderer::render2DAtlas(const Particle& particle) const
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
		DX9CallTexture2DAtlas(particle);
	}

	void DX9QuadRenderer::render2DAtlasRot(const Particle& particle) const
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		DX9CallColorAndVertex(particle);
		DX9CallTexture2DAtlas(particle);
	}

	HRESULT DX9QuadRenderer::OnD3D9CreateDevice()
	{
		DX9Info::getDevice()->CreateVertexDeclaration(QuadVertexDecl, &pVertexDecl);
		DX9Info::getDevice()->CreateVertexDeclaration(QuadVertexDecl2D, &pVertexDecl2D);
		DX9Info::getDevice()->CreateVertexDeclaration(QuadVertexDecl3D, &pVertexDecl3D);

		return S_OK;
	}
}}
