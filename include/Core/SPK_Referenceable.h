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

#ifndef H_SPK_REFERENCEABLE
#define H_SPK_REFERENCEABLE

#include <map>

// A macro implementing the clone method for Referenceable children
#define SPK_IMPLEMENT_REFERENCEABLE(ClassName) \
\
private : \
virtual ClassName* clone() const \
{ \
	return SPK_NEW(ClassName,*this); \
}

namespace SPK
{
	class System;
	class Group;

	class SPK_PREFIX Referenceable : public SPKObject
	{
	friend class System;
	friend class Group;
	friend class SPKContext;
	template<typename T> friend class Ref;

	SPK_START_DESCRIPTION
	SPK_PARENT_ATTRIBUTES(SPKObject)
	SPK_ATTRIBUTE("shared",ATTRIBUTE_TYPE_BOOL)
	SPK_END_DESCRIPTION

	public :

		inline unsigned int getNbReferences() const;

		inline bool isShared() const;
		inline void setShared(bool shared);

		template<typename T>
		static Ref<T> copyRegisterable(const Ref<T>& ref);

	protected :

		Referenceable();
		Referenceable(const Referenceable& referenceable);

		virtual ~Referenceable();

		template<typename T>
		static Ref<T> copyChild(const Ref<T>& ref);

		virtual void innerImport(const IO::Descriptor& descriptor);
		virtual void innerExport(IO::Descriptor& descriptor) const;

	private :

		static std::map<WeakRef<Referenceable>,WeakRef<Referenceable>> copyBuffer;

		unsigned int nbReferences;
		bool shared;

		virtual Referenceable* clone() const = 0;
	};

	inline unsigned int Referenceable::getNbReferences() const
	{
		return nbReferences;
	}

	inline bool Referenceable::isShared() const
	{
		return shared;
	}

	inline void Referenceable::setShared(bool shared)
	{
		this->shared = shared;
	}

	template<typename T>
	Ref<T> Referenceable::copyRegisterable(const Ref<T>& ref)
	{
		if (ref == NULL)
			return ref;

		copyBuffer.clear();
		return Ref<T>(dynamic_cast<T*>(ref->clone()));
	}

	template<typename T>
	Ref<T> Referenceable::copyChild(const Ref<T>& ref)
	{
		if (ref == NULL)
			return Ref<T>();

		if (ref->isShared())
			return ref;

		std::map<WeakRef<Referenceable>,WeakRef<Referenceable>>::const_iterator it = copyBuffer.find(ref);
		if (it != copyBuffer.end())
			return it->second.cast<T>();

		Referenceable* clone = ref->clone();
		copyBuffer.insert(std::make_pair(WeakRef<Referenceable>(ref),WeakRef<Referenceable>(clone)));
		return Ref<T>(dynamic_cast<T*>(clone));
	}
}

#endif
