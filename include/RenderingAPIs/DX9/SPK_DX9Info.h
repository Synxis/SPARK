//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2009 - foulon matthieu - stardeath@wanadoo.fr					//
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

#ifndef H_SPK_DX9INFO
#define H_SPK_DX9INFO

#include "RenderingAPIs/DX9/SPK_DX9_DEF.h"
#include "Core/SPK_Group.h"

#include <vector>
#include <map>

namespace SPK
{
namespace DX9
{
	class DX9Renderer;

	class SPK_DX9_PREFIX DX9Info
	{
	private:
		static LPDIRECT3DDEVICE9 device;

		static std::vector<DX9Renderer *> renderers;

	public:
		static LPDIRECT3DDEVICE9 getDevice();
		static D3DPOOL getPool();

		static void setDevice(LPDIRECT3DDEVICE9 device);
		static void setPool(D3DPOOL pool);

		static void DX9RegisterRenderer(DX9Renderer *renderer);
		static void DX9ReleaseRenderer(DX9Renderer *renderer);

		static void DX9DestroyAllBuffers();
	};

	inline void DX9Info::DX9RegisterRenderer(DX9Renderer *renderer)
	{
		renderers.push_back(renderer);
	}

	inline void DX9Info::DX9ReleaseRenderer(DX9Renderer *renderer)
	{
		for(std::vector<DX9Renderer *>::iterator it = renderers.begin(); it != renderers.end(); )
		{
			if (*it == renderer)
				it = renderers.erase(it);
			else
				++it;
		}
	}

	inline LPDIRECT3DDEVICE9 DX9Info::getDevice()
	{
		return DX9Info::device;
	}

	inline void DX9Info::setDevice(LPDIRECT3DDEVICE9 device)
	{
		DX9Info::device = device;
	}
}}

#endif
