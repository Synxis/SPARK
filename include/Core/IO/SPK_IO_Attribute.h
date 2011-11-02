//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

		template<typename T> static AttributeType getAttributeType();
		template<typename T> static AttributeType getAttributeTypeArray();

		template<typename T> void setValue(const T& value,bool optional = false);
		template<typename T> void setValues(const T* values,size_t nb,bool optional = false);
		template<typename T> T getValue() const;
		template<typename T> std::vector<T> getValues() const;

		template<typename T> void setValueRef(const Ref<T>& value,bool optional = false);
		template<typename T> void setValuesRef(const Ref<T>* values,size_t nb,bool optional = false);
		template<typename T> Ref<T> getValueRef() const;
		template<typename T> std::vector<Ref<T>> getValuesRef() const;

		void setValueOptionalOnFalse(bool value);
		void setValueOptionalOnTrue(bool value);
		void setValueOptionalOnNull(const Ref<SPKObject>& value);
		void setValueOptionalOnEmpty(const std::string& value);

	private :

		std::string name;
		AttributeType type;
		size_t offset;

		Descriptor* descriptor;
		
		bool valueSet;
		bool optional;
	};

	inline Attribute::Attribute(const std::string& name,AttributeType type) :
		name(name),
		type(type),
		offset(0),
		descriptor(NULL),
		valueSet(false)
	{}

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

	template<typename T>
	void Attribute::setValue(const T& value,bool optional)
	{
		SPK_ASSERT(getAttributeType<T>() == type,"Attribute::setValue<T>(AttributeType,const T&,bool) - The value is not of the right type");

		offset = descriptor->buffer.size();
		const char* valueC = reinterpret_cast<const char*>(&value);
		for (size_t i = 0; i < sizeof(T); ++i)
			descriptor->buffer.push_back(valueC[i]);
		valueSet = true;
		this->optional = optional;

		SPK_LOG_DEBUG("Set value for attribute \"" << name << "\" : " << value);
	}

	template<typename T>
	void Attribute::setValues(const T* values,size_t nb,bool optional)
	{
		SPK_ASSERT(getAttributeTypeArray<T>() == type,"Attribute::setValues<T>(AttributeType,const T&,size_t,bool) - The array of values is not of the right type");
		if (nb == 0) return; // the value is not set if the array is empty

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
		Logger::Stream os = SPK::Logger::get().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Set " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << values[i];
		os << '\n';
		SPK::Logger::get().flush();
#endif
	}

	template<typename T>
	T Attribute::getValue() const
	{
		SPK_ASSERT(getAttributeType<T>() == type,"Attribute::getValue<T>(AttributeType) - The desired value is not of the right type");
		SPK_ASSERT(valueSet,"Attribute::getValue<T>(AttributeType) - The value is not set and therefore cannot be read");

		SPK_LOG_DEBUG("Get value for attribute \"" << name << "\" : " << (*reinterpret_cast<T*>(&descriptor->buffer[offset])));

		return *reinterpret_cast<T*>(&descriptor->buffer[offset]);
	}

	template<typename T>
	std::vector<T> Attribute::getValues() const
	{
		SPK_ASSERT(getAttributeTypeArray<T>() == type,"Attribute::getValues<T>(AttributeType) - The desired array of values is not of the right type");
		SPK_ASSERT(valueSet,"Attribute::getValues<T>(AttributeType) - The value is not set and therefore cannot be read");

		size_t nb = *reinterpret_cast<size_t*>(&descriptor->buffer[offset]);
		std::vector<T> tmpBuffer;
		for (size_t i = 0; i < nb; ++i)
			tmpBuffer.push_back(*reinterpret_cast<T*>(&descriptor->buffer[offset + sizeof(size_t) + i * sizeof(T)]));

#if !defined(SPK_NO_LOG) && defined(_DEBUG)
		Logger::Stream os = SPK::Logger::get().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Get " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << tmpBuffer[i];
		os << '\n';
		SPK::Logger::get().flush();
#endif

		return tmpBuffer;
	}

	template<typename T>
	void Attribute::setValueRef(const Ref<T>& value,bool optional)				
	{ 
		SPK_ASSERT(ATTRIBUTE_TYPE_REF == type,"Attribute::setValueRef(const Ref<SPKObject>&,bool) - The value is not a reference");

		offset = descriptor->buffer.size();
		size_t refBufferSize = descriptor->refBuffer.size();
		const char* refOffset = reinterpret_cast<const char*>(&refBufferSize);
		for (size_t i = 0; i < sizeof(size_t); ++i)
			descriptor->buffer.push_back(refOffset[i]);

		descriptor->refBuffer.push_back(value);
		
		valueSet = true;
		this->optional = optional;

		SPK_LOG_DEBUG("Set value for attribute \"" << name << "\" : " << value);
	}
	
	template<typename T>
	Ref<T> Attribute::getValueRef() const												
	{ 
		SPK_ASSERT(ATTRIBUTE_TYPE_REF == type,"Attribute::getValueRef() - The desired value is not a reference");
		SPK_ASSERT(valueSet,"Attribute::getValueRef() - The value is not set and therefore cannot be read");
		
		SPK_LOG_DEBUG("Get value for attribute \"" << name << "\" : " << descriptor->refBuffer[*reinterpret_cast<size_t*>(&descriptor->buffer[offset])]);

		return descriptor->refBuffer[*reinterpret_cast<size_t*>(&descriptor->buffer[offset])].cast<T>();
	}

	template<typename T>
	void Attribute::setValuesRef(const Ref<T>* values,size_t nb,bool optional)	
	{ 
		SPK_ASSERT(ATTRIBUTE_TYPE_REFS == type,"Attribute::setValuesRef<T>(const Ref<T>*,size_t,bool) - The array of values is not an array of references");
		if (nb == 0) return; // the value is not set if the array is empty		

		offset = descriptor->buffer.size();
		const char* nbC = reinterpret_cast<const char*>(&nb);
		size_t refBufferSize = descriptor->refBuffer.size();
		const char* refOffset = reinterpret_cast<const char*>(&refBufferSize);
		for (size_t i = 0; i < sizeof(size_t); ++i)			// Writes the number of objects
			descriptor->buffer.push_back(nbC[i]);
		for (size_t i = 0; i < sizeof(size_t); ++i)			// Writes the starting offset in the refBuffer
			descriptor->buffer.push_back(refOffset[i]);
		for (size_t i = 0; i < nb; ++i)						// Writes each objects
			descriptor->refBuffer.push_back(values[i]);
		valueSet = true;
		this->optional = optional;

#if !defined(SPK_NO_LOG) && defined(_DEBUG)
		Logger::Stream os = SPK::Logger::get().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Set " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << values[i];
		os << '\n';
		SPK::Logger::get().flush();
#endif
	}

	template<typename T>
	std::vector<Ref<T>> Attribute::getValuesRef() const				
	{ 
		SPK_ASSERT(ATTRIBUTE_TYPE_REFS == type,"Attribute::getValuesRef<T>() - The desired array of values is an array of references");
		SPK_ASSERT(valueSet,"Attribute::getValuesRef<T>() - The value is not set and therefore cannot be read");

		size_t nb = *reinterpret_cast<size_t*>(&descriptor->buffer[offset]);
		size_t refOffset = *reinterpret_cast<size_t*>(&descriptor->buffer[offset + sizeof(size_t)]);
		std::vector<Ref<T>> tmpBuffer;
		for (size_t i = 0; i < nb; ++i)
			tmpBuffer.push_back(descriptor->refBuffer[refOffset + i].cast<T>());

#if !defined(SPK_NO_LOG) && defined(_DEBUG)
		Logger::Stream os = SPK::Logger::get().getStream(SPK::LOG_PRIORITY_DEBUG);
		os << "Get " << nb << " values for attribute \"" << name << "\" : ";
		for (size_t i = 0; i < nb; ++i)
			os << " " << tmpBuffer[i];
		os << '\n';
		SPK::Logger::get().flush();
#endif

		return tmpBuffer;
	}

	// Specialization for string (TODO : Factorize that)
	template<>
	inline void Attribute::setValue(const std::string& value,bool optional)
	{
		SPK_ASSERT(ATTRIBUTE_TYPE_STRING == type,"Attribute::setValue<T>(AttributeType,const T&,bool) - The value is not of the right type");

		offset = descriptor->buffer.size();
		const char* valueC = value.c_str();
		for (size_t i = 0; i < value.size() + 1; ++i)
			descriptor->buffer.push_back(valueC[i]);
		valueSet = true;
		this->optional = optional;

		SPK_LOG_DEBUG("Set value for attribute \"" << name << "\" : " << value);
	}

	template<>
	inline void Attribute::setValues(const std::string* values,size_t nb,bool optional)
	{
		SPK_LOG_INFO("Serialization of array of strings is not implemented yet");
	}

	template<>
	inline std::string Attribute::getValue() const
	{
		SPK_ASSERT(ATTRIBUTE_TYPE_STRING == type,"Attribute::getValue<T>(AttributeType) - The desired value is not of the right type");
		SPK_ASSERT(valueSet,"Attribute::getValue<T>(AttributeType) - The value is not set and therefore cannot be read");

		SPK_LOG_DEBUG("Get value for attribute \"" << name << "\" : " << &descriptor->buffer[offset]);

		return std::string(&descriptor->buffer[offset]);
	}

	template<>
	inline std::vector<std::string> Attribute::getValues() const
	{
		SPK_LOG_INFO("Deserialization of array of strings is not implemented yet");
		return std::vector<std::string>();
	}

	// Specialization for refs
	template<> inline void Attribute::setValue(const Ref<SPKObject>& value,bool optional)					{ setValueRef(value,optional); }
	template<> inline void Attribute::setValues(const Ref<SPKObject>* values,size_t nb,bool optional)		{ setValuesRef(values,nb,optional); }
	template<> inline Ref<SPKObject> Attribute::getValue() const											{ return getValueRef<SPKObject>(); }
	template<> inline std::vector<Ref<SPKObject>> Attribute::getValues() const								{ return getValuesRef<SPKObject>(); }

	template<> inline AttributeType Attribute::getAttributeType<char>()						{ return ATTRIBUTE_TYPE_CHAR; }
	template<> inline AttributeType Attribute::getAttributeType<bool>()						{ return ATTRIBUTE_TYPE_BOOL; }
	template<> inline AttributeType Attribute::getAttributeType<int32>()					{ return ATTRIBUTE_TYPE_INT32; }
	template<> inline AttributeType Attribute::getAttributeType<uint32>()					{ return ATTRIBUTE_TYPE_UINT32; }
	template<> inline AttributeType Attribute::getAttributeType<float>()					{ return ATTRIBUTE_TYPE_FLOAT; }
	template<> inline AttributeType Attribute::getAttributeType<Vector3D>()					{ return ATTRIBUTE_TYPE_VECTOR; }
	template<> inline AttributeType Attribute::getAttributeType<Color>()					{ return ATTRIBUTE_TYPE_COLOR; }
	template<> inline AttributeType Attribute::getAttributeType<std::string>()				{ return ATTRIBUTE_TYPE_STRING; }
	template<> inline AttributeType Attribute::getAttributeType<Ref<SPKObject>>()			{ return ATTRIBUTE_TYPE_REF; }

	template<> inline AttributeType Attribute::getAttributeTypeArray<char>()				{ return ATTRIBUTE_TYPE_CHARS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<bool>()				{ return ATTRIBUTE_TYPE_BOOLS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<int32>()				{ return ATTRIBUTE_TYPE_INT32S; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<uint32>()				{ return ATTRIBUTE_TYPE_UINT32S; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<float>()				{ return ATTRIBUTE_TYPE_FLOATS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<Vector3D>()			{ return ATTRIBUTE_TYPE_VECTORS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<Color>()				{ return ATTRIBUTE_TYPE_COLORS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<std::string>()			{ return ATTRIBUTE_TYPE_STRINGS; }
	template<> inline AttributeType Attribute::getAttributeTypeArray<Ref<SPKObject>>()		{ return ATTRIBUTE_TYPE_REFS; }

	// Helper methods (after specilization as they instantiate some template methods)
	inline void Attribute::setValueOptionalOnFalse(bool value)							{ setValue<bool>(value,!value); }
	inline void Attribute::setValueOptionalOnTrue(bool value)							{ setValue<bool>(value,value); }
	inline void Attribute::setValueOptionalOnNull(const Ref<SPKObject>& value)			{ setValueRef(value,!value); }
	inline void Attribute::setValueOptionalOnEmpty(const std::string& value)			{ setValue<std::string>(value,value.empty()); }
}}

#endif
