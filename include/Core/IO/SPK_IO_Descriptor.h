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

#ifndef H_SPK_DESCRIPTOR
#define H_SPK_DESCRIPTOR

#include <vector>
#include <string>

namespace SPK
{
	class SPKObject;

namespace IO
{
	class SPK_PREFIX Descriptor
	{
	friend class Attribute;
	friend class SPKObject;

	public :

		Descriptor(const Descriptor& descriptor);

		Attribute* getAttribute(const std::string& name);
		Attribute* getAttributeWithValue(const std::string& name);
		Attribute& getAttribute(size_t index);

		inline const Attribute* getAttribute(const std::string& name) const;
		inline const Attribute* getAttributeWithValue(const std::string& name) const;
		inline const Attribute& getAttribute(size_t index) const;

		inline size_t getNbAttributes() const;
		inline size_t getSignature() const;

		inline const std::string& getName() const;

	private :

		Descriptor(const std::vector<Attribute>& attributes); // Constructor only accessible by SPKObject

		std::vector<Attribute> attributes;		
		std::vector<char> buffer; // An internal buffer is used to limit memory allocation of attribute values

		unsigned long int signature;

		std::string name;

		void computeSignature();
		void markAttributes();

		inline void setName(const std::string& name);
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

	inline const Attribute* Descriptor::getAttribute(const std::string& name) const
	{
		return const_cast<Descriptor*>(this)->getAttribute(name);
	}

	inline const Attribute* Descriptor::getAttributeWithValue(const std::string& name) const
	{
		return const_cast<Descriptor*>(this)->getAttribute(name);
	}

	inline const Attribute& Descriptor::getAttribute(size_t index) const
	{
		return const_cast<Descriptor*>(this)->getAttribute(index);
	}

	inline size_t Descriptor::getNbAttributes() const
	{
		return attributes.size();
	}

	inline size_t Descriptor::getSignature() const
	{
		return signature;
	}

	inline const std::string& Descriptor::getName() const
	{
		return name;
	}

	inline void Descriptor::setName(const std::string& name)
	{
		this->name = name;
	}
}}

#endif
