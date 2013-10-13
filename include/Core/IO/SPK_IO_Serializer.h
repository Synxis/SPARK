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

#ifndef H_SPK_IO_SERIALIZER
#define H_SPK_IO_SERIALIZER

namespace SPK
{
namespace IO
{
	namespace detail
	{
		/**
		* @internal
		* @brief Generates an interface by recursively adding functions to inherit.
		*/
		template<template<int> class relations, int n = 0, bool isVoid = relations<n>::isVoid>
		struct SerializerBase : public SerializerBase<relations, n + 1>
		{
			using SerializerBase<relations, n + 1>::serializeAttribute;

			typedef typename relations<n>::type item;
			typedef typename item::cppType type;
		
			virtual void serializeAttribute(const char* name, const type& value, const Context& sc) = 0;
			virtual void serializeAttribute(const char* name, const Pair<type>& value, const Context& sc) = 0;
			virtual void serializeAttribute(const char* name, const Triplet<type>& value, const Context& sc) = 0;
			virtual void serializeAttribute(const char* name, const Quadruplet<type>& value, const Context& sc) = 0;
			virtual void serializeAttribute(const char* name, const std::vector<type>& value, const Context& sc) = 0;
		};

		template<template<int> class relations, int n>
		struct SerializerBase<relations, n, true>
		{
			void serializeAttribute() {}
		};
	}
	
	/**
	* @brief Base interface for Serializers.
	* @note The main function, serializeAttribute(), is inherited from the base of Serializer.
	* @note This class is just an interface to be used in function declaration (ex: myFunc(Serializer& s)).
	* Do not inherit it directly!
	* @see SerializerConcept
	*/
	struct Serializer : public detail::SerializerBase<meta::typeRelationsArray>
	{
		/** @brief Destructor */
		virtual ~Serializer() {}

		/**
		* @brief Called when a structured attribute have no values
		*
		* @param name The name of the attribute
		*/
		virtual void emptyAttribute(const char* name, const Context& sc) {}
	};

	namespace detail
	{
		/**
		* @internal
		* @brief Implement all function recursively generated in SerializerBase by forwarding them on
		* a template function (thanks to CRTP).
		*/
		template<template<int> class relations, typename op, int n = 0, bool isVoid = relations<n>::isVoid>
		struct SerializerForwarder : public SerializerForwarder<relations, op, n + 1>
		{
			typedef typename relations<n>::type item;
			typedef typename item::cppType type;

			void serializeAttribute(const char* name, const type& value, const Context& sc)
			{
				((op*)this)->serialize(name,value,sc);
			}

			void serializeAttribute(const char* name, const Pair<type>& value, const Context& sc)
			{
				((op*)this)->serialize(name,value,sc);
			}

			void serializeAttribute(const char* name, const Triplet<type>& value, const Context& sc)
			{
				((op*)this)->serialize(name,value,sc);
			}

			void serializeAttribute(const char* name, const Quadruplet<type>& value, const Context& sc)
			{
				((op*)this)->serialize(name,value,sc);
			}

			void serializeAttribute(const char* name, const std::vector<type>& value, const Context& sc)
			{
				((op*)this)->serialize(name,value,sc);
			}
		};

		template<template<int> class relations, typename op, int n>
		struct SerializerForwarder<relations, op, n, true> : public Serializer
		{
		};
	}

	/**
	* @brief Base class for serializers.
	* To create a serializer, you have to inherit it, and define the template function 'serialize', example:
	* @code
	* class MySerializer : public SPK::SerializerConcept<MySerializer>
	* {
	* public:
	*     template<typename T>
	*     void serialize(const char* attributeName, T value, const SPK::IO::Context& context)
	*     {
	*         // ...
	*     }
	* };
	* @endcode
	*/
	template<class op>
	struct SerializerConcept : public detail::SerializerForwarder<meta::typeRelationsArray, op>
	{
	};
}
}

#endif
