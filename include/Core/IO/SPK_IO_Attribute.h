//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to  anyone to use this software for any purpose,		//
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

#ifndef H_SPK_ATTRIBUTE
#define H_SPK_ATTRIBUTE

#include <string>
#include <vector>

namespace SPK
{
	class SPKObject;

namespace IO
{
	class Descriptor;
	
	/** @brief Constants defining attribute's types */
	enum AttributeType
	{
		ATTRIBUTE_TYPE_CHAR,		/**< @brief the value is a char */
		ATTRIBUTE_TYPE_BOOL,		/**< @brief the value is a boolean */
		ATTRIBUTE_TYPE_INT32,		/**< @brief the value is an int 32 bits */
		ATTRIBUTE_TYPE_UINT32,		/**< @brief the value is an unsigned int 32 bits */
		ATTRIBUTE_TYPE_FLOAT,		/**< @brief the value is a float */
		ATTRIBUTE_TYPE_VECTOR,		/**< @brief the value is a Vector3D */
		ATTRIBUTE_TYPE_COLOR,		/**< @brief the value is a Color */
		ATTRIBUTE_TYPE_STRING,		/**< @brief the value is a std::string */
		ATTRIBUTE_TYPE_REF,			/**< @brief the value is a Ref<SPKObject> */
		ATTRIBUTE_TYPE_CHARS,		/**< @brief the value is an array of char */
		ATTRIBUTE_TYPE_BOOLS,		/**< @brief the value is an array of boolean */		
		ATTRIBUTE_TYPE_INT32S,		/**< @brief the value is an array of int 32 bits */
		ATTRIBUTE_TYPE_UINT32S,		/**< @brief the value is an array of unsigned int 32 bits */
		ATTRIBUTE_TYPE_FLOATS,		/**< @brief the value is an array of float */
		ATTRIBUTE_TYPE_VECTORS,		/**< @brief the value is an array of Vector3D */
		ATTRIBUTE_TYPE_COLORS,		/**< @brief the value is an array of Color */
		ATTRIBUTE_TYPE_STRINGS,		/**< @brief the value is an array of std::string */
		ATTRIBUTE_TYPE_REFS,		/**< @brief the value is an array of Ref<SPKObject> */
	};

	/**
	* @brief A single field of a serializable object
	*
	* An attribute is defined by :
	* <ul>
	* <li>a name</li>
	* <li>a type</li>
	* <li>a value (which can be set or not</li>
	* </ul>
	*/
	class Attribute
	{
	friend class Descriptor;

	public : 

		Attribute(const std::string& name,AttributeType type);

		/**
		* @brief Gets the name of this attribute
		* @return the name
		*/
		const std::string& getName() const;

		/**
		* @brief Gets the type of the attribute
		* @return the type
		*/
		AttributeType getType() const;

		/**
		* @brief Tells whether the value of this attribute is set
		* @return true if the value is set, false if not
		*/
		bool hasValue() const;

		/**
		* @brief Tells whether the value of this attribute is optional
		* 
		* An optional value will not necessarily be exported when serializing because it is not needed to deserialize the object
		* (Typically it is the default value of the attribute).
		* Optional value are useful when serializing in human readable format (xml for instance) to keep the information clear.
		*
		* @return true if the value is optinal, false otherwise
		*/
		bool isValueOptional() const;

		// For type safety

		/**
		* @brief Sets the value (char)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueChar(char value,bool optional = false)										{ setValue(ATTRIBUTE_TYPE_CHAR,value,optional); }

		/**
		* @brief Sets the value (bool)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueBool(bool value,bool optional = false)										{ setValue(ATTRIBUTE_TYPE_BOOL,value,optional); }

		/**
		* @brief Sets the value (int 32 bits)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueInt32(long int value,bool optional = false)								{ setValue(ATTRIBUTE_TYPE_INT32,value,optional); }

		/**
		* @brief Sets the value (unsigned int 32 bits)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueUint32(unsigned long int value,bool optional = false)						{ setValue(ATTRIBUTE_TYPE_UINT32,value,optional); }

		/**
		* @brief Sets the value (float)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueFloat(float value,bool optional = false)									{ setValue(ATTRIBUTE_TYPE_FLOAT,value,optional); }

		/**
		* @brief Sets the value (Vector3D)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueVector(const Vector3D& value,bool optional = false)						{ setValue(ATTRIBUTE_TYPE_VECTOR,value,optional); }

		/**
		* @brief Sets the value (Color)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueColor(const Color& value,bool optional = false)							{ setValue(ATTRIBUTE_TYPE_COLOR,value,optional); }

		/**
		* @brief Sets the value (std::string)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueString(const std::string& value,bool optional = false)						{ setValue(ATTRIBUTE_TYPE_STRING,value,optional); }

		/**
		* @brief Sets the value (Ref)
		* @param value : the value
		* @param optional : is this value optional ?
		*/
		void setValueRef(const WeakRef<SPKObject>& value,bool optional = false)					{ setValue(ATTRIBUTE_TYPE_REF,value,optional); }
		
