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

#ifndef H_SPK_OBJECT
#define H_SPK_OBJECT

#include <string>
#include <map>

namespace SPK
{
	/** @brief Defines the share policy for a SPKObject */
	enum SharePolicy
	{
		SHARE_POLICY_CUSTOM,	/**< The SPKObject is shared at the user's discretion */
		SHARE_POLICY_TRUE,		/**< The SPKObject is forced to be shared */		
		SHARE_POLICY_FALSE		/**< The SPKObject is forced to be unshared */
	};

	/** @brief The base class of all SPARK objects */
	class SPK_PREFIX SPKObject
	{
	public :

		/**
		* @brief Copies a SPK Object
		* @param : ref : The SPK object to copy
		* @return a SPK object which is a copy of ref
		*/
		template<typename T>
		static Ref<T> copy(const Ref<T>& ref);

		virtual ~SPKObject();

		///////////////
		// Reference //
		///////////////

		/**
		* @brief Gets the number of references of this object
		* When the number of references reaches 0, the object is destroyed.
		* @return the number of references
		*/
		unsigned int getNbReferences() const;

		/**
		* Tells whether this object is shared or not
		* During a deep copy, a referenced shared object will not be copied but only its reference will.
		* @return true if the object is shared, false otherwise
		*/
		bool isShared() const;

		/**
		* Sets whether this object is shareable or not
		* Note that some type of objects are not shareable
		* @param shared : true to make this obkect shared, false not to
		*/
		void setShared(bool shared);

		//////////
		// Name //
		//////////

		/**
		* @brief Sets the name of this object
		* @param name : the name
		*/
		void setName(const std::string& name);

		/**
		* @brief Gets the name of this object
		* @return the name
		*/
		const std::string& getName() const;

		/**
		* @brief Traverses this object to find an object with the given name
		*
		* The first object found with the given name is returned.<br>
		* This object is always checked first.<br>
		* If no object with the given name is found, NULL is returned.<br>
		*
		* @param name : the name of the object to find
		* @return : the object with the given name or null
		*/
		virtual Ref<SPKObject> findByName(const std::string& name);

	public:
		spark_description(SPKObject, void)
		(
			spk_attribute(std::string, name, setName, getName);
			spk_attribute(bool, shared, setShared, isShared);
		);

	private:
		friend class System;
		template<typename T> friend class Ref;
		template<typename T> friend class ValueControl;
		friend SPK_PREFIX ConnectionStatus connect(const Ref<SPKObject>&, const std::string&, const Ref<SPKObject>&,
			const std::string&, unsigned int, const std::string&);
		friend SPK_PREFIX void disconnect(const Ref<SPKObject>&, const std::string&, const Ref<SPKObject>&,
			const std::string&, unsigned int, const std::string&);
		
	protected :

		// abstract class
		SPKObject(SharePolicy SHARE_POLICY = SHARE_POLICY_CUSTOM);
		SPKObject(const SPKObject& obj);

		template<typename T>
		Ref<T> copyChild(const Ref<T>& ref) const;

	private :

		std::string name;

		unsigned int nbReferences;

		const SharePolicy SHARE_POLICY;
		bool shared;

		// The copy buffer is used to be able to correctly perform a deep copy of objects. If an object is present more than once is the hierarchy it will be copied only once
		// The choice to have a field that is used only for the copy was made because the memory overhead is neglictible and the other possible choices were not satisfying :
		// * Having a static copyBuffer made the copy not thread safe (As concurrent copy of object would make concurrent read/write on the buffer)
		// * Passing the copyBuffer to methods was too dirty and messed up the interface (Impossible to use the copy constructor anymore)
		// Note that with this method the copy of the same object remains not thread safe but the copy of different object is
		mutable std::map<SPKObject*,SPKObject*>* copyBuffer;
	};

	inline unsigned int SPKObject::getNbReferences() const
	{
		return nbReferences;
	}

	inline bool SPKObject::isShared() const
	{
		return shared;
	}

	inline void SPKObject::setName(const std::string& name)
	{
		this->name = name;
	}

	inline const std::string& SPKObject::getName() const
	{
		return name;
	}

	inline Ref<SPKObject> SPKObject::findByName(const std::string& name)
	{
		return getName().compare(name) == 0 ? this : NULL;
	}

	template<typename T>
	Ref<T> SPKObject::copy(const Ref<T>& ref)
	{
		if (!ref)
			return ref;

		if (ref->copyBuffer != NULL)
		{
			SPK_LOG_FATAL("The object is already being copied and cannot be copied more than once at the same time");
			return SPK_NULL_REF;
		}

		ref->copyBuffer = new std::map<SPKObject*,SPKObject*>(); // Creates the copy buffer to allow correct copy of underlying SPARK objects
		Ref<T> clone = dynamicCast<T>(dynamicCast<SPKObject>(ref)->clone());
		delete ref->copyBuffer; // Deletes the copy buffer used for the copy
		ref->copyBuffer = NULL;
		return clone;
	}

	template<typename T>
	Ref<T> SPKObject::copyChild(const Ref<T>& ref) const
	{
		if (!ref)
			return SPK_NULL_REF;

		if (ref->isShared())
			return ref;

		if (copyBuffer == NULL)
		{
			SPK_LOG_FATAL("The copy buffer of the object is NULL while copying the object");
			return SPK_NULL_REF;
		}

		std::map<SPKObject*,SPKObject*>::const_iterator it = copyBuffer->find(ref.get());
		if (it != copyBuffer->end())
			return dynamic_cast<T*>(it->second);

		ref->copyBuffer = copyBuffer; // Sets the copyBuffer of the child to the copyBuffer of the parent
		Ref<SPKObject> clone = dynamicCast<SPKObject>(ref)->clone();
		ref->copyBuffer = NULL; // Removes the reference to the copy buffer (the copy buffer is deleted by the top level copied object)

		copyBuffer->insert(std::make_pair(ref.get(),clone.get()));
		return dynamicCast<T>(clone);
	}
}

#endif
