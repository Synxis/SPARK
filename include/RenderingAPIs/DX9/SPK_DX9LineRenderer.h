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


#ifndef H_SPK_DX9LINERENDERER
#define H_SPK_DX9LINERENDERER

#include "SPK_DX9Renderer.h"

namespace SPK
{
namespace DX9
{
	struct LineVertex
	{
		D3DXVECTOR3 position;
		D3DCOLOR color;
	};
	
	/**
	* @class DX9LineRenderer
	* @brief A Renderer drawing particles as quad
	*
	* the length of the lines is function of the Particle velocity and is defined in the universe space
	* while the width is fixed and defines in the screen space (in pixels).<br>
	* <br>
	* Below are the parameters of Particle that are used in this Renderer (others have no effects) :
	* <ul>
	* <li>SPK::PARAM_RED</li>
	* <li>SPK::PARAM_GREEN</li>
	* <li>SPK::PARAM_BLUE</li>
	* <li>SPK::PARAM_ALPHA (only if blending is enabled)</li>
	* </ul>
	*/
	class SPK_DX9_PREFIX DX9LineRenderer : public DX9Renderer
	{
		SPK_IMPLEMENT_REGISTERABLE(DX9LineRenderer)

	public :

		//////////////////
		// Constructors //
		//////////////////

		/**
		* @brief Constructor of DX9LineRenderer
		* @param length : the length multiplier of this DX9LineRenderer
		* @param width : the width of this DX9LineRenderer in pixels
		* @version 1.03.00
		*/
		DX9LineRenderer(float length = 1.0f, float width = 1.0f);

		/////////////
		// Setters //
		/////////////

		/**
		* @brief Sets the length multiplier of this GLLineRenderer
		*
		* The length multiplier is the value which will be multiplied by the Particle velocity to get the line length in the universe.<br>
		* A positive length means the line will be drawn in advance to the Particle, as opposed to a negative length.
		*
		* @param length : the length multiplier of this GLLineRenderer
		*/
		inline void setLength(float length);

		/**
		* @brief Sets the width of this DX9LineRenderer
		* @param width : the width of this DX9LineRenderer in pixels
		*/
		inline void setWidth(float width);

		/////////////
		// Getters //
		/////////////

		/**
		* @brief Gets the length multiplier of this DX9LineRenderer
		* @return the length multiplier of this DX9LineRenderer
		*/
		inline float getLength() const;

		/**
		* @brief Gets the width of this DX9LineRenderer
		* @return the width of this DX9LineRenderer in pixels
		*/
		inline float getWidth() const;

		///////////////
		// Interface //
		///////////////

		virtual void createBuffers(const Group& group);
		virtual void destroyBuffers(const Group& group);

		virtual void render(const Group& group);

		virtual HRESULT OnD3D9CreateDevice();

	private :

		float length;
		float width;

		// vertex buffers and iterators
		
		static LPDIRECT3DVERTEXBUFFER9 gpuBuffer;
		static LineVertex* gpuIterator;

#define DX9LINERENDERER_AS_LINELIST
#ifndef DX9LINERENDERER_AS_LINELIST
		static LPDIRECT3DINDEXBUFFER9 indexBuffer;
		static short* indexIterator;
#endif

		// buffers names
		static const std::string GPU_BUFFER_NAME;
#ifndef DX9LINERENDERER_AS_LINELIST
		static const std::string INDEX_BUFFER_NAME;
#endif

		virtual bool checkBuffers(const Group& group);
	};


	inline void DX9LineRenderer::setLength(float length)
	{
		this->length = length;
	}

	inline void DX9LineRenderer::setWidth(float width)
	{
		this->width = width;
	}

	inline float DX9LineRenderer::getLength() const
	{
		return length;
	}

	inline float DX9LineRenderer::getWidth() const
	{
		return width;
	}
}}

#endif
