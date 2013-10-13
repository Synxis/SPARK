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

#ifndef H_SPK_DEFAULTINITIALIZER
#define H_SPK_DEFAULTINITIALIZER

namespace SPK
{
	template<typename T>
	class DefaultInitializer : public Interpolator<T>
	{
		typedef typename Arg<T>::type Tv;

	public :

		static  Ref<DefaultInitializer<T> > create(Tv value);

		void setDefaultValue(Tv value);
		Tv getDefaultValue() const;

	public :
		spark_description(DefaultInitializer, Interpolator)
		(
			spk_attribute(T, value, setDefaultValue, getDefaultValue);
		);

	private :

		T defaultValue;

		DefaultInitializer<T>(Tv value = T());
		DefaultInitializer<T>(const DefaultInitializer<T>& interpolator);

		virtual  void interpolate(T* data,Group& group,DataSet* dataSet) const {}
		virtual  void init(T& data,Particle& particle,DataSet* dataSet) const;
	};

	template<typename T>
	DefaultInitializer<T>::DefaultInitializer(Tv value) :
		Interpolator<T>(false),
		defaultValue(value)
	{}

	template<typename T>
	DefaultInitializer<T>::DefaultInitializer(const DefaultInitializer<T>& interpolator) :
		Interpolator<T>(interpolator),
		defaultValue(interpolator.defaultValue)
	{}

	template<typename T>
	inline Ref<DefaultInitializer<T> > DefaultInitializer<T>::create(Tv value)
	{
		return SPK_NEW(DefaultInitializer<T>,value);
	}

	template<typename T>
	inline void DefaultInitializer<T>::setDefaultValue(Tv value)
	{
		defaultValue = value;
	}

	template<typename T>
	inline typename DefaultInitializer<T>::Tv DefaultInitializer<T>::getDefaultValue() const
	{
		return defaultValue;
	}

	template<typename T>
	inline void DefaultInitializer<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		data = defaultValue;
	}

	// Typedefs
	typedef DefaultInitializer<Color> ColorDefaultInitializer;
	typedef DefaultInitializer<float> FloatDefaultInitializer;

	spark_description_specialization( ColorDefaultInitializer );
	spark_description_specialization( FloatDefaultInitializer );
}

#endif
