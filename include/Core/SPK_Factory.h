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

#ifndef H_SPK_FACTORY
#define H_SPK_FACTORY

#include <map>

namespace SPK
{
	/**
	* @brief Creates SPARK objects from a string
	*
	* This factory is for object creation only - it does not deserialize values.
	* Please note that templated types have a special handling - see registerType() for more information.
	*
	* All SPARK objects are pre-registered.
	*/
	class SPK_PREFIX Factory
	{
	private:
		typedef SPKObject* (*CreateObjectFunction)();
		typedef std::map<std::string, CreateObjectFunction> MapType;

	public:
		static Factory& getInstance();
		
		/**
		* @brief Register the specified type
		*
		* @note For templated types, the factory cannot know the template arguments
		* and it cannot create new instance of it with template arguments based on the
		* string (template = compile-time). Thus, you have to typedef every specialization
		* you want to use and register them separately. You also have to update their
		* class name (for SPARK). Example:
		* @code
		*	template<typename T>
		*	struct MyObject : public SPKObject
		*	{
		*		spark_description(MyObject, SPKObject)
		*		(
		*		);
		*	};
		*
		*	// Typedefs for more clarity
		*	typedef MyObject<Color> ColorObject;
		*	typedef MyObject<float> FloatObject;
		*	typedef MyObject<int> IntObject;
		*
		*	// Update their class name in SPARK description <- NECESSARY
		*	spark_description_specialization( ColorObject );
		*	spark_description_specialization( FloatObject );
		*	spark_description_specialization( IntObject );
		*
		*	// Registration
		*	Factory::registerType<ColorObject>();
		*	Factory::registerType<FloatObject>();
		*	Factory::registerType<IntObject>();
		* @endcode
		*/
		template<typename T>
		void registerType();

		/**
		* @brief Unregister the specified type.
		* @see registerType()
		*/
		template<typename T>
		void unregisterType();

		/**
		* @brief Creates an object based on the class name.
		* @return The object. If no type with the specified name is registered,
		* then a null reference is returned.
		*/
		Ref<SPKObject> createObject(const std::string& className) const;

		/**
		* @brief Returns the number of registered types
		*/
		unsigned int getRegistrationNb() const;

	private:
		Factory();
		~Factory();
		void registerAll();
		MapType registeredTypes;
	};

	template<typename T>
	inline void Factory::registerType()
	{
		if(registeredTypes.find(T::description::getClassName()) != registeredTypes.end())
		{
			SPK_LOG_ERROR("Factory::registerType<" << T::description::getClassName() << ">() - Type already registered");
			return;
		}
		registeredTypes[T::description::getClassName()] = &T::description::createObject;
	}

	template<typename T>
	inline void Factory::unregisterType()
	{
		MapType::iterator it = registeredTypes.find(T::description::getClassName());
		if(it != registeredTypes.end())
			registeredTypes.erase(it);
	}
}

#endif
