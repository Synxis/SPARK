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

#ifndef H_SPK_STATICDESCRIPTION
#define H_SPK_STATICDESCRIPTION

namespace SPK
{
	/**
	* @internal
	* @brief Empty description, with no attributes nor controls.
	* Provided for template metaprogramming purposes.
	*/
	class EmptyDescriptionBase
	{
	public:
		static unsigned int getControlNb() { return 0; }
		static const char* getControlName(unsigned int) { return ""; }
		static ValueType getControlType(unsigned int) { return ValueType(); }
		static unsigned int getAttributeNb() { return 0; }
		static const char* getAttributeName(unsigned int) { return ""; }
		static bool isAttributeStructured(unsigned int) { return false; }
		static ValueType getAttributeType(unsigned int) { return ValueType(); }
		static unsigned int getFieldNb(unsigned int) { return 0; }
		static const char* getFieldName(unsigned int, unsigned int) { return ""; }
		static ValueType getFieldType(unsigned int, unsigned int) { return ValueType(); }
		static void deserialize(void*, IO::Deserializer&) {}
		static void serialize(void*, IO::Serializer&) {}
		static void propagate(void*) {}
		template<typename V>
		static bool setControl(void*, unsigned int, V) { return false; }
		static bool doesInherit(const std::string&) { return false; }
		static ConnectionStatus processConnection(ConnectionParameters&) { return CONNECTION_STATUS_INVALID_ATTRIBUTE; }
		static ConnectionStatus connect(ConnectionParameters&, void*) { return CONNECTION_STATUS_INVALID_CONTROL; }
		static void processDisconnection(const ConnectionParameters&) {}
		static ConnectionItem* getControlConnection(void*, unsigned int) { return 0; }
	};

	/**
	* @internal
	* @brief Used to get the description of the specified type, with a fallback to the empty description in case of void.
	* Provided for template metaprogramming purposes.
	*/
	template<typename Parent>
	struct GetDescription
	{
		typedef typename Parent::description type;
	};

	/** @brief Default-case specialization of GetDescription */
	template<>
	struct GetDescription<void>
	{
		typedef EmptyDescriptionBase type;
	};

	/**
	* @internal
	* @brief Implementation of most of the description methods
	*/
	template<typename T, typename Obj, typename Parent>
	class DescriptionBase : public ConnectableDescription
	{
	public:
		/**
		* @brief Type of the parent description, ie the description of the parent class of Obj.
		*/
		typedef typename GetDescription<Parent>::type parentDescription;

		/**
		* @brief Creates an instance of the class containing the description.
		* @note If the class is abstract, this function will return 0.
		*/
		static SPKObject* createObject();

		/**
		* @brief Returns the ClassDescription object associated with the description.
		* @param obj The object for which to retrieve the description. It is an instance
		* of the class for which the description was generated.
		*/
		static const ClassDescription getClassDescription(Obj* obj);

		/**
		* @brief Returns the number of controls in this description
		*/
		static unsigned int getControlNb();

		/**
		* @brief Returns the name of the specified control
		* @note If parameter is out-of-range, returns an empty string
		*/
		static const char* getControlName(unsigned int);

		/**
		* @brief Returns the type of the specified control
		* @note Always return a value, but this value is meaningful only
		* if the parameter is not out-of-range.
		*/
		static ValueType getControlType(unsigned int);

		/**
		* @brief Returns the number of attributes in this description
		*/
		static unsigned int getAttributeNb();

		/**
		* @brief Returns the name of the specified attribute
		* @note If parameter is out-of-range, returns an empty string
		*/
		static const char* getAttributeName(unsigned int);

		/**
		* @brief Returns whether the specified attribute is structured
		* @note If parameter is out-of-range, returns false.
		*/
		static bool isAttributeStructured(unsigned int);

		/**
		* @brief Returns the type of the specified attribute
		* @note Always return a value, but this value is meaningful only
		* if the parameter is not out-of-range.
		*/
		static ValueType getAttributeType(unsigned int);

		/**
		* @brief Returns the number of field in the specified structured attribute
		* @note If the specified attribute is not structured, or is not valid (out-of-range),
		* 0 is returned.
		*/
		static unsigned int getFieldNb(unsigned int a);

		/**
		* @brief Returns the name of the specified field
		* @param a The attribute containing the field. It has to be valid
		* and to be structured, or else an empty string is returned.
		* @param f The field in the attribute. If this parameter is
		* out-of-range, an empty string is returned;
		*/
		static const char* getFieldName(unsigned int a, unsigned int f);

		/**
		* @brief Returns the type of the specified field
		* @param a The attribute containing the field. It has to be valid
		* and to be structured, or else a meaningless value is returned.
		* @param f The field in the attribute. If this parameter is
		* out-of-range, a meaningless value is returned
		*/
		static ValueType getFieldType(unsigned int a, unsigned int f);

		/**
		* @brief Deserialize the object
		* @note Only attributes are deserialized
		*/
		static void deserialize(Obj* o, IO::Deserializer& d);