		/**
		* @brief Sets the reference value and make it optional if NULL
		* @param value : the value
		*/
		void setValueRefOptionalOnNull(const WeakRef<SPKObject>& value)							{ setValueRef(value,value == NULL); }

		/**
		* @brief Sets the boolean value and make it optional if false
		* @param value : the value
		*/
		void setValueBoolOptionalOnFalse(bool value)											{ setValueBool(value,!value); }

		/**
		* @brief Sets the array of values (char)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesChar(const char* values,size_t nb,bool optional = false)					{ setValues(ATTRIBUTE_TYPE_CHARS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (bool)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesBool(const bool* values,size_t nb,bool optional = false)					{ setValues(ATTRIBUTE_TYPE_BOOLS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (int 32 bits)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesInt32(const long int* values,size_t nb,bool optional = false)				{ setValues(ATTRIBUTE_TYPE_INT32S,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (unsigned int 32 bits)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesUint32(const unsigned long int* values,size_t nb,bool optional = false)	{ setValues(ATTRIBUTE_TYPE_UINT32S,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (float)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesFloat(const float* values,size_t nb,bool optional = false)				{ setValues(ATTRIBUTE_TYPE_FLOATS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (Vector3D)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesVector(const Vector3D* values,size_t nb,bool optional = false)			{ setValues(ATTRIBUTE_TYPE_VECTORS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (Color)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesColor(const Color* values,size_t nb,bool optional = false)				{ setValues(ATTRIBUTE_TYPE_COLORS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (std::string)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesString(const std::string* values,size_t nb,bool optional = false)			{ setValues(ATTRIBUTE_TYPE_STRINGS,values,nb,optional); }
		
		/**
		* @brief Sets the array of values (Ref)
		* @param value : the array of values
		* @param nb : the number of values
		* @param optional : is this value optional ?
		*/
		void setValuesRef(const WeakRef<SPKObject>* values,size_t nb,bool optional = false)		{ setValues(ATTRIBUTE_TYPE_REFS,values,nb,optional); }
		
		/**
		* @brief Gets the value (char)
		* @return value : the array of values
		*/
		char getValueChar() const									{ return getValue<char>(ATTRIBUTE_TYPE_CHAR); }

		/**
		* @brief Gets the value (bool)
		* @return value : the array of values
		*/
		bool getValueBool() const									{ return getValue<bool>(ATTRIBUTE_TYPE_BOOL); }

		/**
		* @brief Gets the value (int 32 bits)
		* @return value : the array of values
		*/
		long int getValueInt32() const								{ return getValue<long int>(ATTRIBUTE_TYPE_INT32); }

		/**
		* @brief Gets the value (unsigned int 32 bits)
		* @return value : the array of values
		*/
		unsigned long int getValueUint32() const					{ return getValue<unsigned long int>(ATTRIBUTE_TYPE_UINT32); }

		/**
		* @brief Gets the value (float)
		* @return value : the array of values
		*/
		float getValueFloat() const									{ return getValue<float>(ATTRIBUTE_TYPE_FLOAT); }

		/**
		* @brief Gets the value (Vector3D)
		* @return value : the array of values
		*/
		Vector3D getValueVector() const								{ return getValue<Vector3D>(ATTRIBUTE_TYPE_VECTOR); }

		/**
		* @brief Gets the value (Color)
		* @return value : the array of values
		*/
		Color getValueColor() const									{ return getValue<Color>(ATTRIBUTE_TYPE_COLOR); }

		/**
		* @brief Gets the value (std::string)
		* @return value : the array of values
		*/
		std::string getValueString() const							{ return getValue<std::string>(ATTRIBUTE_TYPE_STRING); }

		/**
		* @brief Gets the value (Ref)
		* @return value : the array of values
		*/
		WeakRef<SPKObject> getValueRef() const						{ return getValue<WeakRef<SPKObject>>(ATTRIBUTE_TYPE_REF); }

		/**
		* @brief Gets the array of values (char)
		* @return value : the array of values
		*/
		std::vector<char> getValuesChar() const						{ return getValues<char>(ATTRIBUTE_TYPE_CHARS); }

		/**
		* @brief Gets the array of values (bool)
		* @return value : the array of values
		*/
		std::vector<bool> getValuesBool() const						{ return getValues<bool>(ATTRIBUTE_TYPE_BOOLS); }

		/**
		* @brief Gets the array of values (int 32 bits)
		* @return value : the array of values
		*/
		std::vector<long int> getValuesInt32() const				{ return getValues<long int>(ATTRIBUTE_TYPE_INT32S); }

		/**
		* @brief Gets the array of values (unsigned int 32 bits)
		* @return value : the array of values
		*/
		std::vector<unsigned long int> getValuesUint32() const		{ return getValues<unsigned long int>(ATTRIBUTE_TYPE_UINT32S); }

		/**
		* @brief Gets the array of values (float)
		* @return value : the array of values
		*/
		std::vector<float> getValuesFloat() const					{ return getValues<float>(ATTRIBUTE_TYPE_FLOATS); }

		/**
		* @brief Gets the array of values (Vector3d)
		* @return value : the array of values
		*/
		std::vector<Vector3D> getValuesVector() const				{ return getValues<Vector3D>(ATTRIBUTE_TYPE_VECTORS); }

