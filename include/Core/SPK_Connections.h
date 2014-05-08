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

#ifndef H_SPK_CONNECTIONS
#define H_SPK_CONNECTIONS

namespace SPK
{
	/**
	* @internal
	* @Brief The connection system
	*
	* The phases of a connection:
	* @code
	*	PHASE 0, static:
	*		connect(sender, "control", receiver, "attr", id, "field");
	*
	*	PHASE 1, sender:
	*		connect("control", receiver, "attr", id, "field");
	*
	*	PHASE 2, control:
	*		connect(receiver, "attr", id, "field");
	*
	*	PHASE 3, receiver:
	*		connect("attr", id, "field");
	*
	*	PHASE 4, attribute:
	*		standard/array:
	*			finalize_connect
	*		structured:
	*			connect(id, "field");
	*
	*	PHASE 5, field:
	*		finalize_connect
	* @endcode
	*
	* Memory-model: controls and attributes are part several intrusive lists.
	* Attributes can be connected to only one control; controls can be connected
	* to an unlimited number attributes.
	*
	* @code
	*
	*	 control,obj
	*		  |
	*	attr_A, obj_0			<- standard connection
	*		  |
	*	attr_B, obj_0			<- standard connection
	*		  |
	*	attr_A, obj_1			<- standard connection
	*		  |
	*	field_C, obj_1, id_4	<- field connection
	*
	* @endcode
	*
	* Deletion:
	*	- of object: object disonnect all (controls + attributes)
	*	- of attribute: disconnect
	*	- of control: disconnect all
	*
	* Disconnect = connection->remove()
	*	- will reset the attribute connection for standard / array attributes
	*	- will delete the connection object for field
	*/
	class ConnectionItem;
	template<typename T, typename O, typename F> class FieldConnectionHead;

	/**
	* @brief Create a connection between a control and an attribute.
	* @param sender Object containing the control
	* @param control Control to connect
	* @param receiver Object containing the attribute
	* @param attribute Attribute to connect
	* @param id Id of the value, if the attribute is structured (ignored if the attribute is not structured)
	* @param field Field to connect (ignored if the attribute is not structured)
	* @return A status indicating the success or not of the operation.
	*/
	SPK_PREFIX ConnectionStatus connect(const Ref<SPKObject>& sender, const std::string& control,
		const Ref<SPKObject>& receiver, const std::string& attribute,
		unsigned int id = 0, const std::string& field = "");

	/**
	* @brief Remove the specified connection
	*
	* If the connection receiver is a field, then the behavior is the following:
	*  - if @c id and @c field are specified, then only the connection from the control
	*    to the specified field at the specified id is removed (if found)
	*  - if @c field is left empty, all connections from control to all fields in the
	*    attribute, at the specified id, are removed.
	*/
	SPK_PREFIX void disconnect(const Ref<SPKObject>& sender, const std::string& control,
		const Ref<SPKObject>& receiver, const std::string& attribute,
		unsigned int id = 0, const std::string& field = "");

	/**
	*
	*/
	struct ConnectionParameters
	{
		/// PHASE 1
		/** @brief Control to connect */
		const std::string& control;

		/// PHASE 2
		/** @brief Object containing the attribute */
		Ref<SPKObject> receiver;

		/// PHASE 3
		/** @brief Name of the attribute to connect */
		const std::string& attribute;

		/// PHASE 4
		/** @brief Type of the control */
		ValueType type;

		/// PHASE 4
		/** @brief Id of the field to connect */
		unsigned int id;

		/// PHASE 4
		/** @brief Name of the field to connect */
		const std::string& field;

		/// PHASE 4, 5
		/** @brief Pointer to the control to connect */
		ConnectionItem* controlPtr;
	};

	/**
	*
	*/
	struct ConnectableDescription
	{
		virtual ConnectionStatus acceptConnection(ConnectionParameters& parameters) = 0;
		virtual ConnectionStatus connect(ConnectionParameters& parameters, const Ref<SPKObject>& sender) = 0;
		virtual void disconnect(const ConnectionParameters& cp) = 0;
	};

