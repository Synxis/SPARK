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

#ifndef H_SPK_CONTROLS
#define H_SPK_CONTROLS

namespace SPK
{
	/**
	* @brief Interface for controls
	*
	* A control is a value that can be propagated to
	* attributes that are connected to. The control
	* and attribute sytem is very similar to the
	* signal and slot system in Qt.
	*/
	template<typename T>
	class ValueControl : public ControlConnection<T>
	{
	public:
		/**
		* @brief Constructor (value is default-initialized)
		*/
		ValueControl() : value(T()) {}

		/**
		* @brief Destructor
		* @note Control is automatically disconnected on destruction
		*/
		~ValueControl()
		{
			disconnect();
		}

		/**
		* @brief Removes all connections
		*/
		void disconnect()
		{
			while(nextConnection)
				nextConnection->remove();
		}

		/**
		* @internal
		* @brief Propagates the value to all connected attributes
		*/
		void propagate()
		{
			Connection<T>* c = (Connection<T>*)nextConnection;
			while(c)
			{
				c->set(value);
				c = (Connection<T>*)c->nextConnection;
			}
		}

		/**
		* @internal
		* @brief Create a new connection based on the specified parameters
		*/
		ConnectionStatus connect(ConnectionParameters& cp)
		{
			cp.controlPtr = this;
			cp.type = ToSPKType<T>::type;
			return cp.receiver->getConnectableDescription()->acceptConnection(cp);
		}

		/**
		* @brief Sets the value of this control.
		*/
		void set(typename Arg<T>::type v)
		{
			value = v;
		}
		
	private:
		T value;
	};

	/**
	* @internal
	* @brief Type used in descriptions
	*
	* This class is a 'static' implementation of a ValueControl for a specific
	* object. The control object is stored inside the description.
	*/
	template<typename O, typename T, typename N, void* (*ctrlObj)(O*)>
	class ObjectValueControl : public ValueControl<T>
	{
	public:
		using ValueControl<T>::set;
		using ValueControl<T>::connect;
		using ValueControl<T>::propagate;
		using ValueControl<T>::disconnect;

		typedef O objectType;
		typedef ObjectValueControl<O,T,N,ctrlObj> thisType;

	public:
		static const ValueType valueType;

		/** @brief Returns the control object */
		static thisType* self(O* obj)
		{
			return (thisType*)ctrlObj(obj);
		}

		/** @brief Name of the control */
		static const char* asName()
		{
			return N::get();
		}

		/** @brief Sets the control value */
		static bool set(O* object, typename Arg<T>::type v)
		{
			self(object)->set(v);
			return true;
		}

		/** @brief Default behavior for types that do not match the type of the control */
		template<typename V>
		static bool set(O* object, V v)
		{
			return false;
		}

		/** @brief Static version of propagate() */
		static void propagate(O* object)
		{
			self(object)->propagate();
		}

		/** @brief Static version of connect() */
		static ConnectionStatus connect(ConnectionParameters& cp, O* obj)
		{
			return self(obj)->connect(cp);
		}

		/** @brief Static version of disconnect() */
		static void disconnect(O* obj)
		{
			self(obj)->disconnect();
		}
	};

	template<typename O, typename T, typename N, void* (*ctrlObj)(O*)>
	const ValueType ObjectValueControl<O,T,N,ctrlObj>::valueType = ToSPKType<T>::type;
}

#endif
