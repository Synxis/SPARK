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

		IRRMaterialProxy(irr::video::SMaterial& material) : material(material) {}

		const irr::video::SMaterial& getMaterial() const { return material; }

		void setAntiAliasing(irr::u8 aa)		{ material.AntiAliasing = aa; }
		irr::u8 getAntiAliasing() const			{ return material.AntiAliasing; }

		void setFogEnable(bool enable)			{ material.FogEnable = enable; }
		bool getFogEnable() const				{ return material.FogEnable; }

		void setPointCloud(bool enable)			{ material.PointCloud = enable; }
		bool getPointCloud() const				{ return material.PointCloud; }

		void setWireframe(bool enable)			{ material.Wireframe = enable; }
		bool getWireframe() const				{ return material.Wireframe; }

	private :

		irr::video::SMaterial& material;
	};
}}

#endif
