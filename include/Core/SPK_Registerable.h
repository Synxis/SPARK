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

#ifndef H_SPK_REGISTERABLE
#define H_SPK_REGISTERABLE

#include <map>

#include "Core/SPK_DEF.h"
#include "Core/SPK_Nameable.h"

// A macro implementing the clone method for Registerable children
#define SPK_IMPLEMENT_REGISTERABLE(ClassName) \
\
private : \
virtual ClassName* clone() const \
{ \
	return SPK_NEW(ClassName,*this); \
} \
\
public : \
virtual std::string getClassName() const {return #ClassName;}

namespace SPK
{
	class SPK_PREFIX Registerable : public Nameable
	{
	friend class System;
	friend class Group;
	friend class SPKContext;

	public :

		virtual ~Registerable();

		inline void increment();
		void decrement();

		inline unsigned int getNbReferences() const;
		inline bool isShared() const;
		inline bool isDestroyable() const;
		
		inline void setShared(bool shared);
		inline void setDestroyable(bool destroyable);

		bool destroy(bool decrement = true);

		static Registerable* copyRegisterable(const Registerable* registerable,bool increment = true);

		virtual std::string getClassName() const = 0;

	protected :

		Registerable();
		Registerable(const Registerable& registerable);

		static void incrementChild(Registerable* registerable);
		static void decrementChild(Registerable* registerable);
		static bool destroyChild(Registerable* registerable);
		static Registerable* copyChild(Registerable* registerable,bool increment = true);

	private :

		static std::map<const Registerable*,Registerable*> copyBuffer;

		unsigned int nbReferences;
		bool shared;
		bool destroyable;

		virtual Registerable* clone() const = 0;
	};

	inline void Registerable::increment()
	{
		++nbReferences;
	}

	inline unsigned int Registerable::getNbReferences() const
	{
		return nbReferences;
	}

	inline bool Registerable::isShared() const
	{
		return shared;
	}

	inline bool Registerable::isDestroyable() const
	{
		return destroyable;
	}

	inline void Registerable::setShared(bool shared)
	{
		this->shared = shared;
	}

	inline void Registerable::setDestroyable(bool destroyable)
	{
		this->destroyable = destroyable;
	}
}

#endif