		/**
		* @brief Serialize the object
		* @note Only attributes are serialized
		*/
		static void serialize(Obj* o, IO::Serializer& s);

		/**
		* @brief Propagates the value of all controls
		*/
		static void propagate(Obj* o);

		/**
		* @brief Sets the value of the specified control
		* @return True on success; for that, the control have to exist and
		* the types (type of the control & type of the value) have to match.
		*/
		template<typename V>
		static bool setControl(Obj* object, unsigned int ctrl, V value);

		/**
		* @brief Returns whether the object containing this description inherits from
		* the specified class.
		* @note This will return false for bases of SPKObject, since all objects with a
		* description have to inherit SPKObject.
		* @note A class is supposed to inherit itself: if you have A inheriting B inheriting C
		* (A -> B -> C), then A inherits A, B and C, B inherits B and C, and C inherits C.
		*/
		static bool doesInherit(const std::string& className);

	protected:
		static void deserializeStatic(void* o, IO::Deserializer& d);
		static void serializeStatic(void* o, IO::Serializer& s);

		static ConnectionStatus connect(ConnectionParameters& cp, Obj* object);
		static ConnectionStatus processConnection(ConnectionParameters& cp);
		static void processDisconnection(const ConnectionParameters& cp);

		virtual ConnectionStatus connect(ConnectionParameters& parameters, const Ref<SPKObject>& sender);
		virtual ConnectionStatus acceptConnection(ConnectionParameters& cp);
		virtual void disconnect(const ConnectionParameters& cp);

		static ConnectionItem* getControlConnection(void* o, unsigned int i);

	private:
		// Array of controls
		template<int n>
		struct Controls : public T::template spkCtrl<n,true> {};

		// Array of attributes
		template<int n>
		struct Attributes : public T::template spkAttr<n,true> {};

		/**
		* @brief Holds the 'queries' allowing to get / alter information on controls and attributes
		* @note This class is a template to prevent immediate instantiation
		* of the static queries, as it would result in an error (T::spkCtrl
		* and T::spkAttr are not known in the first phase).
		*/
		template<bool = true>
		struct query
		{
			typedef meta::StaticQuery<0, Controls, ControlTraits> ctrl;
			typedef meta::StaticQuery<0, Attributes, AttributeTraits> attr;
		};
	};

	template<typename T, typename Obj, typename Parent>
	inline SPKObject* DescriptionBase<T,Obj,Parent>::createObject()
	{
		return SPK::meta::Cloner<Obj>::create();
	}

	template<typename T, typename Obj, typename Parent>
	inline unsigned int DescriptionBase<T,Obj,Parent>::getControlNb()
	{
		return parentDescription::getControlNb() + query<>::ctrl::size;
	}

