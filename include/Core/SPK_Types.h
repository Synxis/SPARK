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

#ifndef H_SPK_TYPES
#define H_SPK_TYPES

#include <vector>
#include <string>

namespace SPK
{
	// Forward declarations
	class Vector3D;
	class Color;
	class SPKObject;
	class Action;
	class Emitter;
	class Controller;
	class Group;
	class Modifier;
	class Renderer;
	class System;
	class Zone;
	class ZonedModifier;
	template<typename T> class Interpolator;
	template<typename T> class Ref;

	/**
	* @brief Base type of value that can be used in SPARK
	*
	* @note How to declare another base type:
	*		- add the type in this enumeration
	*		- Add the type relation at the end of this file (@c spk_type_relations)
	*/
	enum BaseType
	{
		TYPE_BOOL,						/**< @brief C++ 'bool' type */
		TYPE_CHAR,						/**< @brief C++ 'char' type */
		TYPE_INT,						/**< @brief 32 bit signed integer value type */
		TYPE_UINT,						/**< @brief 32 bit unsigned integer value type */
		TYPE_FLOAT,						/**< @brief C++ 'float' type */
		TYPE_VECTOR,					/**< @brief Correspond to SPK::Vector3D */
		TYPE_COLOR,						/**< @brief Correspond to SPK::Color */
		TYPE_STRING,					/**< @brief Correspond to std::string */
		TYPE_REF_OBJECT,				/**< @brief Correspond to SPK::Ref<SPK::SPKObject> */
		TYPE_REF_ACTION,				/**< @brief Correspond to SPK::Ref<SPK::Action> */
		TYPE_REF_CONTROLLER,			/**< @brief Correspond to SPK::Ref<SPK::Controller> */
		TYPE_REF_EMITTER,				/**< @brief Correspond to SPK::Ref<SPK::Emitter> */
		TYPE_REF_GROUP,					/**< @brief Correspond to SPK::Ref<SPK::Group> */
		TYPE_REF_COLOR_INTERPOLATOR,	/**< @brief Correspond to SPK::Ref<SPK::Interpolator<SPK::Color> > */
		TYPE_REF_FLOAT_INTERPOLATOR,	/**< @brief Correspond to SPK::Ref<SPK::Interpolator<float> > */
		TYPE_REF_MODIFIER,				/**< @brief Correspond to SPK::Ref<SPK::Modifier> */
		TYPE_REF_RENDERER,				/**< @brief Correspond to SPK::Ref<SPK::Renderer> */
		TYPE_REF_SYSTEM,				/**< @brief Correspond to SPK::Ref<SPK::System> */
		TYPE_REF_ZONE,					/**< @brief Correspond to SPK::Ref<SPK::Zone> */
		TYPE_REF_ZONED_MODIFIER			/**< @brief Correspond to SPK::Ref<SPK::ZonedModifier> */
	};

	/**
	* @brief Specify how a value is organized
	*/
	enum TypeSpecifier
	{
		TS_SINGLE,			/**< @brief The base type is not modified */
		TS_PAIR,			/**< @brief The type is SPK::Pair<base_type> */
		TS_TRIPLET,			/**< @brief The type is SPK::Triplet<base_type> */
		TS_QUADRUPLET,		/**< @brief The type is SPK::Quadruplet<base_type> */
		TS_ARRAY			/**< @brief The type is std::vector<base_type> */
	};

	/**
	* @brief Used for run-time type identification.
	*/
	struct ValueType
	{
		/**
		* @brief Base type of the value
		*/
		BaseType base : 16;

		/**
		* @brief Type specifier applied on the base type to form the value type
		*/
		TypeSpecifier specifier : 16;

		/**
		* @brief Equality operator
		*/
		bool operator==(const ValueType& other) const
		{
			return base == other.base && specifier == other.specifier;
		}

		/**
		* @brief Inequality operator
		*/
		bool operator!=(const ValueType& other) const
		{
			return !(*this == other);
		}
	};

	/**
	* @brief A pair of values
	*
	* @tparam T The type of the values.
	*
	* @note T should be default-constructible.
	*/
	template<typename T>
	struct Pair
	{
		/**
		* @brief First value of the pair
		*/
		T value1;

		/**
		* @brief Second value of the pair
		*/
		T value2;

		/**
		* @brief Constructor
		*/
		Pair(const T& v1 = T(), const T& v2 = T()) :
			value1(v1),
			value2(v2)
		{
		}
	};

	/**
	* @brief A triplet of values
	*
	* @tparam T The type of the values.
	*
	* @note T should be default-constructible.
	*/
	template<typename T>
	struct Triplet
	{
		/**
		* @brief First value of the triplet
		*/
		T value1;

		/**
		* @brief Second value of the triplet
		*/
		T value2;

		/**
		* @brief Third value of the triplet
		*/
		T value3;

