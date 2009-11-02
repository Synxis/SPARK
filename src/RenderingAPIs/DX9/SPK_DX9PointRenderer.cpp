//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//                           foulon matthieu - stardeath@wanadoo.fr				//
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


#include "RenderingAPIs/DX9/SPK_DX9PointRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"

namespace SPK
{
namespace DX9
{
	LPDIRECT3DVERTEXBUFFER9 DX9PointRenderer::vertexBuffer = NULL;

	const std::string DX9PointRenderer::VERTEX_BUFFER_NAME("SPK_DX9PointRenderer_Vertex");

	const float DX9PointRenderer::QUADRATIC_SCREEN[3] = {1.0f,0.0f,0.0f};

	float DX9PointRenderer::pixelPerUnit = 1024.0f;

	const float DX9PointRenderer::POINT_SIZE_CURRENT = 32.0f;
	const float DX9PointRenderer::POINT_SIZE_MIN = 1.0f;
	const float DX9PointRenderer::POINT_SIZE_MAX = 1024.0f;

	DX9PointRenderer::DX9PointRenderer(float size) :
		DX9Renderer(),
		type(POINT_SQUARE),
		size(size),
		textureIndex(0),
		worldSize(false)
	{}

	bool DX9PointRenderer::setType(PointType type)
	{
		this->type = type;
		return true;
	}

	bool DX9PointRenderer::enableWorldSize(bool worldSizeEnabled)
	{
		worldSize = worldSizeEnabled;
		return worldSize;
	}

	bool DX9PointRenderer::checkBuffers(const Group& group)
	{
		// if device reseted or destroyed, send message to recreate buffers
		if( reinit == true )
		{
			reinit = false;
			return false;
		}

		DX9VertexBuffer<PointVertex>* pvbBuffer;

		if ((pvbBuffer = dynamic_cast<DX9VertexBuffer<PointVertex>*>(group.getBuffer(VERTEX_BUFFER_NAME))) == NULL)
			return false;

		vertexBuffer = pvbBuffer->getData();

		return true;
	}

	void DX9PointRenderer::createBuffers(const Group& group)
	{
		DX9VertexBuffer<PointVertex>* vbVertexBuffer = dynamic_cast<DX9VertexBuffer<PointVertex>*>(group.createBuffer(VERTEX_BUFFER_NAME, DX9VertexBufferCreator<PointVertex>((D3DFVF_XYZ|D3DFVF_DIFFUSE), 1), 0, false));
		vertexBuffer = vbVertexBuffer->getData();
	}

	void DX9PointRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(VERTEX_BUFFER_NAME);
	}

	void DX9PointRenderer::render(const Group& group)
	{
		initBlending();

		if (worldSize)
		{
			enablePointParameter(size, true);
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, true);
		}
		else
		{
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSIZE, FtoDW(size));
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, false);
		}

		DX9Info::getDevice()->SetTexture(0, textureIndex);
		DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLOROP, getTextureBlending());
		DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		DX9Info::getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		DX9Info::getDevice()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		DX9Info::getDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, true);

		void *ptr;
		if( group.getNbParticles() != 0 )
		{
			vertexBuffer->Lock(0, (group.getNbParticles())*sizeof(PointVertex), &ptr, 0);
			for(size_t i = 0; i < group.getNbParticles(); i++)
			{
				Assign(((PointVertex*)ptr)[i].position, group.getParticle(i).position());

				/*
				if( group.getModel()->isEnabled(PARAM_ALPHA) )
				{
					((PointVertex*)ptr)[i].color = D3DCOLOR_COLORVALUE(
						group.getParticle(i).getParamCurrentValue(PARAM_RED),
						group.getParticle(i).getParamCurrentValue(PARAM_GREEN),
						group.getParticle(i).getParamCurrentValue(PARAM_BLUE),
						group.getParticle(i).getParamCurrentValue(PARAM_ALPHA));
				}
				else
				{
					((PointVertex*)ptr)[i].color = D3DCOLOR_COLORVALUE(
						group.getParticle(i).getParamCurrentValue(PARAM_RED),
						group.getParticle(i).getParamCurrentValue(PARAM_GREEN),
						group.getParticle(i).getParamCurrentValue(PARAM_BLUE),
						1.0f);
				}
				*/
				((PointVertex*)ptr)[i].color = D3DCOLOR_COLORVALUE(
					group.getParticle(i).getR(),
					group.getParticle(i).getG(),
					group.getParticle(i).getB(),
					group.getParticle(i).getParamCurrentValue(PARAM_ALPHA));
			}
			vertexBuffer->Unlock();

			DX9Info::getDevice()->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
			DX9Info::getDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(PointVertex));
			DX9Info::getDevice()->DrawPrimitive(D3DPT_POINTLIST, 0, group.getNbParticles());
		}
	}

	void DX9PointRenderer::setPixelPerUnit(float fovy,int screenHeight)
	{
		// the pixel per unit is computed for a distance from the camera of screenHeight
		pixelPerUnit = screenHeight / (2.0f * tan(fovy * 0.5f));
	}

	void DX9PointRenderer::enablePointParameter(float size,bool distance)
	{
		// derived size = size * sqrt(1 / (A + B * distance + C * distance²))
		if (distance)
		{
			const float sqrtC = POINT_SIZE_CURRENT / (size * pixelPerUnit);
			const float QUADRATIC_WORLD[3] = {0.0f, 0.0f, sqrtC * sqrtC}; // A = 0; B = 0; C = (POINT_SIZE_CURRENT / (size * pixelPerUnit))²
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(QUADRATIC_WORLD[0]));
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(QUADRATIC_WORLD[1]));
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(QUADRATIC_WORLD[2]));
		}
		else
		{
			const float sqrtA = POINT_SIZE_CURRENT / size;
			const float QUADRATIC_WORLD[3] = {sqrtA * sqrtA, 0.0f, 0.0f};	// A = (POINT_SIZE_CURRENT / size)²; B = 0; C = 0
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(QUADRATIC_WORLD[0]));
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(QUADRATIC_WORLD[1]));
			DX9Info::getDevice()->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(QUADRATIC_WORLD[2]));
		}

		DX9Info::getDevice()->SetRenderState(D3DRS_POINTSIZE, FtoDW(size));
		DX9Info::getDevice()->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(POINT_SIZE_MIN));
		DX9Info::getDevice()->SetRenderState(D3DRS_POINTSIZE_MAX, FtoDW(POINT_SIZE_MAX));
	}

	HRESULT DX9PointRenderer::OnD3D9CreateDevice()
	{
		return S_OK;
	}
}}
