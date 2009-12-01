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

namespace SPK
{
namespace DX9
{
	class SPK_DX9_PREFIX DX9Info
	{
	private:
		static LPDIRECT3DDEVICE9 device;
		static D3DPOOL pool;

		static std::vector<Group *> groups;
		static std::vector<IUnknown **> resources;

		// état du device
		static bool lost; // device perdu
		static bool destroyed; // device détruit

	public:
		inline static LPDIRECT3DDEVICE9 getDevice();
		inline static D3DPOOL getPool();

		inline static void setDevice(LPDIRECT3DDEVICE9 device);
		inline static void setPool(D3DPOOL pool);

		// état du device
		inline static void OnD3D9LostDevice();
		inline static void OnD3D9DestroyDevice();

		inline static bool isDeviceLost();
		inline static bool isDeviceDestroyed();

		inline static void addGroup(Group *group);
		inline static void removeGroup(Group *group);
		inline static void destroyAllBuffers();

		inline static void addResource(IUnknown **resource);
		inline static void removeResource(IUnknown **resource);
		inline static void destroyAllResources();
	};

	inline LPDIRECT3DDEVICE9 DX9Info::getDevice()
	{
		return DX9Info::device;
	}

	inline D3DPOOL DX9Info::getPool()
	{
		return DX9Info::pool;
	}

	inline void DX9Info::setDevice(LPDIRECT3DDEVICE9 device)
	{
		DX9Info::device = device;
		DX9Info::lost = false;
		DX9Info::destroyed = false;
	}

	inline void DX9Info::setPool(D3DPOOL pool)
	{
		DX9Info::pool = pool;
	}

	inline void DX9Info::OnD3D9LostDevice()
	{
		DX9Info::lost = true;
		if( DX9Info::getPool() != D3DPOOL_MANAGED )
			return;
		DX9Info::destroyAllResources();
	}

	inline void DX9Info::OnD3D9DestroyDevice()
	{
		DX9Info::destroyed = true;
		DX9Info::lost = true;
		DX9Info::device = NULL;
		DX9Info::destroyAllResources();
	}

	inline bool DX9Info::isDeviceLost()
	{
		return DX9Info::lost;
	}

	inline bool DX9Info::isDeviceDestroyed()
	{
		return DX9Info::destroyed;
	}

	inline void DX9Info::addGroup(Group *group)
	{
		DX9Info::groups.push_back(group);
	}

	inline void DX9Info::removeGroup(Group *group)
	{
		std::vector<Group*>::iterator it = std::find(groups.begin(),groups.end(),group);
		if (it != groups.end())
		{
			groups.erase(it);
		}
	}

	inline void DX9Info::destroyAllBuffers()
	{
		for( int i = 0; i < DX9Info::groups.size(); ++i )
		{
			DX9Info::groups[i]->destroyAllBuffers();
		}
	}

	inline void DX9Info::addResource(IUnknown **resource)
	{
		DX9Info::resources.push_back(resource);
	}

	inline void DX9Info::removeResource(IUnknown **resource)
	{
		std::vector<IUnknown **>::iterator it = std::find(DX9Info::resources.begin(),DX9Info::resources.end(),resource);
		if (it != DX9Info::resources.end())
		{
			DX9Info::resources.erase(it);
		}
	}

	inline void DX9Info::destroyAllResources()
	{
		for( int i = 0; i < DX9Info::resources.size(); ++i )
		{
			SAFE_RELEASE( *DX9Info::resources[i] );
		}
		DX9Info::resources.clear();
	}
}}

#endif
