//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_IO_LOADER
#define H_SPK_IO_LOADER

//#include <list>

namespace SPK
{
	class System;

namespace IO
{
	/**
	* @brief Class used to load a system.
	*
	* To create a new loader, derive it and implement the load() method. Its work
	* is to create all objects, load their attributes, and connect them if specified
	* in the file to load.
	* To create objects by a type name, use Factory::getInstance().createObject(name).
	*/
	class Loader
	{
	public:
		virtual Ref<System> load(std::istream& is) = 0;
	};
}
}

#endif
