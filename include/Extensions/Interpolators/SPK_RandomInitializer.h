//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_RANDOMINITIALIZER
#define H_SPK_RANDOMINITIALIZER

namespace SPK
{	
	template<typename T>
	class RandomInitializer : public Interpolator<T>
	{
	SPK_DEFINE_OBJECT_TEMPLATE(RandomInitializer<T>);
	SPK_DEFINE_DESCRIPTION_TEMPLATE

	public :

		static Ref<RandomInitializer<T>> create(const T& minValue = T(),const T& maxValue = T());

		void setValues(const T& minValue,const T& maxValue);
		const T& getMinValue() const;
		const T& getMaxValue() const;

	protected :

		virtual void innerImport(const IO::Descriptor& descriptor);
		virtual void innerExport(IO::Descriptor& descriptor) const;

	private :
		
		T minValue;
		T maxValue;

		RandomInitializer<T>(const T& min = T(),const T& max = T());
		RandomInitializer<T>(const RandomInitializer<T>& interpolator);

		virtual void interpolate(T* data,Group& group,DataSet* dataSet) const {}
		virtual void init(T& data,Particle& particle,DataSet* dataSet) const;
	};

	typedef RandomInitializer<Color> ColorRandomInitializer;
	typedef RandomInitializer<float> FloatRandomInitializer;

	SPK_IMPLEMENT_OBJECT_TEMPLATE(ColorRandomInitializer)
	SPK_IMPLEMENT_OBJECT_TEMPLATE(FloatRandomInitializer)

	SPK_START_DESCRIPTION_TEMPLATE(ColorRandomInitializer)
	SPK_PARENT_ATTRIBUTES(ColorInterpolator)
	SPK_ATTRIBUTE("values",ATTRIBUTE_TYPE_COLORS)
	SPK_END_DESCRIPTION

	SPK_START_DESCRIPTION_TEMPLATE(FloatRandomInitializer)
	SPK_PARENT_ATTRIBUTES(FloatInterpolator)
	SPK_ATTRIBUTE("values",ATTRIBUTE_TYPE_FLOATS)
	SPK_END_DESCRIPTION

	template<typename T>
	RandomInitializer<T>::RandomInitializer(const T& minValue,const T& maxValue) :
		Interpolator(false),
		minValue(minValue),
		maxValue(maxValue)
	{}

	template<typename T>
	RandomInitializer<T>::RandomInitializer(const RandomInitializer<T>& interpolator) :
		Interpolator(interpolator),
		minValue(interpolator.minValue),
		maxValue(interpolator.maxValue)
	{}

	template<typename T>
	inline Ref<RandomInitializer<T>> RandomInitializer<T>::create(const T& min,const T& max)
	{
		return SPK_NEW(RandomInitializer<T>,min,max);
	}

	template<typename T>
	inline void RandomInitializer<T>::setValues(const T& minValue,const T& maxValue)
	{
		this->minValue = minValue;
		this->maxValue = maxValue;
	}

	template<typename T>
	inline const T& RandomInitializer<T>::getMinValue() const
	{
		return minValue;
	}

	template<typename T>
	inline const T& RandomInitializer<T>::getMaxValue() const
	{
		return maxValue;
	}

	template<typename T>
	inline void RandomInitializer<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		data = SPK_RANDOM(minValue,maxValue);
	}

	// Explicit specialization declaration
	template<> SPK_PREFIX void RandomInitializer<float>::innerImport(const IO::Descriptor& descriptor);
	template<> SPK_PREFIX void RandomInitializer<float>::innerExport(IO::Descriptor& descriptor) const;
	template<> SPK_PREFIX void RandomInitializer<Color>::innerImport(const IO::Descriptor& descriptor);
	template<> SPK_PREFIX void RandomInitializer<Color>::innerExport(IO::Descriptor& descriptor) const;	
}

#endif