	/**
	* @internal
	* @brief Used to store the connection information (who is connected to who) inside the objects
	*/
	class ConnectionItem
	{
	public:
		ConnectionItem() : previousConnection(0), nextConnection(0) {}
		virtual void remove() = 0;
		virtual bool insertAfter(ConnectionItem* item) = 0;
		virtual Ref<SPKObject> getReceiver() const { return NullReferenceValue(); }
		virtual const char* getAttribute() const { return ""; }
		virtual unsigned int getFieldId() const { return 0; }
		virtual const char* getField() const { return ""; }

		ConnectionItem* getNext() const { return nextConnection; }

	protected:
		virtual ~ConnectionItem() {}
		SPK_PREFIX void removeConnection();
		SPK_PREFIX bool insertAfterConnection(ConnectionItem* item);

	protected:
		template<typename> friend class ValueControl;
		ConnectionItem* previousConnection;
		ConnectionItem* nextConnection;
	};

	/**
	* @internal
	* @brief Typed version of ConnectionItem
	*/
	template<typename T>
	class Connection : public ConnectionItem
	{
	public:
		virtual void set(typename Arg<T>::type value) = 0;
	};

	/**
	* @internal
	* @brief Control version of the connection item (head of the list)
	*/
	template<typename T>
	class ControlConnection : public Connection<T>
	{
	public:
		void remove() {}
		bool insertAfter(ConnectionItem* item) { return false; }
	};

	/**
	* @internal
	* @brief Connection item for attributes (not structured)
	*/
	template<typename T, typename O, typename A>
	class AttributeConnection : public Connection<T>
	{
	public:
		AttributeConnection() : controlledObject(0) {}
		~AttributeConnection()
		{
			remove();
		}
		Ref<SPKObject> getReceiver() const
		{
			return controlledObject;
		}
		const char* getAttribute() const
		{
			return A::asName();
		}
		void remove()
		{
			this->removeConnection();
			controlledObject = 0;
		}
		bool insertAfter(ConnectionItem* item)
		{
			return this->insertAfterConnection(item);
		}
		void set(typename Arg<T>::type value)
		{
			if(controlledObject)
				A::set(controlledObject, value);
		}
		ConnectionStatus connect(const ConnectionParameters& cp)
		{
			// Check control type
			if(cp.type != A::valueType)
				return CONNECTION_STATUS_TYPE_ERROR;

			// Check that we are not already connected
			if(controlledObject)
				return CONNECTION_STATUS_ALREADY_CONNECTED;

			// Create the connection
			insertAfter(cp.controlPtr);
			controlledObject = (O*)cp.receiver.get();
			return cp.field.size() > 0 ? CONNECTION_STATUS_OK_FIELD_NOT_NEEDED : CONNECTION_STATUS_OK;
		}

	private:
		O* controlledObject;
	};

	/**
	* @internal
	* @brief Base class for field connections (structured attributes cannot be
	* connected to, only their fields can).
	*/
	template<typename T>
	class FieldConnection : public Connection<T>
	{
	public:
		FieldConnection() : previousField(0), nextField(0) {}
		virtual ~FieldConnection() {}
		virtual bool insertAfterField(FieldConnection* field) = 0;
		virtual void moveAfter(FieldConnection* field, unsigned int id) = 0;

	protected:
		void removeField()
		{
			if(previousField)
				previousField->nextField = nextField;
			if(nextField)
				nextField->previousField = previousField;
			previousField = nextField = 0;
		}

	protected:
		template<typename T_, typename O, typename F> friend class FieldConnectionHead;
		template<typename T_, typename O, typename F> friend class FieldConnectionItem;
		FieldConnection<T>* previousField;
		FieldConnection<T>* nextField;
	};

