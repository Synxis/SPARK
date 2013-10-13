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

#ifndef H_SPK_IO_SAVER
#define H_SPK_IO_SAVER

namespace SPK
{
namespace IO
{
	/**
	* @brief Interface allowing to create new formatting formats.
	* By subclassing this interface and registering it in the IO manager,
	* you can save a whole system to an arbitrary stream (file, console, network, ...).
	* The save process is divided in two consecutive phases:
	*	1. Saving the objects of the system (including the system itself). This is an
	*	   attribute-centric phase, in which a serializer is used to serialize all attributes
	*	   of all objects in the system, consecutively. There is no method called to indicate
	*	   that the object changed; instead, a IO::Context is provided, and it contains
	*	   (among other things) the object containing the attribute.
	*	2. Saving the connections. For each connection, the method @c serializeConnection
	*	   is called. The @c id and @c field of a connection are meaningless if the
	*	   attribute is structured (in this case, the name of the field will be empty).
	*/
	class Saver
	{
	public:
		/** @brief Destructor */
		virtual ~Saver() {}
		
		/**
		* @brief Starts a save operation.
		* The @c objRef parameter allows to attach an object to a ref id.
		* Its use is not compulsory. This parameter is guaranteed not to
		* be null.
		*/
		virtual void beginSave(std::ostream& os, const std::vector<SPKObject*>& objRef) = 0;

		/**
		* @brief Serialize a connection.
		* This method is called once for each connection in the system; if 'field' is empty, then
		* the connection is not made to a field and both 'field' and 'fieldId' can be ignored.
		* @note This function is called only after the serialization of all objects in the system,
		* thus attributes and objects are serialized first, and connections are serialized at last.
		*/
		virtual void serializeConnection(const Ref<SPKObject>& sender, const std::string& ctrl,
			const Ref<SPKObject>& receiver, const std::string& attr, unsigned int fieldId, const std::string& field) = 0;

		/**
		* @brief Ends the save operation.
		* @return True if the save succeeded, else false
		*/
		virtual bool endSave() = 0;

	protected:
		friend class Manager;

		/**
		* @brief Returns the serializer used to save the objects
		*/
		virtual Serializer* getSerializer() = 0;
	};
}}

#endif
