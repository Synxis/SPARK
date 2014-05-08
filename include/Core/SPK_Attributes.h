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

#ifndef H_SPK_ATTRIBUTES
#define H_SPK_ATTRIBUTES

namespace SPK
{
	/**
	*
	*/
	template<typename T, typename obj, bool isArray, bool isStruct, typename name>
	struct AttributeBase
	{
		typedef T type;

		static const ValueType valueType;

		static const bool isArrayAttribute = isArray;

		static const bool isStructured = isStruct;

		typedef obj objectType;

		static const char* asName()
		{
			return name::get();
		}

		static unsigned int getFieldNb()
		{
			return 0;
		}

		static const char* getFieldName(unsigned int)
		{
			return "";
		}

		static ValueType getFieldType(unsigned int)
		{
			return ValueType();
		}
	};

	template<typename T, typename obj, bool isArray, bool isStruct, typename name>
	const ValueType AttributeBase<T,obj,isArray,isStruct,name>::valueType = ToSPKType<T>::type;

	/**
	*
	*/
	template<typename T, class obj, typename Setter<T,obj>::type func, typename getters,
		typename name, void* (*attrObj)(obj*)>
	struct Attribute : public AttributeBase<T,obj,false,false,name>, public AttributeConnection<T, obj, Attribute<T, obj, func, getters, name, attrObj> >
	{
	public:
		using AttributeBase<T,obj,false,false,name>::asName;

	private:
		typedef AttributeConnection<T, obj, Attribute<T, obj, func, getters, name, attrObj> > ConnectionType;

	public:
		// Static
		static void set(obj* object, typename Arg<T>::type value)
		{
			Setter<T,obj>::template call<func>(object,value);
		}

		static T get(obj* object)
		{
			return getters::call(object);
		}

		static void serialize(IO::Serializer& s, obj* o)
		{
			s.serializeAttribute(asName(), get(o), IO::standardContext(o));
		}

		static void deserialize(IO::Deserializer& d, obj* o)
		{
			d.deserializeAttribute(asName(), SetterDelegate<T,obj,func>(o), IO::standardContext(o));
		}

	private:
		template<typename attr, int> friend struct AttributeTraits;
		static ConnectionStatus connect(const ConnectionParameters& cp)
		{
			// Get instance of attribute for the receiver
			Attribute* self = (Attribute*)attrObj((obj*)cp.receiver.get());
			return self->ConnectionType::connect(cp);
		}

		static void disconnect(const ConnectionParameters& cp)
		{
			Attribute* self = (Attribute*)attrObj((obj*)cp.receiver.get());
			self->remove();
		}
	};

	/**
	*
	*/
	template<typename T, class obj, typename Setter<T,obj>::type _add, typename Setter<T,obj>::type _remove,
		void (obj::* _clear)(), typename Arg<T>::type (obj::* _getAt)(size_t) const, size_t (obj::* _getSize)() const,
		typename name, void* (*attrObj)(obj*)>
	struct ArrayAttribute : public AttributeBase<std::vector<T>,obj,true,false,name>, public AttributeConnection<std::vector<T>, obj, ArrayAttribute<T, obj, _add, _remove, _clear, _getAt, _getSize, name, attrObj> >
	{
	public:
		using AttributeBase<std::vector<T>,obj,true,false,name>::asName;

	private:
		typedef AttributeConnection<std::vector<T>, obj, ArrayAttribute<T, obj, _add, _remove, _clear, _getAt, _getSize, name, attrObj> > ConnectionType;

	public:
		static void set(obj* object, const std::vector<T>& value)
		{
			(object->*_clear)();
			for(size_t t = 0; t < value.size(); t++)
				(object->*_add)(value[t]);
		}

		static std::vector<T> get(obj* object)
		{
			std::vector<T> vect;
			for(size_t t = 0; t < (object->*_getSize)(); t++)
				vect.push_back((object->*_getAt)(t));
			return vect;
		}

