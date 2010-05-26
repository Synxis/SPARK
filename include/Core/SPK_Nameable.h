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

#ifndef H_SPK_NAMEABLE
#define H_SPK_NAMEABLE

#include "Core/SPK_DEF.h"
#include <string>

namespace SPK
{
	/**
	* @class Nameable
	* @brief An abstract class allowing an object to be named
	*
	* This class offers an interface to name and object and also to traverse the object
	* to find a nameable with a given name.
	*/
	class Nameable
	{
	public :

		/**
		* @brief Sets the name of this nameable
		* @param name : the name
		*/
		inline void setName(const std::string& name);

		/**
		* @brief Gets the name of this nameable
		* @return the name
		*/
		inline const std::string& getName() const;
		
		/**
		* @brief Traverses this nameable to find a nameable with the given name
		*
		* The first nameable found with the given name is returned.<br>
		* This nameable is always checked first.<br>
		* If no nameable with the given name is found, null is returned.<br>
		*
		* @param name : the name of the nameable to find
		* @return : the nameable with the given name or null
		*/
		virtual inline Nameable* findByName(const std::string& name);

	protected :

		// abstract class
		inline Nameable() {}
		inline Nameable(const Nameable& nameable) : name(nameable.name) {}

	private :

		std::string name;		
	};

	void Nameable::setName(const std::string& name)
	{
		this->name = name;
	}

	const std::string& Nameable::getName() const
	{
		return name;
	}

	Nameable* Nameable::findByName(const std::string& name)
	{
		return getName().compare(name) == 0 ? this : NULL;
	}
}

#endif
