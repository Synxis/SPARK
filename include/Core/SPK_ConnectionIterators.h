//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 :                                                    //
//  - Julien Fryer - julienfryer@gmail.com				                        //
//  - Thibault Lescoat - info-tibo@orange.fr                                    //
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

#ifndef H_SPK_CONNECTIONITERATORS
#define H_SPK_CONNECTIONITERATORS

namespace SPK
{
	/**
	* @brief Helper function to post increment and operator
	*/
	template<typename T>
	T post_increment(T* t)
	{
		T it = *t;
		++(*t);
		return it;
	}

	/**
	* @brief Iterator class
	*
	* With such an iterator you can traverse the list of connections of a control or an object ('out' connections).
	* Example:
	* @code
	* // Iterate over all connections of the control "myControl" of 'object'.
	* for(ControlConnectionIterator it(object, "myControl"); !it.end(); ++it)
	* {
	*     do_something(it->sender(), it->control(), it->receiver(), it->attribute());
	* }
	* @endcode
	*/
	class ConnectionIterator
	{
	public:
		/** @brief Returns the sender object of the connection pointed by this iterator */
		virtual Ref<SPKObject> sender() const = 0;

		/** @brief Returns the control of the connection pointed by this iterator */
		virtual const char* control() const = 0;

		/** @brief Returns the receiver object of the connection pointed by this iterator */
		virtual Ref<SPKObject> receiver() const = 0;

		/** @brief Returns the attribute of the connection pointed by this iterator */
		virtual const char* attribute() const = 0;

		/**
		* @brief Returns the id of the connection pointed by this iterator.
		* It will always be 0 if the attribute is not a structured one.
		*/
		virtual unsigned int id() const = 0;

		/**
		* @brief Returns the field of the connection pointed by this iterator
		* An empty string is returned if the attribute is not a structured one
		* (and more precisely, if the attribute is structured, this string cannot
		* return an empty string).
		*/
		virtual const char* field() const = 0;

		/**
		* @brief Convenience operator returning the iterator.
		* This allows to follow some usual syntax when dealing with iterators
		*/
		const ConnectionIterator& operator*() const
		{
			return *this;
		}

		/**
		* @brief Convenience operator returning the iterator.
		* This allows to follow some usual syntax when dealing with iterators
		*/
		const ConnectionIterator* operator->() const
		{
			return this;
		}

	protected:
		ConnectionItem* getControlConnection(const Ref<SPKObject>& object, unsigned int id) const
		{
			return object ? object->getDescription().getControlConnection(id) : 0;
		}
	};

	/**
	* @brief An iterator used to iterate over the connections of a specific control
	*/
	class ControlConnectionsIterator : public ConnectionIterator
	{
	public :
		/**
		* @brief Constructor of iterator.
		* The iterator points at the beginning of the collection.
		* @param ctrlId Used to specify which control to iterate
		*/
		ControlConnectionsIterator(const Ref<SPKObject>& obj, unsigned int ctrlId) :
			senderObj(obj),
			controlId(ctrlId),
			current(0)
		{
			if(!obj)
				return;

			current = getControlConnection(obj, controlId);
			if(current) current = current->getNext();
		}

		/**
		* @brief Constructor of iterator.
		* The iterator points at the beginning of the collection.
		* @param ctrlName Used to specify which control to iterate
		*/
		ControlConnectionsIterator(const Ref<SPKObject>& obj, const std::string& ctrlName) :
			senderObj(obj),
			controlId(0),
			current(0)
		{
			if(!obj)
				return;

			// Find id of the control
			ClassDescription cd = obj->getDescription();
			bool found = false;
			for(unsigned int t = 0; t < cd.getControlNb(); t++)
				if(ctrlName == cd.getControlName(t))
				{
					controlId = t;
					found = true;
					break;
				}
			
			if(found)
			{
				current = getControlConnection(obj, controlId);
				if(current) current = current->getNext();
			}
		}

		/**
		* @brief Pre-increments the position of the iterator
		* @return the incremented iterator
		*/
		ControlConnectionsIterator& operator++()
		{
			if(current)
				current = current->getNext();
			return *this;
		}

		/**
		* @brief Post-increments the position of the iterator
		* @return the incremented iterator
		*/
		ControlConnectionsIterator operator++(int)
		{
			return post_increment(this);
		}

		/**
		* @brief Checks whether the iterator has reached the end of the collection
		* @return true if the iterator has reached the end of the collection, false if not
		*/
		bool end() const
		{
			return current == 0;
		}

		/** @brief Reimplementation */
		Ref<SPKObject> sender() const
		{
			return senderObj;
		}

		/** @brief Reimplementation */
		const char* control() const
		{
			return senderObj->getDescription().getControlName(controlId);
		}

		/** @brief Reimplementation */
		Ref<SPKObject> receiver() const
		{
			return current ? current->getReceiver() : NullReferenceValue();
		}

		/** @brief Reimplementation */
		const char* attribute() const
		{
			return current ? current->getAttribute() : "";
		}

		/** @brief Reimplementation */
		unsigned int id() const
		{
			return current ? current->getFieldId() : 0;
		}

		/** @brief Reimplementation */
		const char* field() const
		{
			return current ? current->getField() : "";
		}

	protected:
		Ref<SPKObject> senderObj;
		unsigned int controlId;
		ConnectionItem* current;
	};

	/**
	* @brief An iterator used to iterate over the out connections of an object (ie, all connections from all controls of that object)
	*/
	class OutConnectionsIterator : public ControlConnectionsIterator
	{
	public :
		/**
		* @brief Constructor of iterator
		* The iterator points at the beginning of the collection
		*/
		OutConnectionsIterator(const Ref<SPKObject>& obj) :
			ControlConnectionsIterator(obj, 0)
		{
		}

		/**
		* @brief Pre-increments the position of the iterator
		* @return the incremented iterator
		*/
		OutConnectionsIterator& operator++()
		{
			if(current)
			{
				current = current->getNext();
				if(!current)
				{
					controlId++;
					current = getControlConnection(senderObj, controlId);
					if(current) current = current->getNext();
				}
			}
			return *this;
		}

		/**
		* @brief Post-increments the position of the iterator
		* @return the incremented iterator
		*/
		OutConnectionsIterator operator++(int)
		{
			return post_increment(this);
		}
	};
}

#endif
