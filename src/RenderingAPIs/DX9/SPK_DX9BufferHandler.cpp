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


#include "RenderingAPIs/DX9/SPK_DX9BufferHandler.h"

namespace SPK
{
namespace DX9
{
	bool DX9BufferHandler::DX9PrepareBuffers(const Group& group)
	{
		if(!DX9CheckBuffers(group))
		{
			//if (isBuffersCreationEnabled())
			{
				DX9DestroyBuffers(group);
				if( !DX9CreateBuffers(group) )
				{
//					std::cout << "DX9CreateBuffers echec" << std::endl;
					return false;
				}
				return true;
			}
			//return false;
		}
		return true;
	}

	bool DX9BufferHandler::DX9Bind(const Group& group, int key, void** to)
	{
		DX9BuffersKey = std::pair<const Group *, int>(&group, key);
		DX9BuffersIt = DX9Buffers.find(DX9BuffersKey);
		if( DX9BuffersIt == DX9Buffers.end() ) return false;

		switch( key )
		{
			case DX9_VERTEX_BUFFER_KEY:
			case DX9_COLOR_BUFFER_KEY:
			case DX9_TEXTURE_BUFFER_KEY:
				*to = reinterpret_cast<IDirect3DVertexBuffer9*>(DX9BuffersIt->second);
				break;
			case DX9_INDEX_BUFFER_KEY:
				*to = reinterpret_cast<IDirect3DIndexBuffer9*>(DX9BuffersIt->second);
				break;
			default:
				return false;
		}
		return true;
	}

	bool DX9BufferHandler::DX9Release(const Group& group, int key)
	{
		DX9BuffersKey = std::pair<const Group *, int>(&group, key);
		DX9BuffersIt = DX9Buffers.find(DX9BuffersKey);
		if( DX9BuffersIt != DX9Buffers.end() )
		{
			SAFE_RELEASE( DX9BuffersIt->second );
			DX9Buffers.erase(DX9BuffersIt);
		}
		return true;
	}
}}