	/**
	* @internal
	* @brief Item used to store the connection state for 1 field (several connections to the same
	* field - with different ids - are stored as a list of this item).
	*/
	template<typename T, typename O, typename F>
	class FieldConnectionItem : public FieldConnection<T>
	{
	public:
		FieldConnectionItem(O* obj = 0, unsigned int i = 0) :
			controlledObject(obj),
			id(i)
		{
		}
		~FieldConnectionItem()
		{
			this->removeConnection();
			this->removeField();
		}
		Ref<SPKObject> getReceiver() const
		{
			return controlledObject;
		}
		const char* getAttribute() const
		{
			return F::structureName();
		}
		unsigned int getFieldId() const
		{
			return id;
		}
		const char* getField() const
		{
			return F::asName();
		}
		void remove()
		{
			SPK_DELETE(this);
		}
		bool insertAfter(ConnectionItem* item)
		{
			return this->insertAfterConnection(item);
		}
		bool insertAfterField(FieldConnection<T>* field)
		{
			if(!field) return false;

			this->previousField = field;
			this->nextField = field->nextField;
			field->nextField = this;
			if(this->nextField) this->nextField->previousField = this;
			return true;
		}
		void set(typename Arg<T>::type value)
		{
			if(controlledObject)
				F::set(controlledObject, id, value);
		}

		void moveAfter(FieldConnection<T>* field, unsigned int newId)
		{
			id = newId;

			if(!field || field == this) return;

			this->removeField();
			insertAfterField(field);
		}

	private:
		friend class FieldConnectionHead<T, O, F>;
		O* controlledObject;
		unsigned int id;
	};

	/**
	* @internal
	* @brief Head of the list of connections of 1 field.
	*/
	template<typename T, typename O, typename F>
	class FieldConnectionHead : public FieldConnection<T>
	{
	public:
		typedef FieldConnectionItem<T, O, F> ItemType;

	public:
		FieldConnectionHead() {}
		~FieldConnectionHead()
		{
			disconnectAll();
		}
		void remove()
		{
		}
		bool insertAfter(ConnectionItem*)
		{
			return false;
		}
		bool insertAfterField(FieldConnection<T>*)
		{
			return false;
		}
		void set(typename Arg<T>::type)
		{
		}
		void moveAfter(FieldConnection<T>*, unsigned int)
		{
		}

		void disconnectAll()
		{
			while(this->nextField)
				this->nextField->remove();
		}

		void removeElement(unsigned int id)
		{
			ItemType* toRemove = getElementOfId(id);
			ItemType* last = getLastElement();

			if(last)
				last->moveAfter(toRemove, id);
			if(toRemove)
				toRemove->remove();
		}

		ItemType* getElementBeforeId(unsigned int id)
		{
			ItemType* field = (ItemType*)(this->nextField);
			ItemType* previous = 0;
			while(field && field->id < id)
			{
				previous = field;
				field = (ItemType*)field->nextField;
			}
			return previous;
		}

		ItemType* getElementOfId(unsigned int id)
		{
			ItemType* before = getElementBeforeId(id);
			ItemType* field = (before ? (ItemType*)before->nextField : 0);
			return field && field->id == id ? field : 0;
		}

		ItemType* getLastElement()
		{
			FieldConnection<T>* field = this;
			while(field->nextField)
				field = field->nextField;
			return (ItemType*)(field == this ? 0 : field);
		}

		ConnectionStatus connect(const ConnectionParameters& cp)
		{
			// Check control type
			if(cp.type != F::valueType)
				return CONNECTION_STATUS_TYPE_ERROR;

			// Check that we are not already connected
			ItemType* beforeId = getElementBeforeId(cp.id);
			if(beforeId && beforeId->nextField && ((ItemType*)(beforeId->nextField))->id == cp.id)
				return CONNECTION_STATUS_ALREADY_CONNECTED;

			// Create the connection
			ItemType* newField = SPK_NEW(ItemType, (O*)cp.receiver.get(), cp.id);
			newField->insertAfter(cp.controlPtr);
			newField->insertAfterField(beforeId ? (FieldConnection<T>*)beforeId : (FieldConnection<T>*)this);
			return CONNECTION_STATUS_OK;
		}

		void disconnect(const ConnectionParameters& cp)
		{
			if(ItemType* toRemove = getElementOfId(cp.id))
				toRemove->remove();
		}
	};
}

#endif
