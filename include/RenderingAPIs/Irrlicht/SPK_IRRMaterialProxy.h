//////////////////////////////////////////////////////////////////////////////////
// SPARK Irrlicht Rendering library												//
// Copyright (C) 2011															//
// Thibault Lescoat -  info-tibo <at> orange <dot> fr							//
// Julien Fryer - julienfryer@gmail.com											//
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

#ifndef SPK_IRRMATERIALPROXY
#define SPK_IRRMATERIALPROXY

#include "RenderingAPIs/Irrlicht/SPK_IRR_DEF.h"

namespace SPK
{
namespace IRR
{
	/**
	* @brief A Proxy for Irrlicht material
	* As some properties of the material are controlled by SPARK and must not be altered by user, a proxy is used to access the Irrlicht material
	* @since 1.5.6
	*/
	class IRRMaterialProxy
	{
	public :

		IRRMaterialProxy();

		const irr::video::SMaterial& getMaterial() const { return material; }

		void setAntiAliasing(irr::u8 aa)		{ material.AntiAliasing = aa; }
		irr::u8 getAntiAliasing() const			{ return material.AntiAliasing; }

		void setFogEnable(bool enable)			{ material.FogEnable = enable; }
		bool getFogEnable() const				{ return material.FogEnable; }

		void setPointCloud(bool enable)			{ material.PointCloud = enable; }
		bool getPointCloud() const				{ return material.PointCloud; }

		void setThickness(irr::f32 thickness)	{ material.Thickness = thickness; }
		irr::f32 getThickness() const			{ return material.Thickness; }

		void setWireframe(bool enable)			{ material.Wireframe = enable; }
		bool getWireframe() const				{ return material.Wireframe; }

		void setZBuffer(irr::u8 z)				{ material.ZBuffer = z; }
		irr::u8 getZBuffer() const				{ return material.ZBuffer; }

		void setZWriteEnable(bool enable)		{ material.ZWriteEnable = enable; }
		bool getZWriteEnable() const			{ return material.ZWriteEnable; }

		irr::video::SMaterialLayer& textureLayer()				{ return material.TextureLayer[0]; }
		const irr::video::SMaterialLayer& textureLayer() const	{ return material.TextureLayer[0]; }

		void setBlending(irr::video::E_BLEND_FACTOR srcFunc,irr::video::E_BLEND_FACTOR destFunc,unsigned int alphaSrc);
		
		irr::video::E_BLEND_FACTOR getBlendSrcFunc() const	{ return blendSrcFunc; }
		irr::video::E_BLEND_FACTOR getBlendDestFunc() const	{ return blendDestFunc; }
		unsigned int getAlphaSource() const					{ return alphaSource; }


	private :

		irr::video::SMaterial material;

		// Stored here to avoid the non convenient unpack function on material
		irr::video::E_BLEND_FACTOR blendSrcFunc;
		irr::video::E_BLEND_FACTOR blendDestFunc;
		unsigned int alphaSource;
	};

	inline IRRMaterialProxy::IRRMaterialProxy()
	{
		material.GouraudShading = true;												// fix 1.05.00 for ATI cards
		material.Lighting = false;													// No lights per default
		material.BackfaceCulling = false;											// Deactivates backface culling
		material.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;					// To allow complex blending functions
		setBlending(irr::video::EBF_ONE,irr::video::EBF_ZERO,irr::video::EAS_NONE);	// No blending
	}

	inline void IRRMaterialProxy::setBlending(irr::video::E_BLEND_FACTOR srcFunc,irr::video::E_BLEND_FACTOR destFunc,unsigned int alphaSrc)
	{
		blendSrcFunc = srcFunc;
		blendDestFunc = destFunc;
		alphaSource = alphaSrc;
		material.MaterialTypeParam = irr::video::pack_texureBlendFunc(
			blendSrcFunc,
			blendDestFunc,
			irr::video::EMFN_MODULATE_1X,
			alphaSource);
	}
}}

#endif
