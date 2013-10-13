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

#ifndef H_SPK_TRAITS
#define H_SPK_TRAITS

/**
* @file
* @see StaticQuery
*/
namespace SPK
{
	// Tags
	struct propagate_ctrl;
	struct connect_ctrl;
	struct set_ctrl_value { typedef bool R; };
	struct item_name { typedef const char* R; };
	struct item_type { typedef ValueType R; };
	struct item_connection { typedef ConnectionItem* R; };
	struct is_structured { typedef bool R; };
	struct field_nb { typedef unsigned int R; };
	struct field_name { typedef const char* R; };
	struct field_type { typedef ValueType R; };
	struct deserialize_attr;
	struct serialize_attr;
	struct deserialize_field;
	struct serialize_field;
	struct connect_attr;
	struct connect_field;
	struct disconnect_attr;
	struct disconnect_field;
	struct field_element_removed;
	struct field_elements_cleared;

	/**
	* @internal
	* @brief Used to simplify the traits (ie, this is the default case that can be handled
	* the same way as a 'normal' case).
	*/
	template<typename obj>
	struct DefaultControl
	{
		typedef void objectType;
		static const ValueType valueType;
		static const char* asName() { return ""; }
		template<typename V>
		static bool set(void*, V) { return false; }
		static ConnectionItem* self(void*) { return 0; }
	};
	template<typename obj>
	const ValueType DefaultControl<obj>::valueType = ToSPKType<void>::type;

	/**
	* @internal
	* @brief Used to simplify the traits (ie, this is the default case that can be handled
	* the same way as a 'normal' case).
	*/
	template<typename obj>
	struct DefaultAttribute
	{
		typedef obj objectType;
		static const ValueType valueType;
		static const bool isStructured = false;
		static const char* asName() { return ""; }
		static unsigned int getFieldNb() { return 0; }
		static const char* getFieldName(unsigned int) { return ""; }
		static ValueType getFieldType(unsigned int) { return ValueType(); }
	};
	template<typename obj>
	const ValueType DefaultAttribute<obj>::valueType = ToSPKType<void>::type;

	/**
	* @internal
	* @brief Used to simplify the traits (ie, this is the default case that can be handled
	* the same way as a 'normal' case).
	*/
	template<typename obj>
	struct DefaultField
	{
		typedef obj objectType;
		static const ValueType valueType;
		static const char* asName() { return ""; }
	};
	template<typename obj>
	const ValueType DefaultField<obj>::valueType = ToSPKType<void>::type;

	/**
	* @internal
	*/
	template<typename T>
	struct StaticArrayTraits
	{
		static const char* get(item_name*)
		{
			return T::asName();
		}

		static ValueType get(item_type*)
		{
			return T::valueType;
		}
	};

	/**
	* @internal
	*/
	template<typename ctrl, int>
	struct ControlTraits : public StaticArrayTraits<ctrl>
	{
		using StaticArrayTraits<ctrl>::get;
		typedef typename ctrl::objectType objectType;

		static ConnectionItem* get(item_connection*, objectType* obj)
		{
			return ctrl::self(obj);
		}

		static void call(connect_ctrl*, ConnectionParameters& cp, ConnectionStatus& s, objectType* obj)
		{
			if(cp.control == ctrl::asName())
				s = ctrl::connect(cp, obj);
		}

		static void call(propagate_ctrl*, objectType* obj)
		{
			ctrl::propagate(obj);
		}

		template<typename V>
		static bool get(set_ctrl_value*, V value, objectType* obj)
		{
			return ctrl::set(obj, value);
		}
	};

	/**
	* @internal
	*/
	template<typename attr, int>
	struct AttributeTraits : public StaticArrayTraits<attr>
	{
		using StaticArrayTraits<attr>::get;
		
		typedef typename attr::objectType objectType;

		static bool get(is_structured*)
		{
			return attr::isStructured;
		}

		static unsigned int get(field_nb*)
		{
			return attr::getFieldNb();
		}

		static const char* get(field_name*, unsigned int i)
		{
			return attr::getFieldName(i);
		}

		static ValueType get(field_type*, unsigned int i)
		{
			return attr::getFieldType(i);
		}

		static void call(deserialize_attr*, IO::Deserializer& d, objectType* obj)
		{
			attr::deserialize(d,obj);
		}

		static void call(serialize_attr*, IO::Serializer& s, objectType* obj)
		{
			attr::serialize(s,obj);
		}

		static void call(connect_attr*, const ConnectionParameters& cp, ConnectionStatus& s)
		{
			if(cp.attribute == attr::asName())
				s = attr::connect(cp);
		}

		static void call(disconnect_attr*, const ConnectionParameters& cp)
		{
			if(cp.attribute == attr::asName())
				attr::disconnect(cp);
		}
	};

	/**
	*
	*/
	template<typename field, int n>
	struct FieldsTraits : public StaticArrayTraits<field>
	{
		using StaticArrayTraits<field>::get;
		
		typedef typename field::objectType objectType;

		static void call(connect_field*, const ConnectionParameters& cp, ConnectionStatus& s)
		{
			if(cp.field == field::asName())
				s = field::connect(cp);
		}

		static void call(disconnect_field*, const ConnectionParameters& cp)
		{
			if(cp.field == field::asName() || cp.field == "")
				field::disconnect(cp);
		}

		static void call(field_element_removed*, objectType* obj, unsigned int id)
		{
			field::elementRemoved(obj, id);
		}

		static void call(field_elements_cleared*, objectType* obj)
		{
			field::elementsCleared(obj);
		}

		static void call(serialize_field*, IO::Serializer& s, objectType* obj, IO::StructuredAttributeContext c)
		{
			c.fieldIndex = (unsigned int)n;
			field::serialize(s, obj, c);
		}

		static void call(deserialize_field*, IO::Deserializer& d, objectType* obj, IO::StructuredAttributeContext c)
		{
			c.fieldIndex = (unsigned int)n;
			field::deserialize(d, obj, c);
		}
	};
}

#endif

