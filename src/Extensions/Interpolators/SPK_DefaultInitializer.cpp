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
#include "Extensions/Interpolators/SPK_DefaultInitializer.h"

namespace SPK
{
	template<>
	void DefaultInitializer<float>::innerImport(const IO::Descriptor& descriptor)
	{
		Interpolator<float>::innerImport(descriptor);

		const IO::Attribute* attrib = NULL;
		if (attrib = descriptor.getAttributeWithValue("default value"))
			setDefaultValue(attrib->getValueFloat());
	}

	template<>
	void DefaultInitializer<float>::innerExport(IO::Descriptor& descriptor) const
	{
		Interpolator<float>::innerExport(descriptor);
		descriptor.getAttribute("default value")->setValueFloat(getDefaultValue());
	}

	template<>
	void DefaultInitializer<Color>::innerImport(const IO::Descriptor& descriptor)
	{
		Interpolator<Color>::innerImport(descriptor);

		const IO::Attribute* attrib = NULL;
		if (attrib = descriptor.getAttributeWithValue("default value"))
			setDefaultValue(attrib->getValueColor());
	}

	template<>
	void DefaultInitializer<Color>::innerExport(IO::Descriptor& descriptor) const
	{
		Interpolator<Color>::innerExport(descriptor);
		descriptor.getAttribute("default value")->setValueColor(getDefaultValue());
	}	
}
