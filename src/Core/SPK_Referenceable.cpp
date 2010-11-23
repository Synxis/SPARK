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

#include <SPARK_Core.h>

namespace SPK
{
	std::map<WeakRef<Referenceable>,WeakRef<Referenceable>> Referenceable::copyBuffer;

	Referenceable::Referenceable() :
		SPKObject(),
		nbReferences(0),
		shared(false)
	{}

	Referenceable::Referenceable(const Referenceable& referenceable) :
		SPKObject(referenceable),
		nbReferences(0),
		shared(referenceable.shared)
	{}

	Referenceable::~Referenceable()
	{
		if (nbReferences != 0)
		{
			SPK_LOG_ERROR("Referenceable::~Referenceable() - The number of references of the object is not 0 during destruction");
		}
	}

	void Referenceable::innerImport(const IO::Descriptor& descriptor)
	{
		SPKObject::innerImport(descriptor);

		const IO::Attribute* attrib = NULL;
		if (attrib = descriptor.getAttributeWithValue("shared"))
			setShared(attrib->getValueBool());
	}
	
	void Referenceable::innerExport(IO::Descriptor& descriptor) const
	{
		SPKObject::innerExport(descriptor);
		descriptor.getAttribute("shared")->setValueBool(isShared());			
	}
}