		/**
		* @brief Constructor
		*/
		Triplet(const T& v1 = T(), const T& v2 = T(), const T& v3 = T()) :
			value1(v1),
			value2(v2),
			value3(v3)
		{
		}
	};

	/**
	* @brief A quadruplet of values
	*
	* @tparam T The type of the values.
	*
	* @note T should be default-constructible.
	*/
	template<typename T>
	struct Quadruplet
	{
		/**
		* @brief First value of the quadruplet
		*/
		T value1;

		/**
		* @brief Second value of the quadruplet
		*/
		T value2;

		/**
		* @brief Third value of the quadruplet
		*/
		T value3;

		/**
		* @brief Fourth value of the quadruplet
		*/
		T value4;

		/**
		* @brief Constructor
		*/
		Quadruplet(const T& v1 = T(), const T& v2 = T(), const T& v3 = T(), const T& v4 = T()) :
			value1(v1),
			value2(v2),
			value3(v3),
			value4(v4)
		{
		}
	};

	/**
	* @brief Internal template code
	*/
	namespace meta
	{
		/**
		* @internal
		* @brief Defines a relation between a C++ type and a SPK::BaseType, with some extra info
		*/
		template<typename CppType, BaseType SpkType, typename CppName, typename SpkName, typename PassType = PassByRef>
		struct BaseTypeRelation
		{
			typedef CppType cppType;
			static const BaseType spkType = SpkType;
			typedef typename PassType::template transform<CppType>::type passType;

			static const char* cppName() { return CppName::get(); }
			static const char* spkName() { return SpkName::get(); }
		};

		/**
		* @internal
		* @brief A tag for the static searchs
		* @note @c find_cpp_type needs the parameter to be the C++ type to search
		*/
		struct find_cpp_type;

		/**
		* @internal
		* @brief Tag used to get the name of a type
		*/
		struct get_spktype_name;

		/**
		* @brief Traits for base type relations
		*/
		template<typename relation, int index>
		struct TypeRelationTraits
		{
			template<typename, typename, bool=true>
			struct predicate
			{
			};

			template<typename param, bool b>
			struct predicate<find_cpp_type, param, b>
			{
				static const bool value = IsSame<param, typename relation::cppType>::value;
			};

			static void call(get_spktype_name*, const char*& result, BaseType type, bool getSpkName)
			{
				if(relation::spkType == type)
				{
					if(getSpkName)
						result = relation::spkName();
					else
						result = relation::cppName();
				}
			}
		};

		/**
		* @internal
		* @brief List of all SPARK base types
		*
		* List of relation, each item is a "relation" between a C++ type
		* and its equivalent in the enum BaseType. Allows to easily declare
		* type operations, like conversions from C++ to SPARK and vice-versa.
		*/
		spk_type_relations
		(
			spk_basetype( bool,						TYPE_BOOL,						meta::PassByValue );
			spk_basetype( char,						TYPE_CHAR,						meta::PassByValue );
			spk_basetype( int,						TYPE_INT,						meta::PassByValue );
			spk_basetype( unsigned int,				TYPE_UINT,						meta::PassByValue );
			spk_basetype( float,					TYPE_FLOAT,						meta::PassByValue );
			spk_basetype( Vector3D,					TYPE_VECTOR,					meta::PassByRef );
			spk_basetype( Color,					TYPE_COLOR,						meta::PassByRef );
			spk_basetype( std::string,				TYPE_STRING,					meta::PassByRef );
			spk_basetype( Ref<SPKObject>,			TYPE_REF_OBJECT,				meta::PassByRef );
			spk_basetype( Ref<Action>,				TYPE_REF_ACTION,				meta::PassByRef );
			spk_basetype( Ref<Controller>,			TYPE_REF_CONTROLLER,			meta::PassByRef );
			spk_basetype( Ref<Emitter>,				TYPE_REF_EMITTER,				meta::PassByRef );
			spk_basetype( Ref<Group>,				TYPE_REF_GROUP,					meta::PassByRef );
			spk_basetype( Ref<Interpolator<Color> >,TYPE_REF_COLOR_INTERPOLATOR,	meta::PassByRef );
			spk_basetype( Ref<Interpolator<float> >,TYPE_REF_FLOAT_INTERPOLATOR,	meta::PassByRef );
			spk_basetype( Ref<Modifier>,			TYPE_REF_MODIFIER,				meta::PassByRef );
			spk_basetype( Ref<Renderer>,			TYPE_REF_RENDERER,				meta::PassByRef );
			spk_basetype( Ref<System>,				TYPE_REF_SYSTEM,				meta::PassByRef );
			spk_basetype( Ref<Zone>,				TYPE_REF_ZONE,					meta::PassByRef );
			spk_basetype( Ref<ZonedModifier>,		TYPE_REF_ZONED_MODIFIER,		meta::PassByRef );
		);
	}
}

#endif
