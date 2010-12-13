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

#include "RenderingAPIs/DX9/SPK_DX9Renderer.h"
#include "Extensions/Renderers/SPK_LineRendererInterface.h"

namespace SPK
{
namespace DX9
{
	struct LineVertex
	{
		D3DXVECTOR3 position;
		D3DCOLOR color;
	};
	
	class SPK_DX9_PREFIX DX9LineRenderer : public DX9Renderer, public LineRendererInterface
	{
		SPK_IMPLEMENT_REGISTERABLE(DX9LineRenderer)

	public :

		DX9LineRenderer(float length = 1.0f, float width = 1.0f);

		static DX9LineRenderer* create(float length = 1.0f, float width = 1.0f);

		virtual void render(const Group& group);

		virtual void createBuffers(const Group& group);
		virtual void destroyBuffers(const Group& group);

		

		virtual bool DX9CreateBuffers(const Group& group);
		virtual bool DX9DestroyBuffers(const Group& group);

	protected :

		virtual bool checkBuffers(const Group& group);

		virtual bool DX9CheckBuffers(const Group& group);

	private :

		// vertex buffers and iterators
		
		static LineVertex* gpuBuffer;
		static LineVertex* gpuIterator;

		static LPDIRECT3DVERTEXBUFFER9 DX9VertexBuffer;

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

	};

	inline DX9LineRenderer* DX9LineRenderer::create(float length, float width)
	{
		DX9LineRenderer* obj = new DX9LineRenderer(length, width);
		registerObject(obj);
		DX9Info::DX9RegisterRenderer(obj);
		return obj;
	}
}}

#endif
