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


#ifndef H_SPK_DX9POINTRENDERER
#define H_SPK_DX9POINTRENDERER

#include "SPK_DX9Renderer.h"
#include "Extensions/Renderers/SPK_PointRendererInterface.h"

namespace SPK
{
namespace DX9
{
	struct PointVertex
	{
		D3DXVECTOR3 position;
		D3DCOLOR color;
	};


	/**
	* @class DX9PointRenderer
	* @brief A Renderer drawing drawing particles as DX points
	*
	* openGL points can be configured to render them in 3 different ways :
	* <ul>
	* <li>SPK::POINT_SQUARE : standard DX points</li>
	* <li>SPK::POINT_CIRCLE : antialiased DX points</li>
	* <li>SPK::POINT_SPRITE : DX point sprites (must be supported by the hardware)</li>
	* </ul>
	* Moreover, points size can either be defined in screen space (in pixels) or in the universe space (must be supported by the hardware).
	* The advantage of the universe space is that points size on the screen will be dependant to their distance to the camera, whereas in screen space
	* all points will have the same size on the screen no matter their distance to the camera.
	* <br>
	* Below are the parameters of Particle that are used in this Renderer (others have no effects) :
	* <ul>
	* <li>SPK::PARAM_RED</li>
	* <li>SPK::PARAM_GREEN</li>
	* <li>SPK::PARAM_BLUE</li>
	* <li>SPK::PARAM_ALPHA (only if blending is enabled)</li>
	* </ul>
	*
	* @version 1.03.00
	*/
	class SPK_DX9_PREFIX DX9PointRenderer : public DX9Renderer
	{
		
		SPK_IMPLEMENT_REGISTERABLE(DX9PointRenderer)

	public :

		//////////////////
		// Constructors //
		//////////////////

		/** 
		* @brief Constructor of DX9PointRenderer
		* @param size : the size of the points
		*/
		DX9PointRenderer(float size = 1.0f);

		static inline DX9PointRenderer* create(float size = 1.0f);

		/////////////
		// Setters //
		/////////////

		/**
		* @brief Sets the type of DX points to use in this GLPointRenderer
		*
		* If the type set is SPK::POINT_SPRITE, the extension is checked.
		* if point sprites are not supported by the hardware, false is returned and nothing happens.
		*
		* @param type : the type of openGL points to use in this DX9PointRenderer
		* @return true if the type can be set, false otherwise
		*/
		bool setType(PointType type);

		/**
		* @brief Sets the size of the points in this DX9PointRenderer
		*
		* the values depends on whether the size if in the universe space or in the screen space (see enableWorldSize(bool)).
		* <ul>
		* <li>In screen space it is in pixels</li>
		* <li>In universe space it is in the universe unit</li>
		* </ul>
		*
		* @param size : the size of the points in this DX9PointRenderer
		*/
		inline void setSize(float size);

		/**
		* @brief Sets the texture of this DX9PointRenderer
		*
		* Note that the texture is only used if point sprites are used (type is set to SPK::POINT_SPRITE)
		*
		* @param textureIndex : the index of the openGL texture of this DX9PointRenderer
		*/
		inline void setTexture(LPDIRECT3DTEXTURE9 textureIndex);

		/**
		* @brief Sets the way size of points is computed in this DX9PointRenderer
		*
		* if universe size is used (true), the extension is checked.<br>
		* if universe size is not supported by the hardware, false is returned and nothing happens.<br>
		* <br>
		* If world size is enabled, the static method setPixelPetUnit(float,int) must be called to set the conversion between pixels and world units.
		*
		* @param worldSizeEnabled : true to enable universe size, false to use screen size
		* @return true the type of size can be set, false otherwise
		*/
		bool enableWorldSize(bool worldSizeEnabled);

		/**
		* @brief Computes a conversion ratio between pixels and universe units
		*
		* This method must be called when using GLPointRenderer with world size enabled.<br>
		* It allows to well transpose world size to pixel size by setting the right DX parameters.<br>
		* <br>
		* Note that fovy can be replaced by fovx if screenHeight is replaced by screenWidth.
		*
		* @param fovy : the field of view in the y axis in radians
		* @param screenHeight : the height of the viewport in pixels
		*/
		static void setPixelPerUnit(float fovy,int screenHeight);

		/**
		* @brief Enables the use of point parameters
		*
		* This method will set the right point parameters to get the desired point size.<br>
		* <br>
		* It can either be used to have points size function of the distance to the camera (is distance is true)
		* or only to allow bigger range for point sizes (if distance is false).
		* <br>
		* Note that if distance is set to true setPixelPerUnit(float,int) must be call once before.
		* <br>
		* Note that before calling this method, the user must ensure that the point parameters extension is loaded.
		*
		* @param size : the size of the point
		* @param distance : true to enable the modification of the size function of the distance, false not to.
		*/
		/*static */void enablePointParameter(float size,bool distance);

		/////////////
		// Getters //
		/////////////

		/**
		* @brief Gets the type of points in this DX9PointRenderer
		* @return the type of points in this DX9PointRenderer
		*/
		inline PointType getType() const;

		/**
		* @brief Gets the size of points in this DX9PointRenderer
		* @return the size of points in this DX9PointRenderer
		*/
		inline float getSize() const;

		/**
		* @brief Gets the texture of this DX9PointRenderer
		* @return the texture of this DX9PointRenderer
		*/
		inline LPDIRECT3DTEXTURE9 getTexture() const;

		/**
		* @brief Tells whether world size is enabled or not in this DX9PointRenderer
		* @return true if world size is enabled, false if not
		*/
		inline bool isWorldSizeEnabled() const;

		///////////////
		// Interface //
		///////////////

		virtual void createBuffers(const Group& group);
		virtual void destroyBuffers(const Group& group);

		virtual void render(const Group& group);

		virtual HRESULT OnD3D9CreateDevice();

	private :

		virtual bool checkBuffers(const Group& group);

		PointType type;
		float size;
		LPDIRECT3DTEXTURE9 textureIndex;
		bool worldSize;

		// buffers names
		static const std::string VERTEX_BUFFER_NAME;

		// vertex buffer
		static LPDIRECT3DVERTEXBUFFER9 vertexBuffer;

		//////////////////////
		// Point Parameters //
		//////////////////////

		static const float POINT_SIZE_CURRENT;
		static const float POINT_SIZE_MIN;
		static const float POINT_SIZE_MAX;

		static float pixelPerUnit;

		static const float QUADRATIC_SCREEN[3];
	};

	inline DX9PointRenderer* DX9PointRenderer::create(float size)
	{
		DX9PointRenderer* obj = new DX9PointRenderer(size);
		registerObject(obj);
		return obj;
	}

	inline void DX9PointRenderer::setSize(float size)
	{
		this->size = size;
	}

	inline void DX9PointRenderer::setTexture(LPDIRECT3DTEXTURE9 textureIndex)
	{
		this->textureIndex = textureIndex;
	}

	inline PointType DX9PointRenderer::getType() const
	{
		return type;
	}

	inline float DX9PointRenderer::getSize() const
	{
		return size;
	}

	inline LPDIRECT3DTEXTURE9 DX9PointRenderer::getTexture() const
	{
		return textureIndex;
	}

	inline bool DX9PointRenderer::isWorldSizeEnabled() const
	{
		return worldSize;
	}
}}

#endif
