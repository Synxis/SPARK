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

#ifndef H_SPK_CLASSDESCRIPTION
#define H_SPK_CLASSDESCRIPTION

namespace SPK
{
	class ConnectionIterator;

	/**
	* @internal
	* @brief Hold static functions of an object description
	* @see DescriptionBase
	*/
	struct DescriptionTable
	{
		/** @see ClassDescription::getClassName */
		const char* (*getClassName)();

		/** @see ClassDescription::doesInherit */
		bool (*doesInherit)(const std::string&);

		/** @see ClassDescription::getAttributeNb */
		unsigned int (*getAttributeNb)();

		/** @see ClassDescription::getAttributeName */
		const char* (*getAttributeName)(unsigned int);

		/** @see ClassDescription::getAttributeType */
		ValueType (*getAttributeType)(unsigned int);

		/** @see ClassDescription::isStructured */
		bool (*isStructured)(unsigned int);

		/** @see ClassDescription::getFieldNb */
		unsigned int (*getFieldNb)(unsigned int);

		/** @see ClassDescription::getFieldName */
		const char* (*getFieldName)(unsigned int, unsigned int);

		/** @see ClassDescription::getFieldType */
		ValueType (*getFieldType)(unsigned int, unsigned int);

		/** @see ClassDescription::getControlNb */
		unsigned int (*getControlNb)();

		/** @see ClassDescription::getControlName */
		const char* (*getControlName)(unsigned int);

		/** @see ClassDescription::getControlType */
		ValueType (*getControlType)(unsigned int);

		/** @see ClassDescription::deserialize */
		void (*deserialize)(void*,IO::Deserializer&);

		/** @see ClassDescription::serialize */
		void (*serialize)(void*,IO::Serializer&);

		/** @internal */
		ConnectionItem* (*getControlConnection)(void*, unsigned int);
	};

	/**
	* @brief Handle allowing to get some meta-information on an object
	*
	* You can get the ClassDescription of only SPKObject child classes
	* that possess a description. To get the description, just call
	* getClassDescription():
	*
	* @code
	* SPK::Ref<MyObject> obj = ...;
	* SPK::ClassDescription desc = obj->getClassDescription();
	* @endcode
	*
	* This is because you cannot create a class description (you can
	* copy it, though). Be aware that it retains a pointer to the object
	* used to create it, so as long as a ClassDescription instance is
	* living, its object should not be deleted!
	*
	* Now you can use it to know the layout of the object class (ie,
	* attributes, controls), and you can modify the value of the attributes.
	*
	* @note A ClassDescription object is lightweight (sizeof(ClassDescription) == 2 * sizeof(void*)),
	* so you can pass it as a value without impact on performance.
	*/
	class ClassDescription
	{
	public:
		/**
		* @brief Returns the name of the class of the object used to create this ClassDescription
		*/
		const char* getClassName() const;

		/**
		* @brief Returns whether the object containing the description inherits from
		* the specified class.
		* @note This will return false for bases of SPKObject, since all objects with a
		* description have to inherit SPKObject.
		* @note A class is supposed to inherit itself: if you have A inheriting B inheriting C
		* (A -> B -> C), then A inherits A, B and C, B inherits B and C, and C inherits C.
		*/
		bool doesInherit(const std::string& className) const;

		/**
		* @brief Returns the number of attributes in the object
		*/
		unsigned int getAttributeNb() const;

		/**
		* @brief Returns the name of the specified attribute
		* @note If parameter is out-of-range, returns an empty string
		*/
		const char* getAttributeName(unsigned int index) const;

		/**
		* @brief Returns the type of the specified attribute
		* @note Always return a value, but this value is meaningful only
		* if the parameter is not out-of-range.
		*/
		ValueType getAttributeType(unsigned int index) const;

		/**
		* @brief Returns whether the specified attribute is structured. Structured
		* attributes have fields, whereas non-structured attributes are just simple
		* values.
		* @note If parameter is out-of-range, returns false.
		*/
		bool isAttributeStructured(unsigned int index) const;

