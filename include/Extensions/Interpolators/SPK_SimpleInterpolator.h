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

#ifndef H_SPK_SIMPLEINTERPOLATOR
#define H_SPK_SIMPLEINTERPOLATOR

namespace SPK
{
	template<typename T>
	class SimpleInterpolator : public Interpolator<T>
	{
		typedef typename Arg<T>::type Tv;

	public:

		static  Ref<SimpleInterpolator<T> > create(Tv birthValue,Tv deathValue);

		void setValues(Tv birth,Tv death);
		Tv getBirthValue() const;
		Tv getDeathValue() const;

	public :
		spark_description(SimpleInterpolator, Interpolator)
		(
			spk_attribute(Pair<T>, values, setValues, getBirthValue, getDeathValue);
		);

	private :

		T birthValue;
		T deathValue;

		SimpleInterpolator<T>(Tv birthValue = T(),Tv deathValue = T());
		SimpleInterpolator<T>(const SimpleInterpolator<T>& interpolator);

		virtual void interpolate(T* data,Group& group,DataSet* dataSet) const;
		virtual  void init(T& data,Particle& particle,DataSet* dataSet) const;
	};

	typedef SimpleInterpolator<Color> ColorSimpleInterpolator;
	typedef SimpleInterpolator<float> FloatSimpleInterpolator;

	spark_description_specialization( ColorSimpleInterpolator );
	spark_description_specialization( FloatSimpleInterpolator );

	template<typename T>
	SimpleInterpolator<T>::SimpleInterpolator(Tv birthValue,Tv deathValue) :
		Interpolator<T>(false),
		birthValue(birthValue),
		deathValue(deathValue)
	{}

	template<typename T>
	SimpleInterpolator<T>::SimpleInterpolator(const SimpleInterpolator<T>& interpolator) :
		Interpolator<T>(interpolator),
		birthValue(interpolator.birthValue),
		deathValue(interpolator.deathValue)
	{}

	template<typename T>
	inline Ref<SimpleInterpolator<T> > SimpleInterpolator<T>::create(Tv birthValue,Tv deathValue)
	{
		return SPK_NEW(SimpleInterpolator<T>,birthValue,deathValue);
	}

	template<typename T>
	inline void SimpleInterpolator<T>::setValues(Tv birth,Tv death)
	{
		this->birthValue = birth;
		this->deathValue = death;
	}

	template<typename T>
	inline typename SimpleInterpolator<T>::Tv SimpleInterpolator<T>::getBirthValue() const
	{
		return birthValue;
	}

	template<typename T>
	inline typename SimpleInterpolator<T>::Tv SimpleInterpolator<T>::getDeathValue() const
	{
		return deathValue;
	}

	template<typename T>
	inline void SimpleInterpolator<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		data = birthValue;
	}

	template<typename T>
	void SimpleInterpolator<T>::interpolate(T* data,Group& group,DataSet* dataSet) const
	{
		for (GroupIterator particleIt(group); !particleIt.end(); ++particleIt)
			interpolateParam(data[particleIt->getIndex()],deathValue,birthValue,particleIt->getEnergy());
	}
}

#endif
