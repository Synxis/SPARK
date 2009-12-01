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


#include "RenderingAPIs/DX9/SPK_DX9Info.h"

namespace SPK
{
namespace DX9
{
	LPDIRECT3DDEVICE9 DX9Info::device = NULL;
	D3DPOOL DX9Info::pool = (D3DPOOL)-1;

	bool DX9Info::lost = false;
	bool DX9Info::destroyed = false;

	std::vector<Group *> DX9Info::groups = std::vector<Group *>();
	std::vector<IUnknown **> DX9Info::resources = std::vector<IUnknown **>();
}}
