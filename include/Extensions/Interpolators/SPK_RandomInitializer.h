//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
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
		typedef typename Arg<T>::type Tv;

	public :

		static Ref<RandomInitializer<T> > create(Tv minValue = T(),Tv maxValue = T());

		void setValues(Tv minValue, Tv maxValue);
		Tv getMinValue() const;
		Tv getMaxValue() const;

	public :
		spark_description(RandomInitializer, Interpolator)
		(
			spk_attribute(Pair<T>, values, setValues, getMinValue, getMaxValue);
		);

	private :

		T minValue;
		T maxValue;

		RandomInitializer<T>(Tv min = T(),Tv max = T());
		RandomInitializer<T>(const RandomInitializer<T>& interpolator);

		virtual void interpolate(T* data,Group& group,DataSet* dataSet) const {}
		virtual void init(T& data,Particle& particle,DataSet* dataSet) const;
	};

	typedef RandomInitializer<Color> ColorRandomInitializer;
	typedef RandomInitializer<float> FloatRandomInitializer;

	spark_description_specialization( ColorRandomInitializer );
	spark_description_specialization( FloatRandomInitializer );

	template<typename T>
	RandomInitializer<T>::RandomInitializer(Tv minValue,Tv maxValue) :
		Interpolator<T>(false),
		minValue(minValue),
		maxValue(maxValue)
	{}

	template<typename T>
	RandomInitializer<T>::RandomInitializer(const RandomInitializer<T>& interpolator) :
		Interpolator<T>(interpolator),
		minValue(interpolator.minValue),
		maxValue(interpolator.maxValue)
	{}

	template<typename T>
	inline Ref<RandomInitializer<T> > RandomInitializer<T>::create(Tv min,Tv max)
	{
		return SPK_NEW(RandomInitializer<T>,min,max);
	}

	template<typename T>
	inline void RandomInitializer<T>::setValues(Tv minValue, Tv maxValue)
	{
		this->minValue = minValue;
		this->maxValue = maxValue;
	}

	template<typename T>
	inline typename RandomInitializer<T>::Tv RandomInitializer<T>::getMinValue() const
	{
		return minValue;
	}

	template<typename T>
	inline typename RandomInitializer<T>::Tv RandomInitializer<T>::getMaxValue() const
	{
		return maxValue;
	}

	template<typename T>
	inline void RandomInitializer<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		data = SPK_RANDOM(minValue,maxValue);
	}
}

#endif
