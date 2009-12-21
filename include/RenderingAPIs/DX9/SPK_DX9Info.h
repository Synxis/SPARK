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
/*
	enum DX9BufferUsage
	{
		DX9BU_VERTEX_BUFFER = 0,
		DX9BU_INDEX_BUFFER = 1
	};

	struct DX9BufferInfo
	{
		DX9BufferUsage BufferUsage;
		UINT Length;
		DWORD Usage;
		union
		{
			D3DFORMAT Format;
			DWORD FVF;
		};
		D3DPOOL Pool;
		IDirect3DResource9 **Ptr;
	};
*/
	class SPK_DX9_PREFIX DX9Info
	{
	private:
		static LPDIRECT3DDEVICE9 device;
		static D3DPOOL pool;

		//static std::vector<const Group *> groups;
		//static std::vector<IUnknown **> resources;

		static std::vector<DX9Renderer *> renderers;

		//static std::map<IDirect3DResource9 **, DX9BufferInfo> buffers;

		//static std::map<std::pair<DX9Renderer *, Group *>, IDirect3DResource9 *> gpuResources;

		// état du device
		//static bool lost; // device perdu
		//static bool destroyed; // device détruit

	public:
		inline static LPDIRECT3DDEVICE9 getDevice();
		inline static D3DPOOL getPool();

		inline static void setDevice(LPDIRECT3DDEVICE9 device);
		inline static void setPool(D3DPOOL pool);

		// état du device
//		inline static void OnD3D9LostDevice();
//		inline static void OnD3D9DestroyDevice();

//		inline static bool isDeviceLost();
//		inline static bool isDeviceDestroyed();

//		inline static void addGroup(const Group *group);
//		inline static void removeGroup(const Group *group);
//		inline static void destroyAllBuffers();

//		inline static void addResource(IUnknown **resource);
//		inline static void removeResource(IUnknown **resource);
//		inline static void destroyAllResources();

//		inline static HRESULT CreateBuffer(DX9BufferInfo &bufferInfo);
//		inline static HRESULT DestroyBuffer(DX9BufferInfo &bufferInfo);

//		inline static IUnknown *get(DX9Renderer *renderer, Group *group);
		inline static void DX9RegisterRenderer(DX9Renderer *renderer);
		inline static void DX9ReleaseRenderer(DX9Renderer *renderer);
		/*inline */static void DX9DestroyAllBuffers();
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
/*
	inline void DX9Info::DX9DestroyAllBuffers()
	{
		for(int i = 0; i < renderers.size(); i++)
		{
			renderers[i]->DX9DestroyAllBuffers();
		}
		renderers.clear();
	}
*/
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
//		DX9Info::lost = false;
//		DX9Info::destroyed = false;
	}

	inline void DX9Info::setPool(D3DPOOL pool)
	{
		DX9Info::pool = pool;
	}
/*
	inline void DX9Info::OnD3D9LostDevice()
	{
		destroyAllBuffers();
	}

	inline void DX9Info::OnD3D9DestroyDevice()
	{
		destroyAllBuffers();
	}

	inline bool DX9Info::isDeviceLost()
	{
		return DX9Info::lost;
	}

	inline bool DX9Info::isDeviceDestroyed()
	{
		return DX9Info::destroyed;
	}

	inline void DX9Info::addGroup(const Group *group)
	{
		DX9Info::groups.push_back(group);
	}

	inline void DX9Info::removeGroup(const Group *group)
	{
		std::vector<const Group*>::iterator it = std::find(groups.begin(), groups.end(), group);
		if (it != groups.end())
		{
			groups.erase(it);
		}
	}

	inline void DX9Info::destroyAllBuffers()
	{
		for( size_t i = 0; i < DX9Info::groups.size(); ++i )
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
		for( std::vector<IUnknown **>::iterator it = DX9Info::resources.begin(); it != DX9Info::resources.end(); ++it )
		{
			SAFE_RELEASE( **it );
		}
		DX9Info::resources.clear();
	}

	inline HRESULT DX9Info::CreateBuffer(DX9BufferInfo &bi)
	{
		if( device == NULL )
			return NULL;

		HRESULT hr = S_OK;

		if( bi.BufferUsage == DX9BU_VERTEX_BUFFER )
			hr = device->CreateVertexBuffer(bi.Length, bi.Usage, bi.FVF, bi.Pool, (IDirect3DVertexBuffer9 **)bi.Ptr, NULL);

		if( bi.BufferUsage == DX9BU_INDEX_BUFFER )
			hr = device->CreateIndexBuffer(bi.Length, bi.Usage, bi.Format, bi.Pool, (IDirect3DIndexBuffer9 **)bi.Ptr, NULL);

		buffers[bi.Ptr] = bi;
		return hr;
	}

	inline HRESULT DX9Info::DestroyBuffer(DX9BufferInfo &bi)
	{
		HRESULT hr = S_OK;
		IDirect3DResource9 **ptr = bi.Ptr;
		SAFE_RELEASE( *(bi.Ptr) );
		buffers.erase(ptr);
		return hr;
	}
*/
/*
	inline IDirect3DResource9 *get(DX9Renderer *renderer, Group *group)
	{
		if( renderer == NULL || group == NULL ) return NULL;

		std::pair<DX9Renderer *, Group *> key(renderer, group);
		std::iterator<std::pair<DX9Renderer *, Group *>, IDirect3DResource9 *> it = DX9Info::gpuResources.find(key);

		if( it == DX9Info::gpuResources.end() )
		{
			if( DX9Info::device == NULL ) return NULL;

			HRESULT hr = S_OK;

			if( bi.BufferUsage == DX9BU_VERTEX_BUFFER )
				hr = device->CreateVertexBuffer(bi.Length, bi.Usage, bi.FVF, bi.Pool, (IDirect3DVertexBuffer9 **)bi.Ptr, NULL);

			if( bi.BufferUsage == DX9BU_INDEX_BUFFER )
				hr = device->CreateIndexBuffer(bi.Length, bi.Usage, bi.Format, bi.Pool, (IDirect3DIndexBuffer9 **)bi.Ptr, NULL);

			if( hr != S_OK ) return NULL;
		}

		return it->second;
	}
*/
}}

#endif
