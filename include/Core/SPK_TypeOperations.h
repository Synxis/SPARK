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

#ifndef H_SPK_TYPE_OPERATIONS
#define H_SPK_TYPE_OPERATIONS

namespace SPK
{
	/**
	* @brief Extracts type specifier from a C++ type. Also extract base C++ type
	*
	* Example:
	* @code
	*	SeparateType<int>::specifier = TS_SINGLE
	*	SeparateType<Pair<float> >::specifier = TS_PAIR
	*	SeparateType<Pair<float> >::base = float
	*	SeparateType<std::vector<Vector3D> >::specifier = TS_ARRAY
	* @endcode
	*/
	template<typename T>
	struct SeparateType
	{
		/** @brief Result type specifier */
		static const TypeSpecifier specifier = TS_SINGLE;

		/** @brief Result base C++ type */
		typedef T base;
	};

	/** @brief Pair specialization of SPK::SeparateType<T> */
	template<typename T>
	struct SeparateType<Pair<T> >
	{
		static const TypeSpecifier specifier = TS_PAIR;
		typedef T base;
	};

	/** @brief Triplet specialization of SPK::SeparateType<T> */
	template<typename T>
	struct SeparateType<Triplet<T> >
	{
		static const TypeSpecifier specifier = TS_TRIPLET;
		typedef T base;
	};

	/** @brief Quadruplet specialization of SPK::SeparateType<T> */
	template<typename T>
	struct SeparateType<Quadruplet<T> >
	{
		static const TypeSpecifier specifier = TS_QUADRUPLET;
		typedef T base;
	};

	/** @brief Pair specialization of SPK::SeparateType<T> */
	template<typename T>
	struct SeparateType<std::vector<T> >
	{
		static const TypeSpecifier specifier = TS_ARRAY;
		typedef T base;
	};

	/**
	* @brief Find the appropriate type for parameters, given a type
	*
	* @tparam T The type to use in function parameter. Simple types will be
	* left unchanged, but complex types will be changed in their const-ref
	* form. Examples:
	* @code
	*	Arg<int>::type = int,
	*	Arg<std::vector<...> >::type = const std::vector<...>&
	* @endcode
	*
	* @note The general case add a const ref to T; specializations simply forward
	* the type. Only 'int', 'unsigned int', 'char', 'bool' and 'float' are specialized.
	*/
	template<typename T>
	class Arg
	{
	private:
		// A base type
		template<bool isBaseType = (SeparateType<T>::specifier == TS_SINGLE), bool unused = true>
		struct ArgType
		{
			typedef typename meta::EraseEnum<T>::type realType;
			typedef typename meta::typeRelations::search<meta::find_cpp_type, realType>::result searchResult;
			typedef typename searchResult::passType passType;
			typedef typename meta::SelectType<T, passType, meta::EraseEnum<T>::enumErased>::type type;
		};

		// Not a base type
		template<bool b>
		struct ArgType<false, b>
		{
			typedef const T& type;
		};

	public:
		/**
		* @brief Result type
		*/
		typedef typename ArgType<>::type type;
	};

	/**
	* @brief Extracts base type from a C++ type
	*
	* @note Works only for base C++ types accepted by SPARK, undefined
	* for other types.
	*
	* Example:
	*	BaseFromCPP<int>::base = TYPE_INT
	*	BaseFromCPP<float>::base = TYPE_FLOAT
	*	BaseFromCPP<Vector3D>::base = TYPE_VECTOR
	*
	* Note: enums are treated as ints. Thus, BaseFromCPP<MyEnum>::base = TYPE_INT
	*/
	template<typename T>
	struct BaseFromCPP
	{
		typedef typename meta::EraseEnum<T>::type realType;
		typedef typename meta::typeRelations::search<meta::find_cpp_type, realType>::result searchResult;
		static const BaseType base = searchResult::spkType;
	};

	/**
	* @brief Extract the SPARK value type of a C++ type
	*
	* @tparam T Type to extract SPARK type from.
	*/
	template<typename T>
	struct ToSPKType
	{
		/**
		* @brief Result base type
		*/
		static const BaseType base = BaseFromCPP<typename SeparateType<T>::base>::base;

		/**
		* @brief Result type specifier
		*/
		static const TypeSpecifier specifier = SeparateType<T>::specifier;

		/**
		* @brief Result value type
		*/
		static const ValueType type;
	};

	// Static constant definition
	template<typename T>
	const ValueType ToSPKType<T>::type = { base, specifier };

	/**
	* @brief Extract the SPARK value type of a C++ type, convenience specialization for void
	*
	* @note Only the 'type' field is present in this specialization. Moreover, its value is
	* meaningless and not reliable. It is here only for template metaprogramming purposes.
	*/
	template<>
	struct SPK_PREFIX ToSPKType<void>
	{
		/** @brief Result value type */
		static const ValueType type;
	};

	/**
	* @internal
	* @brief Used to generate a "switch" that will "convert" runtime types to compile time template argument
	*/
	template<template<int> class relations, int n = 0, bool isVoid = relations<n>::isVoid>
	struct RuntimeTypeSwitch : public RuntimeTypeSwitch<relations, n + 1>
	{
		typedef typename relations<n>::type item;
		typedef typename item::cppType baseType;
		
		template<typename T>
		static void select(ValueType type, T templatedFunctor)
		{
			if(type.base == item::spkType)
			{
				if(type.specifier == TS_SINGLE)
					templatedFunctor.call<baseType>();
				else if(type.specifier == TS_PAIR)
					templatedFunctor.call<Pair<baseType> >();
				else if(type.specifier == TS_TRIPLET)
					templatedFunctor.call<Triplet<baseType> >();
				else if(type.specifier == TS_QUADRUPLET)
					templatedFunctor.call<Quadruplet<baseType> >();
				else if(type.specifier == TS_ARRAY)
					templatedFunctor.call<std::vector<baseType> >();
			}
			else
			{
				RuntimeTypeSwitch<relations, n + 1>::template select<T>(type, templatedFunctor);
			}
		}
	};

	template<template<int> class relations, int n>
	struct RuntimeTypeSwitch<relations, n, true>
	{
		template<typename T>
		static void select(ValueType type, T templatedFunctor)
		{
		}
	};

	/**
	* @brief Calls a templated functor with the run-time type
	*/
	template<typename T>
	inline void selectTemplate(ValueType type, T templatedFunctor)
	{
		RuntimeTypeSwitch<meta::typeRelationsArray>::select<T>(type, templatedFunctor);
	}
	
	/**
	* @brief Returns the name of a BaseType
	*/
	SPK_PREFIX const char* getBaseTypeName(BaseType type, bool spkName);

	/**
	* @brief Returns the name of a Type specifier
	*/
	SPK_PREFIX const char* getTypeSpecifierName(TypeSpecifier ts, bool spkName);

	/**
	* @brief Returns the name of a ValueType
	*/
	SPK_PREFIX std::string getValueTypeName(ValueType type, bool spkName);
}

#endif
