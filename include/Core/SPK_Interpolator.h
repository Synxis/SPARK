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

#ifndef H_SPK_INTERPOLATOR
#define H_SPK_INTERPOLATOR

#include "SPK_DEF.h"
#include "SPK_Color.h"
#include "SPK_Registerable.h"
#include "SPK_DataHandler.h"

namespace SPK
{
	class Group;
	class Particle;
	class DataSet;

	template<typename T>
	class Interpolator : public Registerable, public DataHandler
	{
	friend class Group;

	public :

		virtual inline ~Interpolator() {}

	protected :

		inline Interpolator(bool NEEDS_DATASET);
		inline void interpolateParam(T& result,const T& start,const T& end,float ratio) const;
		
	private :

		virtual void interpolate(T* data,Group& group,DataSet* dataSet) const = 0;
		virtual void init(T& data,Particle& particle,DataSet* dataSet) const = 0;
	};

	typedef Interpolator<Color> ColorInterpolator;
	typedef Interpolator<float> FloatInterpolator;

	template<typename T>
	inline Interpolator<T>::Interpolator(bool NEEDS_DATASET) :
		Registerable(),
		DataHandler(NEEDS_DATASET)
	{}

	template<typename T>
	inline void Interpolator<T>::interpolateParam(T& result,const T& start,const T& end,float ratio) const
	{
		result = start * (1.0f - ratio) + end * ratio;
	}

	// Specialization for Color
	template<>
	inline void Interpolator<Color>::interpolateParam(Color& result,const Color& start,const Color& end,float ratio) const
	{
		result.interpolate(start,end,ratio);
	}
}

#endif