		static void serialize(IO::Serializer& s, obj* object)
		{
			s.serializeAttribute(asName(), get(object), IO::standardContext(object));
		}

		static void deserialize(IO::Deserializer& d, obj* object)
		{
			(object->*_clear)();
			d.deserializeAttribute(asName(), FreeSetterDelegate<std::vector<T>, obj, &ArrayAttribute::set>(object), IO::standardContext(object));
		}

	private:
		template<typename attr, int> friend struct AttributeTraits;
		static inline ConnectionStatus connect(const ConnectionParameters& cp)
		{
			// Get instance of attribute for the receiver
			ArrayAttribute* self = (ArrayAttribute*)attrObj((obj*)cp.receiver.get());
			return self->ConnectionType::connect(cp);
		}

		static void disconnect(const ConnectionParameters& cp)
		{
			ArrayAttribute* self = (ArrayAttribute*)attrObj((obj*)cp.receiver.get());
			self->remove();
		}
	};

	/**
	*
	*/
	template<typename T, class obj, typename Setter<T,obj>::template extra<unsigned int>::type func,
		typename getters, typename name, typename structName, void* (*fieldObj)(obj*)>
	struct FieldAttribute : public AttributeBase<T,obj,false,false,name>, protected FieldConnectionHead<T,obj,FieldAttribute<T,obj,func,getters,name,structName,fieldObj> >
	{
		typedef FieldConnectionHead<T,obj,FieldAttribute<T,obj,func,getters,name,structName,fieldObj> > ConnectionType;
		using AttributeBase<T,obj,false,false,name>::asName;

		// Static
		static inline const char* structureName()
		{
			return structName::get();
		}

		static inline void set(obj* object, unsigned int id, typename Arg<T>::type value)
		{
			Setter<T,obj>::template extra<unsigned int>::template call<func>(object,id,value);
		}

		static inline T get(obj* object, unsigned int id)
		{
			return getters::call(object, id);
		}

		static inline void serialize(IO::Serializer& s, obj* o, IO::StructuredAttributeContext sc)
		{
			sc.fieldName = asName();
			s.serializeAttribute(structName::get(), get(o, sc.id), IO::fieldContext(o, sc));
		}

		static inline void deserialize(IO::Deserializer& d, obj* o, IO::StructuredAttributeContext dc)
		{
			dc.fieldName = asName();
			d.deserializeAttribute(structName::get(), IndexedSetterDelegate<T,obj,func>(o, dc.id), IO::fieldContext(o, dc));
		}

		static void elementRemoved(obj* object, unsigned int id)
		{
			ConnectionType* self = (ConnectionType*)fieldObj(object);
			self->removeElement(id);
		}

		static void elementsCleared(obj* object)
		{
			ConnectionType* self = (ConnectionType*)fieldObj(object);
			self->disconnectAll();
		}

	private:
		template<typename, int> friend struct FieldsTraits;

		static inline ConnectionStatus connect(const ConnectionParameters& cp)
		{
			// Get instance of attribute for the receiver
			FieldAttribute* self = (FieldAttribute*)fieldObj((obj*)cp.receiver.get());
			return self->ConnectionType::connect(cp);
		}

		static void disconnect(const ConnectionParameters& cp)
		{
			FieldAttribute* self = (FieldAttribute*)fieldObj((obj*)cp.receiver.get());
			self->ConnectionType::disconnect(cp);
		}
	};

