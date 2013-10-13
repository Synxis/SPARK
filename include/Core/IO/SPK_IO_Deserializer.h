//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 :                                                    //
//  - Julien Fryer - julienfryer@gmail.com				                        //
//  - Thibault Lescoat - info-tibo@orange.fr                                    //
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

#ifndef H_SPK_IO_DESERIALIZER
#define H_SPK_IO_DESERIALIZER

namespace SPK
{
namespace IO
{
	/*
		Deserializing = setting the value of already existing objects
		for each value:
			1. format value (retrieve number of elements for array and structured attributes)
			2. set value (deserializeAttribute)
	*/
	namespace detail
	{
		/**
		* @internal
		* @brief Generates an interface by recursively adding functions to inherit.
		*/
		template<template<int> class relations, int n = 0, bool isVoid = relations<n>::isVoid>
		struct DeserializerBase : public DeserializerBase<relations, n + 1>
		{
			using DeserializerBase<relations, n + 1>::deserializeAttribute;

			typedef typename relations<n>::type item;
			typedef typename item::cppType type;
		
			virtual void deserializeAttribute(const char* name, const BoundSetter<type>& setValue, const Context& dc) = 0;
			virtual void deserializeAttribute(const char* name, const BoundSetter<Pair<type> >& setValue, const Context& dc) = 0;
			virtual void deserializeAttribute(const char* name, const BoundSetter<Triplet<type> >& setValue, const Context& dc) = 0;
			virtual void deserializeAttribute(const char* name, const BoundSetter<Quadruplet<type> >& setValue, const Context& dc) = 0;
			virtual void deserializeAttribute(const char* name, const BoundSetter<std::vector<type> >& setValue, const Context& dc) = 0;
		};

		template<template<int> class relations, int n>
		struct DeserializerBase<relations, n, true>
		{
			void deserializeAttribute() {}
		};
	}
	
	/**
	* @brief Base interface for Deserializers.
	* @note The main function, deserializeAttribute(), is inherited from the base of Deserializer.
	* @note This class is just an interface to be used in function declaration (ex: myFunc(Deserializer& s)).
	* Do not inherit it directly!
	* @see DeserializerConcept
	*/
	struct Deserializer : public detail::DeserializerBase<meta::typeRelationsArray>
	{
		/** @brief Destructor */
		virtual ~Deserializer() {}

		/**
		* @brief Used to retrieve the number of elements in a structured attribute.
		* Structured attributes are deserialized as following:
		* @code
		* n = deserializer.sizeOfAttribute(attributeName);
		* for t = 1 to n
		*    for each field f
		*        deserialize(field f at indice t (specified in the context))
		* @endcode
		*/
		virtual unsigned int sizeOfAttribute(const char* attrName) = 0;
	};

	namespace detail
	{
		/**
		* @internal
		* @brief Implement all function recursively generated in DeserializerBase by forwarding them on
		* a template function (thanks to CRTP).
		*/
		template<template<int> class relations, typename op, int n = 0, bool isVoid = relations<n>::isVoid>
		struct DeserializerForwarder : public DeserializerForwarder<relations, op, n + 1>
		{
			typedef typename relations<n>::type item;
			typedef typename item::cppType type;

			void deserializeAttribute(const char* name, const BoundSetter<type>& setValue, const Context& dc)
			{
				((op*)this)->deserialize(name,setValue,dc);
			}

			void deserializeAttribute(const char* name, const BoundSetter<Pair<type> >& setValue, const Context& dc)
			{
				((op*)this)->deserialize(name,setValue,dc);
			}

			void deserializeAttribute(const char* name, const BoundSetter<Triplet<type> >& setValue, const Context& dc)
			{
				((op*)this)->deserialize(name,setValue,dc);
			}

			void deserializeAttribute(const char* name, const BoundSetter<Quadruplet<type> >& setValue, const Context& dc)
			{
				((op*)this)->deserialize(name,setValue,dc);
			}

			void deserializeAttribute(const char* name, const BoundSetter<std::vector<type> >& setValue, const Context& dc)
			{
				((op*)this)->deserialize(name,setValue,dc);
			}
		};

		template<template<int> class relations, typename op, int n>
		struct DeserializerForwarder<relations, op, n, true> : public Deserializer
		{
		};
	}

	/**
	* @brief Base class for deserializers.
	* To create a deserializer, you have to inherit it, and define the template function 'deserialize', example:
	* @code
	* class MyDeserializer : public SPK::DeserializerConcept<MyDeserializer>
	* {
	* public:
	*     template<typename T>
	*     void deserialize(const char* attributeName, const SPK::BoundSetter<T> setValue, const SPK::IO::Context& context)
	*     {
	*         // To 'return' the value, just do the following:
	*         // setValue(deserializedValue);
	*     }
	* };
	* @endcode
	*/
	template<class op>
	struct DeserializerConcept : public detail::DeserializerForwarder<meta::typeRelationsArray, op>
	{
	};
}
}

#endif
