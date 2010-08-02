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

#include "Core/SPK_Registerable.h"
#include "Core/SPK_Logger.h"

namespace SPK
{
	std::map<const Registerable*,Registerable*> Registerable::copyBuffer;

	Registerable::Registerable() :
		Nameable(),
		nbReferences(0),
		shared(false),
		destroyable(true)
	{}

	Registerable::Registerable(const Registerable& registerable) :
		Nameable(registerable),
		nbReferences(0),
		shared(registerable.shared),
		destroyable(true)
	{}

	Registerable::~Registerable()
	{
		if (nbReferences != 0)
		{
			SPK_LOG_ERROR("Registerable::~Registerable() - The number of references of the object is not 0 during destruction");
		}
	}

	void Registerable::decrement()
	{
		if (nbReferences == 0)
		{
			SPK_LOG_ERROR("Registerable::decrement() - Wrong counting of references of the objects. A call to increment() is missing");
		}
		else		
			--nbReferences;
	}

	bool Registerable::destroy(bool decrement)
	{
		if (decrement)
			this->decrement();
		if (nbReferences == 0)
		{
			delete this;
			return true;
		}
		return false;
	}

	void Registerable::incrementChild(Registerable* registerable)
	{
		if (registerable != NULL)
			registerable->increment();
	}

	void Registerable::decrementChild(Registerable* registerable)
	{
		if (registerable != NULL)
			registerable->decrement();
	}

	bool Registerable::destroyChild(Registerable* registerable)
	{
		if (registerable == NULL)
			return false;

		registerable->decrement();
		if (registerable->destroyable && registerable->nbReferences == 0)
		{
			delete registerable;
			return true;
		}

		return false;
	}
	
	Registerable* Registerable::copyChild(Registerable* registerable,bool increment)
	{
		if (registerable == NULL)
			return NULL;

		if (registerable->isShared())
		{
			if (increment)
				registerable->increment();
			return registerable;
		}

		std::map<const Registerable*,Registerable*>::const_iterator it = copyBuffer.find(registerable);
		if (it != copyBuffer.end())
		{
			if (increment)
				it->second->increment();
			return it->second;
		}

		Registerable* clone = registerable->clone();
		copyBuffer.insert(std::pair<const Registerable*,Registerable*>(registerable,clone));
		if (increment)
			clone->increment();
		return clone;
	}

	Registerable* Registerable::copyRegisterable(Registerable* registerable,bool increment)
	{
		if (registerable == NULL)
			return NULL;

		copyBuffer.clear();
		Registerable* clone = registerable->clone();
		if (increment)
			clone->increment();
		return clone;
	}
}
