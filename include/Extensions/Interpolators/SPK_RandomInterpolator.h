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

#ifndef H_SPK_RANDOMINTERPOLATOR
#define H_SPK_RANDOMINTERPOLATOR

namespace SPK
{
	template<typename T>
	class RandomInterpolator : public Interpolator<T>
	{
		typedef typename Arg<T>::type Tv;

	public :

		static  Ref<RandomInterpolator<T> > create(const T& minBirthValue,const T& maxBirthValue,const T& minDeathValue,const T& maxDeathValue);

		void setValues(Tv minBirthValue, Tv maxBirthValue, Tv minDeathValue, Tv maxDeathValue);

		Tv getMinBirthValue() const;
		Tv getMaxBirthValue() const;
		Tv getMinDeathValue() const;
		Tv getMaxDeathValue() const;

	public :
		spark_description(RandomInterpolator, Interpolator<T>)
		(
			spk_attribute(Quadruplet<T>, values, setValues, getMinBirthValue, getMaxBirthValue, getMinDeathValue, getMaxDeathValue);
		);

	private :

		static const size_t NB_DATA = 2;
		static const size_t BIRTH_VALUE_DATA_INDEX = 0;
		static const size_t DEATH_VALUE_DATA_INDEX = 1;

		T minBirthValue;
		T maxBirthValue;
		T minDeathValue;
		T maxDeathValue;

		RandomInterpolator<T>(
			const T& minBirthValue = T(),
			const T& maxBirthValue = T(),
			const T& minDeathValue = T(),
			const T& maxDeathValue = T());

		RandomInterpolator<T>(const RandomInterpolator<T>& interpolator);

		virtual void createData(DataSet& dataSet,const Group& group) const;

		virtual void interpolate(T* data,Group& group,DataSet* dataSet) const;
		virtual void init(T& data,Particle& particle,DataSet* dataSet) const;
	};

	typedef RandomInterpolator<Color> ColorRandomInterpolator;
	typedef RandomInterpolator<float> FloatRandomInterpolator;

	spark_description_specialization( ColorRandomInterpolator );
	spark_description_specialization( FloatRandomInterpolator );

	template<typename T>
	inline Ref<RandomInterpolator<T> > RandomInterpolator<T>::create(const T& minBirthValue,const T& maxBirthValue,const T& minDeathValue,const T& maxDeathValue)
	{
		return SPK_NEW(RandomInterpolator<T>,minBirthValue,maxBirthValue,minDeathValue,maxDeathValue);
	}

	template<typename T>
	RandomInterpolator<T>::RandomInterpolator(const T& minBirthValue,const T& maxBirthValue,const T& minDeathValue,const T& maxDeathValue) :
		Interpolator<T>(true),
		minBirthValue(minBirthValue),
		maxBirthValue(maxBirthValue),
		minDeathValue(minDeathValue),
		maxDeathValue(maxDeathValue)
	{}

	template<typename T>
	RandomInterpolator<T>::RandomInterpolator(const RandomInterpolator<T>& interpolator) :
		Interpolator<T>(interpolator),
		minBirthValue(interpolator.minBirthValue),
		maxBirthValue(interpolator.maxBirthValue),
		minDeathValue(interpolator.minDeathValue),
		maxDeathValue(interpolator.maxDeathValue)
	{}

	template<typename T>
	void RandomInterpolator<T>::setValues(Tv minBirthValue, Tv maxBirthValue, Tv minDeathValue, Tv maxDeathValue)
	{
		this->minBirthValue = minBirthValue;
		this->maxBirthValue = maxBirthValue;
		this->minDeathValue = minDeathValue;
		this->maxDeathValue = maxDeathValue;
	}

	template<typename T>
	inline typename RandomInterpolator<T>::Tv RandomInterpolator<T>::getMinBirthValue() const
	{
		return minBirthValue;
	}

	template<typename T>
	inline typename RandomInterpolator<T>::Tv RandomInterpolator<T>::getMaxBirthValue() const
	{
		return maxBirthValue;
	}

	template<typename T>
	inline typename RandomInterpolator<T>::Tv RandomInterpolator<T>::getMinDeathValue() const
	{
		return minDeathValue;
	}

	template<typename T>
	inline typename RandomInterpolator<T>::Tv RandomInterpolator<T>::getMaxDeathValue() const
	{
		return maxDeathValue;
	}

	template<typename T>
	void RandomInterpolator<T>::createData(DataSet& dataSet,const Group& group) const
	{
		dataSet.init(NB_DATA);
		ArrayData<T>* birthValuesDataPtr = SPK_NEW(ArrayData<T>,group.getCapacity(),1);
		ArrayData<T>* deathValuesDataPtr = SPK_NEW(ArrayData<T>,group.getCapacity(),1);

		dataSet.setData(BIRTH_VALUE_DATA_INDEX,birthValuesDataPtr);
		dataSet.setData(DEATH_VALUE_DATA_INDEX,deathValuesDataPtr);

		// Inits the newly created data
		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			(*birthValuesDataPtr)[i] = SPK_RANDOM(minBirthValue,maxBirthValue);
			(*deathValuesDataPtr)[i] = SPK_RANDOM(minDeathValue,maxDeathValue);
		}
	}

	template<typename T>
	void RandomInterpolator<T>::interpolate(T* data,Group& group,DataSet* dataSet) const
	{
		const ArrayData<T>& birthValuesData = SPK_GET_DATA(ArrayData<T>,dataSet,BIRTH_VALUE_DATA_INDEX);
		const ArrayData<T>& deathValuesData = SPK_GET_DATA(ArrayData<T>,dataSet,DEATH_VALUE_DATA_INDEX);

		for (GroupIterator particleIt(group); !particleIt.end(); ++particleIt)
		{
			size_t index = particleIt->getIndex();
			this->interpolateParam(data[index],deathValuesData[index],birthValuesData[index],particleIt->getEnergy());
		}
	}

	template<typename T>
	void RandomInterpolator<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		size_t index = particle.getIndex();
		data = SPK_GET_DATA(ArrayData<T>,dataSet,BIRTH_VALUE_DATA_INDEX)[index] = SPK_RANDOM(minBirthValue,maxBirthValue);
		SPK_GET_DATA(ArrayData<T>,dataSet,DEATH_VALUE_DATA_INDEX)[index] = SPK_RANDOM(minDeathValue,maxDeathValue);
	}
}

#endif

