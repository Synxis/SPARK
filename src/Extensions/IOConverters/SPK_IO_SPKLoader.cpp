//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

#include <SPARK_Core.h>
#include "Extensions/IOConverters/SPK_IO_SPKLoader.h"

namespace SPK
{
namespace IO
{	
	bool SPKLoader::innerLoad(std::istream& is,Graph& graph) const
	{
		// Check header
		const Buffer header(HEADER_LENGTH,is);
		for (size_t i = 0; i < 3; ++i)
			if (header.get<char>() != MAGIC_NUMBER[i])
			{
				SPK_LOG_ERROR("Invalid SPK format");
				return false;
			}

		if (header.get<char>() != VERSION)
		{
			SPK_LOG_ERROR("Wrong version of SPK format");
			return false;
		}

		size_t dataLength = header.get<uint32>();
		SPK_LOG_DEBUG("SPKLoader::innerLoad(std::istream&,Graph&) - Data length : " << dataLength << " bytes");

		size_t nbObjects = header.get<uint32>();
		SPK_LOG_DEBUG("SPKLoader::innerLoad(std::istream&,Graph&) - Nb objects : " << nbObjects);
		
		// Loads data
		const Buffer data(dataLength,is);
		if (is.bad())
		{
			SPK_LOG_ERROR("Error while reading the stream");
			return false;
		}

		size_t* objectDataOffset = SPK_NEW_ARRAY(size_t,nbObjects);	
		
		// First pass to get the objects types
		for (size_t i = 0; i < nbObjects; ++i)
		{
			if (data.isAtEnd())
			{
				SPK_LOG_ERROR("Corrupted data");
				SPK_DELETE_ARRAY(objectDataOffset);
				return false;
			}

			std::string type = data.get<std::string>();
			objectDataOffset[i] = data.getPosition() + 4;
			data.skip(data.get<uint32>());
			graph.addNode(i,type);
		}

		if (!graph.validateNodes())
			return false;

		// Second pass to fill the descriptors
		for (size_t i = 0; i < nbObjects; ++i)
		{
			Node* node = graph.getNode(i);
			if (node != NULL)
			{
				Descriptor& desc = node->getDescriptor();

				data.setPosition(objectDataOffset[i]);
				if (desc.getSignature() != data.get<uint32>())
				{
					SPK_LOG_WARNING("Wrong signature for node "+node->getObject()->getClassName());
				}
				else
				{
					for (size_t j = 0; j < desc.getNbAttributes(); ++j)
					{
						if (data.isAtEnd())
						{
							SPK_LOG_ERROR("Corrupted data");
							SPK_DELETE_ARRAY(objectDataOffset);
							return false;
						}

						if (data.get<bool>())
						{
							Attribute& attrib = desc.getAttribute(j);
							switch (attrib.getType())
							{
							case ATTRIBUTE_TYPE_CHAR :		attrib.setValue(data.get<char>()); break;
							case ATTRIBUTE_TYPE_BOOL :		attrib.setValue(data.get<bool>()); break;
							case ATTRIBUTE_TYPE_INT32 :		attrib.setValue(data.get<int32>()); break;
							case ATTRIBUTE_TYPE_UINT32 :	attrib.setValue(data.get<uint32>()); break;
							case ATTRIBUTE_TYPE_FLOAT :		attrib.setValue(data.get<float>()); break;
							case ATTRIBUTE_TYPE_VECTOR :	attrib.setValue(data.get<Vector3D>()); break;
							case ATTRIBUTE_TYPE_COLOR :		attrib.setValue(data.get<Color>()); break;
							case ATTRIBUTE_TYPE_STRING :	attrib.setValue(data.get<std::string>()); break;
							
							case ATTRIBUTE_TYPE_CHARS :		setAttributeValues<char>(attrib,data); break;
							case ATTRIBUTE_TYPE_BOOLS :		setAttributeValues<bool>(attrib,data); break;
							case ATTRIBUTE_TYPE_INT32S :	setAttributeValues<int32>(attrib,data); break;
							case ATTRIBUTE_TYPE_UINT32S :	setAttributeValues<uint32>(attrib,data); break;
							case ATTRIBUTE_TYPE_FLOATS :	setAttributeValues<float>(attrib,data); break;
							case ATTRIBUTE_TYPE_VECTORS :	setAttributeValues<Vector3D>(attrib,data); break;
							case ATTRIBUTE_TYPE_COLORS :	setAttributeValues<Color>(attrib,data); break;
							case ATTRIBUTE_TYPE_STRINGS :	setAttributeValues<std::string>(attrib,data); break;

							case ATTRIBUTE_TYPE_REF : {
								size_t ref = data.get<uint32>() - 1;
								Node* node = graph.getNode(ref);
								if (node != NULL)	
									attrib.setValue(node->getObject());
								break; }

							case ATTRIBUTE_TYPE_REFS : {
								size_t nbValues = data.get<uint32>();
								Ref<SPKObject>* objects = SPK_NEW_ARRAY(Ref<SPKObject>,nbValues);
								for (size_t k = 0; k < nbValues; ++k)
								{
									size_t ref = data.get<uint32>() - 1;
									Node* node = graph.getNode(ref);
									if (node != NULL)	
										objects[k] = node->getObject();
								}
								attrib.setValues(objects,nbValues);
								SPK_DELETE_ARRAY(objects);
								break; }
	
							default : SPK_LOG_FATAL("");
							}
						}
					}
				}
			}
		}

		SPK_DELETE_ARRAY(objectDataOffset);
		return true;
	}
}}
