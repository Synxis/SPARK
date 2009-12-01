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


#ifndef H_SPK_DX9RENDERER
#define H_SPK_DX9RENDERER

#include "RenderingAPIs/DX9/SPK_DX9_DEF.h"
#include "RenderingAPIs/DX9/SPK_DX9Info.h"
#include "Core/SPK_Renderer.h"

namespace SPK
{
namespace DX9
{
	class SPK_DX9_PREFIX DX9Renderer : public Renderer
	{
	public :

		/////////////////
		// Constructor //
		/////////////////

		/** @brief Constructor of DX9Renderer */
		DX9Renderer();

		////////////////
		// Destructor //
		////////////////

		/** @brief Destructor of DX9Renderer */
		virtual ~DX9Renderer();

		/////////////
		// Setters //
		/////////////

		/**
		* @brief Enables or disables the blending of this DX9Renderer
		* @param blendingEnabled true to enable the blending, false to disable it
		*/
		virtual inline void enableBlending(bool blendingEnabled);

		/**
		* @brief Sets the blending functions of this DX9Renderer
		*
		* the blending functions are one of the DX blending functions.
		*
		* @param src : the source blending function of this DX9Renderer
		* @param dest : the destination blending function of this DX9Renderer
		*/
		inline void setBlendingFunctions(int src,int dest);
		virtual void setBlending(BlendingMode blendMode);

		/**
		* @brief Sets the texture blending function of this DX9Renderer
		*
		* the texture blending function is one of the DX texture blending functions.
		*
		* @param textureBlending : the texture blending function of this DX9Renderer
		*/
		inline void setTextureBlending(int textureBlending);

		/////////////
		// Getters //
		/////////////

		/**
		* @brief Tells whether blending is enabled for this DX9Renderer
		* @return true if blending is enabled, false if it is disabled
		*/
		inline bool isBlendingEnabled() const;

		/**
		* @brief Gets the source blending function of this DX9Renderer
		* @return the source blending function of this DX9Renderer
		*/
		inline int getSrcBlendingFunction() const;

		/**
		* @brief Gets the destination blending function of this DX9Renderer
		* @return the source destination function of this DX9Renderer
		*/
		inline int getDestBlendingFunction() const;

		/**
		* @brief Gets the texture blending function of this DX9Renderer
		* @return the texture blending function of this DX9Renderer
		*/
		inline int getTextureBlending() const;

		/*
		//--------------------------------------------------------------------------------------
		// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
		// and aren't tied to the back buffer size
		//--------------------------------------------------------------------------------------
		virtual HRESULT OnD3D9CreateDevice() = 0;

		/*
		//--------------------------------------------------------------------------------------
		// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
		// or that are tied to the back buffer size 
		//--------------------------------------------------------------------------------------
		virtual HRESULT OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice ) = 0;
		*/

		//--------------------------------------------------------------------------------------
		// Release D3D9 resources created in the OnD3D9ResetDevice callback 
		//--------------------------------------------------------------------------------------
		HRESULT OnD3D9LostDevice()
		{
			if( DX9Info::getPool() != D3DPOOL_DEFAULT )
				return S_OK;

			reinit = true;
			return S_OK;
		}

		//--------------------------------------------------------------------------------------
		// Release D3D9 resources created in the OnD3D9CreateDevice callback 
		//--------------------------------------------------------------------------------------
		HRESULT OnD3D9DestroyDevice()
		{
			if( DX9Info::getPool() != D3DPOOL_MANAGED )
				return S_OK;

			reinit = true;
			return S_OK;
		}

		bool DX9PrepareBuffers(const Group& group);

	protected :

		/** @brief Inits the blending of this DX9Renderer */
		inline void initBlending() const;

		inline void initRenderingHints() const;

		bool reinit;

	private :

		static bool statesSaved;

		bool blendingEnabled;
		int srcBlending;
		int destBlending;

		int textureBlending;
	};


	inline void DX9Renderer::enableBlending(bool blendingEnabled)
	{
		this->blendingEnabled = blendingEnabled;
	}

	inline void DX9Renderer::setBlendingFunctions(int src,int dest)
	{
		srcBlending = src;
		destBlending = dest;
	}

	inline void DX9Renderer::setTextureBlending(int textureBlending)
	{
		this->textureBlending = textureBlending;
	}

	inline bool DX9Renderer::isBlendingEnabled() const
	{
		return blendingEnabled;
	}

	inline int DX9Renderer::getSrcBlendingFunction() const
	{
		return srcBlending;
	}

	inline int DX9Renderer::getDestBlendingFunction() const
	{
		return destBlending;
	}

	inline int DX9Renderer::getTextureBlending() const
	{
		return textureBlending;
	}

	inline void DX9Renderer::initBlending() const
	{
		if (blendingEnabled)
		{
			DX9Info::getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			DX9Info::getDevice()->SetRenderState(D3DRS_SRCBLEND, srcBlending);
			DX9Info::getDevice()->SetRenderState(D3DRS_DESTBLEND, destBlending);
		}
		else
			DX9Info::getDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}

	inline void DX9Renderer::initRenderingHints() const
	{
		// alpha test
		if (isRenderingHintEnabled(ALPHA_TEST))
		{
			//glAlphaFunc(GL_GEQUAL,getAlphaTestThreshold());
			DX9Info::getDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		}
		else
			DX9Info::getDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);

		// depth test
		if (isRenderingHintEnabled(DEPTH_TEST))
			DX9Info::getDevice()->SetRenderState(D3DRS_ZENABLE, true);
		else
			DX9Info::getDevice()->SetRenderState(D3DRS_ZENABLE, false);

		// depth write
		DX9Info::getDevice()->SetRenderState(D3DRS_ZWRITEENABLE, isRenderingHintEnabled(DEPTH_WRITE));
	}
}}

#endif
