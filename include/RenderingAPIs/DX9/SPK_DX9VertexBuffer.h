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

#ifndef H_SPK_DX9VERTEXBUFFER
#define H_SPK_DX9VERTEXBUFFER

#include "Core/SPK_Buffer.h"

#include "RenderingAPIs/DX9/SPK_DX9Info.h"

namespace SPK
{
namespace DX9
{
	template<class T> class DX9VertexBufferCreator;

	template<class T>
	class DX9VertexBuffer : public Buffer
	{
	friend class DX9VertexBufferCreator<T>;

	public :

		/**
		* @brief Gets the starting address of the inner array
		* @return the starting address of the first element of the inner array
		*/
		inline LPDIRECT3DVERTEXBUFFER9 getData() const;

		inline LPDIRECT3DVERTEXBUFFER9 *getDataAddr();

		/**
		* @brief Gets the number of elements for a single particle
		* @return the number of elements for a single particle
		*/
		inline const size_t getParticleSize() const;

		/**
		* @brief Gets the total number of T in the inner array
		* The value is equal to <i>particleSize * nbParticles</i>.
		* @return the total number of T in the inner array
		*/
		inline const size_t getDataSize() const;

	private :

		LPDIRECT3DVERTEXBUFFER9 data;
		DWORD fvf;

		size_t particleSize;
		size_t dataSize;

		DX9VertexBuffer<T>(DWORD fvf, size_t nbParticles, size_t particleSize);
		DX9VertexBuffer<T>(const DX9VertexBuffer<T>& buffer);
		virtual ~DX9VertexBuffer<T>();

		virtual DX9VertexBuffer<T>* clone() const;
		virtual void swap(size_t index0,size_t index1);
	};

	template<class T>
	class DX9VertexBufferCreator : public BufferCreator
	{
	public :

		/**
		* @brief Constructor of ArrayBuffer
		* @param device : the direct3d device
		* @param fvf : 
		* @param particleSize : the number of elements per particle in the buffer to be created
		*/
		DX9VertexBufferCreator<T>(DWORD fvf, size_t particleSize);

	private :

		size_t particleSize;
		DWORD fvf;

		virtual DX9VertexBuffer<T>* createBuffer(size_t nbParticles, const Group& group) const;
	};


	template<class T>
	DX9VertexBuffer<T>::DX9VertexBuffer(DWORD fvf, size_t nbParticles, size_t particleSize) :
		Buffer(),
		fvf(fvf),
		dataSize(nbParticles * particleSize),
		particleSize(particleSize)
	{
		DX9Info::getDevice()->CreateVertexBuffer(dataSize*sizeof(T), 0, fvf, DX9Info::getPool(), &data, NULL);
	}

	template<class T>
	DX9VertexBuffer<T>::DX9VertexBuffer(const DX9VertexBuffer<T>& buffer) :
		Buffer(buffer),
		fvf(buffer.fvf),
		dataSize(buffer.dataSize),
		particleSize(buffer.particleSize)
	{
		DX9Info::getDevice()->CreateVertexBuffer(dataSize*sizeof(T), 0, fvf, DX9Info::getPool(), &data, NULL);
		void *ptrTo, *ptrFrom;
		data->Lock(0, dataSize*sizeof(T), &ptrTo, 0);
		buffer.data->Lock(0, dataSize*sizeof(T), &ptrFrom, 0);
		std::memcpy(ptrTo, ptrFrom, dataSize * sizeof(T));
		data->Unlock();
		buffer.data->Unlock();
	}

	template<class T>
	DX9VertexBuffer<T>::~DX9VertexBuffer()
	{
		SAFE_RELEASE( data );
	}

	template<class T>
	LPDIRECT3DVERTEXBUFFER9 DX9VertexBuffer<T>::getData() const
	{
		return data;
	}

	template<class T>
	LPDIRECT3DVERTEXBUFFER9 *DX9VertexBuffer<T>::getDataAddr()
	{
		return &data;
	}

	template<class T>
	inline const size_t DX9VertexBuffer<T>::getParticleSize() const
	{
		return particleSize;
	}

	template<class T>
	inline const size_t DX9VertexBuffer<T>::getDataSize() const
	{
		return dataSize;
	}

	template<class T>
	DX9VertexBuffer<T>* DX9VertexBuffer<T>::clone() const
	{
		return new DX9VertexBuffer<T>(*this);
	}

	template<class T>
	void DX9VertexBuffer<T>::swap(size_t index0,size_t index1)
	{
		if( data == NULL )
			return;
		//*
		void *ptr = NULL;
		data->Lock(0, dataSize*sizeof(T), &ptr, 0);
		T* address0 = ((T*)ptr) + index0 * particleSize;
		T* address1 = ((T*)ptr) + index1 * particleSize;
		for (size_t i = 0; i < particleSize; ++i)
			std::swap(address0[i],address1[i]);
		data->Unlock();
		//*/
	}

	template<class T>
	DX9VertexBufferCreator<T>::DX9VertexBufferCreator(DWORD fvf, size_t particleSize) :
		BufferCreator(),
		fvf(fvf),
		particleSize(particleSize)
	{}

	template<class T>
	DX9VertexBuffer<T>* DX9VertexBufferCreator<T>::createBuffer(size_t nbParticles, const Group& group) const
	{
		return new DX9VertexBuffer<T>(fvf, nbParticles, particleSize);
	}

}}

#endif
