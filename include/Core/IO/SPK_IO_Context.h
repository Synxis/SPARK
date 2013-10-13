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

#ifndef H_SPK_IO_CONTEXT
#define H_SPK_IO_CONTEXT

namespace SPK
{
namespace IO
{
	/**
	* @brief Context information for structured attributes
	*/
	struct StructuredAttributeContext
	{
		/** @brief Size of the currently de/serialized structured attribute */
		unsigned int size;

		/** @brief Index of the currently de/serialized value in its structured attribute */
		unsigned int id;

		/** @brief Number of fields in the structured attribute */
		unsigned int fieldCount;

		/** @brief Index of the currently de/serialized field */
		unsigned int fieldIndex;

		/** @brief Name of the currently de/serialized field */
		const char* fieldName;

		/** @brief Constructor */
		StructuredAttributeContext(unsigned int s = 0, unsigned int i = 0, unsigned int nbFields = 0) :
			size(s),
			id(i),
			fieldCount(nbFields),
			fieldIndex(0),
			fieldName(0)
		{
		}
	};

	/**
	* @brief Context information about attributes
	*/
	struct Context
	{
		/**
		* @brief Context for a structured attribute
		* @note Data in this variable is invalid if the attribute is not structured
		*/
		StructuredAttributeContext structured;

		/**
		* @brief Object that is currently de/serialized
		*/
		SPKObject* object;

		/** @brief Constructor */
		Context(SPKObject* obj) :
			object(obj)
		{
		}

		/** @brief Constructor */
		Context(SPKObject* obj, const StructuredAttributeContext& sc) :
			structured(sc),
			object(obj)
		{
		}

		/**
		* @brief Returns whether the currently de/serialized attribute is a standard one
		* @note Attributes that are handled as array (spk_array) and standard attributes
		* (spk_attribute) are undistinguishable, as an array attribute is always passed
		* the same as a std::vector standard attribute.
		*/
		bool isStandardAttribute() const
		{
			return structured.fieldName == 0;
		}

		/** @brief Returns whether the currently de/serialized attribute is a structured one */
		bool isStructuredAttribute() const
		{
			return structured.fieldName != 0;
		}
	};

	inline Context standardContext(SPKObject* obj)
	{
		return Context(obj);
	}

	inline Context fieldContext(SPKObject* obj, const StructuredAttributeContext& field)
	{
		return Context(obj, field);
	}
}
}

#endif
