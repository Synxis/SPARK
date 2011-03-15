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
	std::map<SPKObject*,SPKObject*> SPKObject::copyBuffer;

	SPKObject::SPKObject(bool SHAREABLE) : 
		name(),
		transform(),
		nbReferences(0),
		SHAREABLE(SHAREABLE),
		shared(false)	
	{}
	
	SPKObject::SPKObject(const SPKObject& obj) : 
		name(obj.name),
		transform(obj.transform),
		nbReferences(0),
		SHAREABLE(obj.SHAREABLE),
		shared(obj.shared)
	{}

	SPKObject::~SPKObject()
	{
		SPK_ASSERT(nbReferences == 0,"SPKObject::~SPKObject() - The number of references of the object is not 0 during destruction");
	}

	void SPKObject::setShared(bool shared)
	{
		if (shared && !SHAREABLE)
		{
			SPK_LOG_WARNING("SPKObject::setShared(bool) - This object is of a type that is not shareable. Nothing happens");
			return;
		}

		this->shared = shared;
	}

	void SPKObject::updateTransform(const Ref<SPKObject>& parent)
	{
		SPK_ASSERT(parent != this,"SPKObject::updateTransform(const SPKObject*) - A SPKObject cannot be its own parent");
		transform.update(parent,*this);
	}

	void SPKObject::importAttributes(const IO::Descriptor& descriptor)
	{
		const std::string& descName = descriptor.getName(); 
		if (descName != getClassName())
		{
			SPK_LOG_ERROR("SPKObject::importAttributes(const Descriptor&) - The descriptor does not match the object : \"" << descName << "\" for \"" << getClassName() << "\"");
		}
		else
			innerImport(descriptor);
	}

	IO::Descriptor SPKObject::exportAttributes() const
	{
		IO::Descriptor& descriptor = createDescriptor();
		innerExport(descriptor);
		return descriptor;
	}

	IO::Descriptor SPKObject::createDescriptor() const
	{
		std::vector<IO::Attribute> attributes;
		fillAttributeList(attributes);
		IO::Descriptor descriptor(attributes);
		descriptor.setName(getClassName());
		SPK_LOG_DEBUG("Create descriptor for " << descriptor.getName() << " with " << descriptor.getNbAttributes() << " attributes and signature " << descriptor.getSignature());
		return descriptor;
	}

	void SPKObject::innerImport(const IO::Descriptor& descriptor)
	{
		const IO::Attribute* attrib = NULL;

		if (attrib = descriptor.getAttributeWithValue("name"))
			setName(attrib->getValue<std::string>());

		if (attrib = descriptor.getAttributeWithValue("transform"))
		{
			std::vector<float> t = attrib->getValues<float>();
			if (t.size() == 16)
				transform.set(&t[0]);
			else
				SPK_LOG_ERROR("SPKObject::innerImport(const IO::Descriptor&) - Wrong number of entries for attribute transform");
		}

		if (attrib = descriptor.getAttributeWithValue("shared"))
			setShared(attrib->getValue<bool>());
	}
	
	void SPKObject::innerExport(IO::Descriptor& descriptor) const
	{
		descriptor.getAttribute("name")->setValue(name,name.empty());
		descriptor.getAttribute("transform")->setValues(transform.getLocal(),Transform::TRANSFORM_LENGTH,transform.isLocalIdentity());
		descriptor.getAttribute("shared")->setValueOptionalOnFalse(isShared());
	}
}