	/*
		Structured attribute

		attr
			[0..n] elements, of structure S (specified by its fields)

		control:
			* control on values of fields
			* control on attribute (add/remove/clear)

		Add a value:
			* from SPK:
				create is called, user code creates an element. The new element has id numElems-1
			* from user code:
				no function to call.
			* add rule:
				the id organization should always been guaranteed : [0..n]. It is only possible to add elements at the end.

		Delete a value:
			* from SPK:
				SPK directly calls the user callback. The elementRemoved(id) function is called by the user. SPK will ensure
				only valid ids are deleted
			* from user code:
				user delete an element and must update the structured attribute by calling elementRemoved(id). It will ensure
				connections are properly updated. This call should be in the removeFunc given by the user.
			* delete rule:
				[0..n] becomes [0..n-1] after the deletion. The last element takes the id of the deleted element.
				Example:
					Ids: [0..n].
					Element m is removed.
					The id of element n is changed and becomes m.
					The total size is decremented
			* note:
				clearing the attribute (ie, deleting all values at once, for example std::vector::clear()) invalidates all connections,
				so the user should call elementsCleared() of the attribute.
	*/
	/**
	*
	*/
	template<typename sAttr, typename sName, typename obj, void (obj::* _create)(),
		void (obj::* _remove)(unsigned int), void (obj::* _clear)(), unsigned int (obj::* _getSize)() const,
		unsigned int instantiationId>
	class StructuredAttributeBase : public AttributeBase<void,obj,false,true,sName>
	{
	protected:
		typedef sName attrName;
		struct instantiation
		{
			typedef sAttr type;
			static const unsigned int index = instantiationId - 1;
		};

	public:
		template<int n>
		struct Fields : public sAttr::template spkField<n>
		{
		};

		/**
		* @brief Holds the 'queries' allowing to get / alter information on fields
		* @note This class is a template to prevent immediate instantiation
		* of the static queries, as it would result in an error (sAttr::spkField
		* is not known in the first phase).
		*/
		template<bool = true>
		struct query
		{
			typedef meta::StaticQuery<0, Fields, FieldsTraits> field;
		};

	public:
		using AttributeBase<void,obj,false,true,sName>::asName;

		static unsigned int getFieldNb()
		{
			return query<>::field::size;
		}

		static const char* getFieldName(unsigned int i)
		{
			return query<>::field::template select<item_name>(i);
		}

		static ValueType getFieldType(unsigned int i)
		{
			return query<>::field::template select<item_type>(i);
		}

		static void serialize(IO::Serializer& s, obj* o)
		{
			unsigned int size = (o->*_getSize)();
			if(size == 0)
			{
				IO::StructuredAttributeContext sc(0, 0, getFieldNb());
				s.emptyAttribute(asName(),IO::fieldContext(o, sc));
			}
			else
				for(unsigned int t = 0; t < size; t++)
					query<>::field::template process<serialize_field, IO::Serializer&>(s, o, IO::StructuredAttributeContext(size,t,getFieldNb()));
		}

		static void deserialize(IO::Deserializer& d, obj* o)
		{
			// Resize attribute
			unsigned int size = d.sizeOfAttribute(asName());
			(o->*_clear)();
			for(unsigned int t = 0; t < size; t++)
				(o->*_create)();

			// Deserialize it
			for(unsigned int t = 0; t < size; t++)
				query<>::field::template process<deserialize_field, IO::Deserializer&>(d, o, IO::StructuredAttributeContext(size,t,getFieldNb()));
		}

		static void elementRemoved(obj* object, unsigned int id)
		{
			query<>::field::template process<field_element_removed>(object, id);
		}

		static void elementsCleared(obj* object)
		{
			query<>::field::template process<field_elements_cleared>(object);
		}

	private:
		template<typename attr, int> friend struct AttributeTraits;
		static inline ConnectionStatus connect(const ConnectionParameters& cp)
		{
			obj* object = (obj*)cp.receiver.get();
			if(cp.id >= (object->*_getSize)())
				return CONNECTION_STATUS_INVALID_ID;

			ConnectionStatus status = CONNECTION_STATUS_INVALID_FIELD;
			query<>::field::template process<connect_field, const ConnectionParameters&, ConnectionStatus&>(cp, status);

			return status;
		}

		static inline void disconnect(const ConnectionParameters& cp)
		{
			query<>::field::template process<disconnect_field, const ConnectionParameters&>(cp);
		}
	};
}

#endif

