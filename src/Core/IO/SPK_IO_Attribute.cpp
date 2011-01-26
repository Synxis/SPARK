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

#include <SPARK_Core.h>

namespace SPK
{
namespace IO
{
	Attribute::Attribute(const std::string& name,AttributeType type) :
		name(name),
		type(type),
		offset(0),
		descriptor(NULL),
		valueSet(false)
	{}

	void Attribute::setValueRef(const Ref<SPKObject>& value,bool optional)				
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
	
	Ref<SPKObject> Attribute::getValueRef() const												
	{ 
		SPK_ASSERT(ATTRIBUTE_TYPE_REF == type,"Attribute::getValueRef() - The desired value is not a reference");
		SPK_ASSERT(valueSet,"Attribute::getValueRef() - The value is not set and therefore cannot be read");
		
		SPK_LOG_DEBUG("Get value for attribute \"" << name << "\" : " << descriptor->refBuffer[*reinterpret_cast<size_t*>(&descriptor->buffer[offset])]);

		return descriptor->refBuffer[*reinterpret_cast<size_t*>(&descriptor->buffer[offset])];
	}
}}
