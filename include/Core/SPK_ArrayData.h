//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_ARRAYDATA
#define H_SPK_ARRAYDATA

#include <algorithm> // for std:swap

#include "Core/SPK_DEF.h"
#include "Core/SPK_DataSet.h"
#include "Core/SPK_Color.h"

namespace SPK
{
	template<typename T>
	class ArrayData : public Data
	{
	public :
		
		inline ArrayData<T>(size_t nbParticles,size_t sizePerParticle);

		inline T* getData();
		inline const T* getData() const;
		
		inline T* getParticleData(size_t index);
		inline const T* getParticleData(size_t index) const;

		inline T& operator[](size_t index);
		inline const T& operator[](size_t index) const;

		inline size_t getTotalSize() const;
		inline size_t getSizePerParticle() const;

	private :

		T* data;
		size_t totalSize;
		size_t sizePerParticle;

		inline ~ArrayData<T>();

		virtual inline void swap(size_t index0,size_t index1);
	};

	typedef ArrayData<float>	FloatArrayData;
	typedef ArrayData<Color>	ColorArrayData;
	typedef ArrayData<Vector3D> Vector3DArrayData;

	template<typename T>
	inline ArrayData<T>::ArrayData(size_t nbParticles,size_t sizePerParticle) :
		Data(),
		totalSize(nbParticles * sizePerParticle),
		sizePerParticle(sizePerParticle)
	{
		data = new T[totalSize];
	}

	template<typename T>
	inline ArrayData<T>::~ArrayData()
	{
		delete[] data;
	}

	template<typename T>
	inline T* ArrayData<T>::getData()
	{
		return data;
	}

	template<typename T>
	inline const T* ArrayData<T>::getData() const
	{
		return data;
	}

	template<typename T>
	inline T* ArrayData<T>::getParticleData(size_t index)
	{
		return data + index * sizePerParticle;
	}

	template<typename T>
	inline const T* ArrayData<T>::getParticleData(size_t index) const
	{
		return data + index * sizePerParticle;
	}

	template<typename T>
	inline T& ArrayData<T>::operator[](size_t index)
	{
		return data[index];
	}
	
	template<typename T>
	inline const T& ArrayData<T>::operator[](size_t index) const
	{
		return data[index];
	}

	template<typename T>
	inline size_t ArrayData<T>::getTotalSize() const
	{
		return totalSize;
	}

	template<typename T>
	inline size_t ArrayData<T>::getSizePerParticle() const
	{
		return sizePerParticle;
	}

	template<typename T>
	inline void ArrayData<T>::swap(size_t index0,size_t index1)
	{
		index0 *= sizePerParticle;
		index1 *= sizePerParticle;
		for (size_t i = 0; i < sizePerParticle; ++i)
			std::swap(data[index0 + i],data[index1 + i]);
	}
}

#endif