	template<typename T, typename Obj, typename Parent>
	inline const char* DescriptionBase<T,Obj,Parent>::getControlName(unsigned int i)
	{
		if(i < parentDescription::getControlNb())
			return parentDescription::getControlName(i);
		else
			return query<>::ctrl::template select<item_name>(i - parentDescription::getControlNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline ValueType DescriptionBase<T,Obj,Parent>::getControlType(unsigned int i)
	{
		if(i < parentDescription::getControlNb())
			return parentDescription::getControlType(i);
		else
			return query<>::ctrl::template select<item_type>(i - parentDescription::getControlNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline unsigned int DescriptionBase<T,Obj,Parent>::getAttributeNb()
	{
		return parentDescription::getAttributeNb() + query<>::attr::size;
	}

	template<typename T, typename Obj, typename Parent>
	inline const char* DescriptionBase<T,Obj,Parent>::getAttributeName(unsigned int i)
	{
		if(i < parentDescription::getAttributeNb())
			return parentDescription::getAttributeName(i);
		else
			return query<>::attr::template select<item_name>(i - parentDescription::getAttributeNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline ValueType DescriptionBase<T,Obj,Parent>::getAttributeType(unsigned int i)
	{
		if(i < parentDescription::getAttributeNb())
			return parentDescription::getAttributeType(i);
		else
			return query<>::attr::template select<item_type>(i - parentDescription::getAttributeNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline bool DescriptionBase<T,Obj,Parent>::isAttributeStructured(unsigned int i)
	{
		if(i < parentDescription::getAttributeNb())
			return parentDescription::isAttributeStructured(i);
		else
			return query<>::attr::template select<is_structured>(i - parentDescription::getAttributeNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline unsigned int DescriptionBase<T,Obj,Parent>::getFieldNb(unsigned int i)
	{
		if(i < parentDescription::getAttributeNb())
			return parentDescription::getFieldNb(i);
		else
			return query<>::attr::template select<field_nb>(i - parentDescription::getAttributeNb());
	}

	template<typename T, typename Obj, typename Parent>
	inline const char* DescriptionBase<T,Obj,Parent>::getFieldName(unsigned int a, unsigned int f)
	{
		if(a < parentDescription::getAttributeNb())
			return parentDescription::getFieldName(a, f);
		else
			return query<>::attr::template select<field_name>(a - parentDescription::getAttributeNb(), f);
	}

	template<typename T, typename Obj, typename Parent>
	inline ValueType DescriptionBase<T,Obj,Parent>::getFieldType(unsigned int a, unsigned int f)
	{
		if(a < parentDescription::getAttributeNb())
			return parentDescription::getFieldType(a, f);
		else
			return query<>::attr::template select<field_type>(a - parentDescription::getAttributeNb(), f);
	}

	template<typename T, typename Obj, typename Parent>
	inline const ClassDescription DescriptionBase<T,Obj,Parent>::getClassDescription(Obj* obj)
	{
		typedef typename Obj::description description;
		static const DescriptionTable dTable =
		{
			// Class
			&description::getClassName,
			&description::doesInherit,

			// Attributes
			&description::getAttributeNb,
			&description::getAttributeName,
			&description::getAttributeType,
			&description::isAttributeStructured,

			// Fields
			&description::getFieldNb,
			&description::getFieldName,
			&description::getFieldType,

			// Controls
			&description::getControlNb,
			&description::getControlName,
			&description::getControlType,

			// IO
			&description::deserializeStatic,
			&description::serializeStatic,

			// Connections
			&description::getControlConnection
		};
		return ClassDescription(&dTable, obj);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::deserializeStatic(void* o, IO::Deserializer& d)
	{
		parentDescription::deserialize((Parent*)((Obj*)o),d);
		query<>::attr::template process<deserialize_attr, IO::Deserializer&>(d, (Obj*)o);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::serializeStatic(void* o, IO::Serializer& s)
	{
		parentDescription::serialize((Parent*)((Obj*)o),s);
		query<>::attr::template process<serialize_attr, IO::Serializer&>(s, (Obj*)o);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::deserialize(Obj* o, SPK::IO::Deserializer& d)
	{
		deserializeStatic((void*)o, d);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::serialize(Obj* o, SPK::IO::Serializer& s)
	{
		serializeStatic((void*)o, s);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::propagate(Obj* o)
	{
		parentDescription::propagate((Parent*)o);
		query<>::ctrl::template process<propagate_ctrl>(o);
	}

	template<typename T, typename Obj, typename Parent>
	template<typename V>
	inline bool DescriptionBase<T,Obj,Parent>::setControl(Obj* object, unsigned int i, V value)
	{
		if(i < parentDescription::getControlNb())
			return parentDescription::template setControl<V>(object, i, value);
		else
			return query<>::ctrl::template select<set_ctrl_value, V>(i - parentDescription::getControlNb(), value, object);
	}

	template<typename T, typename Obj, typename Parent>
	inline bool DescriptionBase<T,Obj,Parent>::doesInherit(const std::string& className)
	{
		if(className == Obj::description::getClassName())
			return true;
		else
			return parentDescription::doesInherit(className);
	}

	template<typename T, typename Obj, typename Parent>
	inline ConnectionStatus DescriptionBase<T,Obj,Parent>::connect(ConnectionParameters& cp, Obj* object)
	{
		ConnectionStatus status = CONNECTION_STATUS_INVALID_CONTROL;
		query<>::ctrl::template process<connect_ctrl, ConnectionParameters&, ConnectionStatus&>(cp, status, object);

		if(status == CONNECTION_STATUS_INVALID_CONTROL)
			return parentDescription::connect(cp, object);
		else
			return status;
	}

	template<typename T, typename Obj, typename Parent>
	inline ConnectionStatus DescriptionBase<T,Obj,Parent>::processConnection(ConnectionParameters& cp)
	{
		ConnectionStatus status = CONNECTION_STATUS_INVALID_ATTRIBUTE;
		query<>::attr::template process<connect_attr, const ConnectionParameters&, ConnectionStatus&>(cp, status);

		if(status == CONNECTION_STATUS_INVALID_ATTRIBUTE)
			return parentDescription::processConnection(cp);
		else
			return status;
	}

	template<typename T, typename Obj, typename Parent>
	inline ConnectionStatus DescriptionBase<T,Obj,Parent>::connect(ConnectionParameters& cp, const Ref<SPKObject>& sender)
	{
		return connect(cp, (Obj*)sender.get());
	}

	template<typename T, typename Obj, typename Parent>
	inline ConnectionStatus DescriptionBase<T,Obj,Parent>::acceptConnection(ConnectionParameters& cp)
	{
		return processConnection(cp);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::processDisconnection(const ConnectionParameters& cp)
	{
		query<>::attr::template process<disconnect_attr, const ConnectionParameters&>(cp);
		parentDescription::processDisconnection(cp);
	}

	template<typename T, typename Obj, typename Parent>
	inline void DescriptionBase<T,Obj,Parent>::disconnect(const ConnectionParameters& cp)
	{
		processDisconnection(cp);
	}

	template<typename T, typename Obj, typename Parent>
	inline ConnectionItem* DescriptionBase<T,Obj,Parent>::getControlConnection(void* object, unsigned int i)
	{
		if(i < parentDescription::getControlNb())
			return parentDescription::getControlConnection(object, i);
		else
			return query<>::ctrl::template select<item_connection>(i - parentDescription::getControlNb(), (Obj*)object);
	}
}

#endif