		/**
		* @brief Gets the array of values (Color)
		* @return value : the array of values
		*/
		std::vector<Color> getValuesColor() const					{ return getValues<Color>(ATTRIBUTE_TYPE_COLORS); }

		/**
		* @brief Gets the array of values (std::string)
		* @return value : the array of values
		*/
		std::vector<std::string> getValuesString() const			{ return getValues<std::string>(ATTRIBUTE_TYPE_STRINGS); }

		/**
		* @brief Gets the array of values (Ref)
		* @return value : the array of values
		*/
		std::vector<WeakRef<SPKObject>> getValuesRef() const		{ return getValues<WeakRef<SPKObject>>(ATTRIBUTE_TYPE_REFS); }

	private :

		std::string name;
		AttributeType type;
		size_t offset;

		Descriptor* descriptor;
		
		bool valueSet;
		bool optional;

		// Generic setValue(s)/getValue(s) (encapsulated to ensure type safety)
		template<typename T> void setValue(AttributeType valueType,const T& value,bool optional = false);
		template<typename T> void setValues(AttributeType valueType,const T* values,size_t nb,bool optional = false);
		template<typename T> T getValue(AttributeType valueType) const;
		template<typename T> const std::vector<T> getValues(AttributeType valueType) const;
	};

	inline const std::string& Attribute::getName() const
	{
		return name;
	}

	inline AttributeType Attribute::getType() const
	{
		return type;
	}

	inline bool Attribute::hasValue() const
	{
		return valueSet;
	}

	inline bool Attribute::isValueOptional() const
	{
		return optional;
	}

	inline Attribute::Attribute(const std::string& name,AttributeType type) :
		name(name),
		type(type),
		offset(0),
		descriptor(NULL),
		valueSet(false)
	{}

	template<typename T>
	void Attribute::setValue(AttributeType valueType,const T& value,bool optional)
	{
		SPK_ASSERT(valueType == type,"Attribute::setValue<T>(AttributeType,const T&,bool) - The value is not of the right type");

		offset = descriptor->buffer.size();
		const char* valueC = reinterpret_cast<const char*>(&value);
		for (size_t i = 0; i < sizeof(T); ++i)
			descriptor->buffer.push_back(valueC[i]);
		valueSet = true;
		this->optional = optional;

		SPK_LOG_DEBUG("Set value for attribute \"" << name << "\" : " << value);
	}

	template<typename T>
	void Attribute::setValues(AttributeType valueType,const T* values,size_t nb,bool optional)
	{
		SPK_ASSERT(valueType == type,"Attribute::setValues<T>(AttributeType,const T&,size_t,bool) - The array of values not of the right type");
		SPK_ASSERT(nb > 0,"Attribute::setValues<T>(AttributeType,const T&,size_t,bool) - An array of size 0 cannot be serialized");

		offset = descriptor->buffer.size();
		const char* nbC = reinterpret_cast<char*>(&nb);
		const char* valuesC = reinterpret_cast<const char*>(values);
		for (size_t i = 0; i < sizeof(size_t); ++i)
			descriptor->buffer.push_back(nbC[i]);
		for (size_t i = 0; i < sizeof(T) * nb; ++i)
			descriptor->buffer.push_back(valuesC[i]);
		valueSet = true;
		this->optional = optional;

#if !defined(SPK_NO_LOG) && defined(_DEBUG)
		Logger::Stream os = SPK::Logger::getInstance().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Set " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << values[i];
		os << '\n';
		SPK::Logger::getInstance().flush();
#endif
	}

	template<typename T>
	T Attribute::getValue(AttributeType valueType) const
	{
		SPK_ASSERT(valueType == type,"Attribute::getValue<T>(AttributeType) - The desired value is not of the right type");
		SPK_ASSERT(valueSet,"Attribute::getValue<T>(AttributeType) - The value is not set and therefore cannot be read");

		SPK_LOG_DEBUG("Get value for attribute \"" << name << "\" : " << (*reinterpret_cast<T*>(&descriptor->buffer[offset])));

		return *reinterpret_cast<T*>(&descriptor->buffer[offset]);
	}

	template<typename T>
	const std::vector<T> Attribute::getValues(AttributeType valueType) const
	{
		SPK_ASSERT(valueType == type,"Attribute::getValue<T>(AttributeType) - The desired array of values is not of the right type");
		SPK_ASSERT(valueSet,"Attribute::getValues(size_t& nb) - The value is not set and therefore cannot be read");

		size_t nb = *reinterpret_cast<size_t*>(&descriptor->buffer[offset]);
		std::vector<T> tmpBuffer;
		for (size_t i = 0; i < nb; ++i)
			tmpBuffer.push_back(*reinterpret_cast<T*>(&descriptor->buffer[offset + sizeof(size_t) + i * sizeof(T)]));

#if !defined(SPK_NO_LOG) && defined(_DEBUG)
		Logger::Stream os = SPK::Logger::getInstance().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Get " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << tmpBuffer[i];
		os << '\n';
		SPK::Logger::getInstance().flush();
#endif

		return tmpBuffer;
	}
}}

#endif