		/**
		* @brief Returns the number of field in the specified structured attribute
		* @note If the specified attribute is not structured, or is not valid (out-of-range),
		* 0 is returned.
		*/
		unsigned int getFieldNb(unsigned int index) const;

		/**
		* @brief Returns the name of the specified field
		* @param index The attribute containing the field. It has to be valid
		* and to be structured, or else an empty string is returned.
		* @param field The field in the attribute. If this parameter is
		* out-of-range, an empty string is returned;
		*/
		const char* getFieldName(unsigned int index, unsigned int field) const;

		/**
		* @brief Returns the type of the specified field
		* @param index The attribute containing the field. It has to be valid
		* and to be structured, or else a meaningless value is returned.
		* @param field The field in the attribute. If this parameter is
		* out-of-range, a meaningless value is returned
		*/
		ValueType getFieldType(unsigned int index, unsigned int field) const;

		/**
		* @brief Returns the number of controls in the object
		*/
		unsigned int getControlNb() const;

		/**
		* @brief Returns the name of the specified control
		* @note If parameter is out-of-range, returns an empty string
		*/
		const char* getControlName(unsigned int index) const;

		/**
		* @brief Returns the type of the specified control
		* @note Always return a value, but this value is meaningful only
		* if the parameter is not out-of-range.
		*/
		ValueType getControlType(unsigned int index) const;

		/**
		* @brief Serialize the object
		*
		* @param s The serializer used to serialize the value of the
		* attributes. Controls are not serialized using this function.
		*/
		void serialize(IO::Serializer& s) const;

		/**
		* @brief Deserialize the object
		*
		* @param d The deserializer used to deserialize the value of the
		* attributes. Controls are not deserialized using this function.
		*/
		void deserialize(IO::Deserializer& d);

	private:
		// Disable creation
		template<typename,typename,typename> friend class DescriptionBase;
		ClassDescription(const DescriptionTable* dt, void* obj);

		// Connection objects
		friend class ConnectionIterator;
		ConnectionItem* getControlConnection(unsigned int id) const;

		// Function table
		const DescriptionTable* dTable;

		// Object on which to operate
		void* object;
	};

	inline ClassDescription::ClassDescription(const DescriptionTable* dt, void* obj) :
		dTable(dt),
		object(obj)
	{
	}

	inline const char* ClassDescription::getClassName() const
	{
		return dTable->getClassName();
	}

	inline bool ClassDescription::doesInherit(const std::string& className) const
	{
		return dTable->doesInherit(className);
	}

	inline unsigned int ClassDescription::getAttributeNb() const
	{
		return dTable->getAttributeNb();
	}

	inline const char* ClassDescription::getAttributeName(unsigned int index) const
	{
		return dTable->getAttributeName(index);
	}

	inline ValueType ClassDescription::getAttributeType(unsigned int index) const
	{
		return dTable->getAttributeType(index);
	}

	inline bool ClassDescription::isAttributeStructured(unsigned int index) const
	{
		return dTable->isStructured(index);
	}

	inline unsigned int ClassDescription::getFieldNb(unsigned int index) const
	{
		return dTable->getFieldNb(index);
	}

	inline const char* ClassDescription::getFieldName(unsigned int index, unsigned int field) const
	{
		return dTable->getFieldName(index, field);
	}

	inline ValueType ClassDescription::getFieldType(unsigned int index, unsigned int field) const
	{
		return dTable->getFieldType(index, field);
	}

	inline unsigned int ClassDescription::getControlNb() const
	{
		return dTable->getControlNb();
	}

	inline const char* ClassDescription::getControlName(unsigned int index) const
	{
		return dTable->getControlName(index);
	}

	inline ValueType ClassDescription::getControlType(unsigned int index) const
	{
		return dTable->getControlType(index);
	}

	inline void ClassDescription::deserialize(IO::Deserializer& d)
	{
		if(object)
			dTable->deserialize(object,d);
	}

	inline void ClassDescription::serialize(IO::Serializer& d) const
	{
		if(object)
			dTable->serialize(object,d);
	}

	inline ConnectionItem* ClassDescription::getControlConnection(unsigned int id) const
	{
		return object ? dTable->getControlConnection(object, id) : 0;
	}
}

#endif
