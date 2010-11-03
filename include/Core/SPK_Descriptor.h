//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_DESCRIPTOR
#define H_SPK_DESCRIPTOR

#include <vector>
#include <string>

namespace SPK
{
	class Descriptor;

	enum AttributeType
	{
		ATTRIBUTE_TYPE_CHAR,
		ATTRIBUTE_TYPE_BOOL,
		ATTRIBUTE_TYPE_INT32,
		ATTRIBUTE_TYPE_UINT32,
		ATTRIBUTE_TYPE_FLOAT,
		ATTRIBUTE_TYPE_VECTOR,
		ATTRIBUTE_TYPE_COLOR,
		ATTRIBUTE_TYPE_STRING,
		ATTRIBUTE_TYPE_REF,
		ATTRIBUTE_TYPE_CHARS,
		ATTRIBUTE_TYPE_BOOLS,
		ATTRIBUTE_TYPE_INT32S,
		ATTRIBUTE_TYPE_UINT32S,
		ATTRIBUTE_TYPE_FLOATS,
		ATTRIBUTE_TYPE_VECTORS,
		ATTRIBUTE_TYPE_COLORS,
		ATTRIBUTE_TYPE_REFS,
		ATTRIBUTE_TYPE_STRINGS,
	};

	class Attribute
	{
	friend class Descriptor;

	public : 

		Attribute(std::string& name,AttributeType type);

		inline std::string& getName() const;
		inline AttributeType getType() const;
		inline bool isValueSet() const;

		template<typename T>
		void setValue(T value);

		template<typename T>
		void setValues(T* values,size_t nb);

		template<typename T>
		T getValue() const;

		template<typename T>
		T* getValues(size_t& nb) const;
		
	private :

		std::string& name;
		AttributeType type;
		size_t offset;

		Descriptor* descriptor;
		bool valueSet;
	};

	class Descriptor
	{
	friend class Attribute;

	public :

		Descriptor(std::vector<Attribute> attributes);

		inline Attribute* getAttribute(const std::string& name);
		inline Attribute& getAttribute(size_t index);

		const Attribute* getAttribute(const std::string& name) const;
		const Attribute& getAttribute(size_t index) const;

		inline size_t getAttributeNb() const;
		inline size_t getSignature() const;

	private :

		std::vector<Attribute> attributes;		
		std::vector<char> buffer; // An internal buffer is used to limit memory allocation of attribute values

		unsigned long int signature;

		void computeSignature();
		void markAttributes();
	};

	inline std::string& Attribute::getName() const
	{
		return name;
	}

	inline AttributeType Attribute::getType() const
	{
		return type;
	}

	inline bool Attribute::isValueSet() const
	{
		return valueSet;
	}

	inline const Attribute* Descriptor::getAttribute(const std::string& name) const
	{
		return const_cast<Descriptor*>(this)->getAttribute(name);
	}

	inline const Attribute& Descriptor::getAttribute(size_t index) const
	{
		return const_cast<Descriptor*>(this)->getAttribute(index);
	}

	inline size_t Descriptor::getAttributeNb() const
	{
		return attributes.size();
	}

	inline size_t Descriptor::getSignature() const
	{
		return signature;
	}

	template<typename T>
	void Attribute::setValue(T value)
	{
		offset = descriptor->buffer.size();
		char* valueC = reinterpret_cast<char*>(value)
		for (size_t i = 0; i < sizeof(T))
			descriptor->buffer.push_back(valueC[i]);
		valueSet = true;
	}

	template<typename T>
	void Attribute::setValues(T* values,size_t nb)
	{
		offset = descriptor->buffer.size();
		char* nbC = reinterpret_cast<char*>(nb)
		char* valuesC = reinterpret_cast<char*>(values)
		for (size_t i = 0; i < sizeof(size_t))
			descriptor->buffer.push_back(nbC[i]);
		for (size_t i = 0; i < sizeof(T) * nb)
			descriptor->buffer.push_back(valuesC[i]);
		valueSet = true;	
	}

	template<typename T>
	T Attribute::getValue() const
	{
		SPK_ASSERT(valueSet,"Attribute::getValue() - The value is not set and therefore cannot be read");
		return reinterpret_cast<T>(descriptor->buffer[offset]);
	}

	template<typename T>
	T* Attribute::getValues(size_t& nb) const
	{
		SPK_ASSERT(valueSet,"Attribute::getValues(size_t& nb) - The value is not set and therefore cannot be read");
		nb = reinterpret_cast<size_t>(descriptor->buffer[offset]);
		T* tmpBuffer = new T[nb];
		for (size_t i = 0; i < nb; ++i)
			tmpBuffer[i] = reinterpret_cast<T>(descriptor->buffer[offset + i * sizeof(T)]);
		return tmpBuffer;
	}
}

#endif
