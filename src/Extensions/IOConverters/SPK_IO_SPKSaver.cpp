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

#include <SPARK_Core.h>
#include "Extensions/IOConverters/SPK_IO_SPKSaver.h"

namespace SPK
{
namespace IO
{
	const unsigned char SPKFormatVariables::MAGIC_NUMBER[3] = { 'S', 'P', 'K' };
	const unsigned char SPKFormatVariables::VERSION = 1;

	struct SPKSaver::SaveContext
	{
		struct Object
		{
			SPKObject* object;
			unsigned int id;
		};

		std::vector<Object> objects;
		IO::Buffer buffer;
		std::ostream& os;
		unsigned int nbConnections;
		unsigned int nbConnectionPosition;

		SaveContext(std::ostream& o) :
			os(o),
			buffer(2048),
			nbConnections(0),
			nbConnectionPosition(0)
		{
		}

		unsigned int refId(SPKObject* object)
		{
			if(!object)
				return 0;
			for(unsigned int t = 0; t < objects.size(); t++)
				if(objects[t].object == object)
					return objects[t].id;
			return 0;
		}
	};

	class SPKSerializer : public SerializerConcept<SPKSerializer>
	{
	public:
		SPKSerializer(SPKSaver* s) : saver(s), current(0), nbAttrPos(0), nbAttrs(0) {}

		template<typename T>
		void storeValue(const T& value, Buffer& buffer)
		{
			buffer << value;
		}

		template<typename T>
		void storeValue(const Ref<T>& value, Buffer& buffer)
		{
			buffer << saver->context->refId(value.get());
		}

		template<typename T>
		void storeValue(const Pair<Ref<T> >& value, Buffer& buffer)
		{
			buffer << saver->context->refId(value.value1.get());
			buffer << saver->context->refId(value.value2.get());
		}

		template<typename T>
		void storeValue(const Triplet<Ref<T> >& value, Buffer& buffer)
		{
			buffer << saver->context->refId(value.value1.get());
			buffer << saver->context->refId(value.value2.get());
			buffer << saver->context->refId(value.value3.get());
		}

		template<typename T>
		void storeValue(const Quadruplet<Ref<T> >& value, Buffer& buffer)
		{
			buffer << saver->context->refId(value.value1.get());
			buffer << saver->context->refId(value.value2.get());
			buffer << saver->context->refId(value.value3.get());
			buffer << saver->context->refId(value.value4.get());
		}

		template<typename T>
		void storeValue(const std::vector<Ref<T> >& value, Buffer& buffer)
		{
			buffer << value.size();
			for(unsigned int t = 0; t < value.size(); t++)
				buffer << saver->context->refId(value[t].get());
		}

		template<typename T>
		void storeTypedValue(const T& value)
		{
			Buffer& buffer = saver->context->buffer;
			
			// Type
			buffer << *(unsigned int*)&(ToSPKType<T>::type);
			
			// Value
			storeValue(value, buffer);
		}

		void storeAttributeHeader(const char* attrName, const SPK::IO::Context& sc)
		{
			Buffer& buffer = saver->context->buffer;

			buffer << std::string(attrName);
			buffer << sc.isStructuredAttribute();
		}

		void saveObjectHeader(SPKObject* obj)
		{
			Buffer& buffer = saver->context->buffer;

			if(current)
			{
				unsigned int pos = buffer.getPosition();
				buffer.setPosition(nbAttrPos);
				buffer << nbAttrs;
				buffer << (pos - buffer.getPosition() - 4);
				buffer.setPosition(pos);
			}

			current = obj;

			if(current)
			{
				// Object header
				buffer << saver->context->refId(obj);
				nbAttrPos = buffer.getPosition();
				buffer << (unsigned int)0 << (unsigned int)0;
				nbAttrs = 0;
			}
		}

		void emptyAttribute(const char* name, const Context& sc)
		{
			// Attribute header
			storeAttributeHeader(name, sc);

			// Structured attribute header
			saver->context->buffer << (unsigned int)0 << (unsigned int)0;

			nbAttrs++;
		}

		template<typename T>
		void serialize(const char* attrName, const T& value, const SPK::IO::Context& sc)
		{
			if(current != sc.object)
				saveObjectHeader(sc.object);

			if(!sc.isStructuredAttribute())
			{
				// Attribute is standard or array
				storeAttributeHeader(attrName, sc);
				storeTypedValue<T>(value);
				nbAttrs++;
			}
			else
			{
				// Attribute is structured, first save the header (if first field)
				Buffer& buffer = saver->context->buffer;
				if(sc.structured.id == 0 && sc.structured.fieldIndex == 0)
				{
					storeAttributeHeader(attrName, sc);
					buffer << sc.structured.size << sc.structured.fieldCount;
					nbAttrs++;
				}

				// Field header
				buffer << std::string(sc.structured.fieldName);
				buffer << sc.structured.id;

				// Value
				storeTypedValue<T>(value);
			}
		}

	private:
		SPKSaver* saver;
		SPKObject* current;
		unsigned int nbAttrPos;
		unsigned int nbAttrs;
	};

	SPKSaver::SPKSaver() :
		serializer(0),
		context(0)
	{
		serializer = SPK_NEW(SPKSerializer, this);
	}

	SPKSaver::~SPKSaver()
	{
		SPK_DELETE(serializer);
	}
		
	void SPKSaver::beginSave(std::ostream& os, const std::vector<SPKObject*>& objRef)
	{
		// Context
		context = SPK_NEW(SaveContext, os);
		for(unsigned int t = 0; t < objRef.size(); t++)
		{
			SaveContext::Object obj;
			obj.id = t + 1;
			obj.object = objRef[t];
			context->objects.push_back(obj);
		}

		// Header
		context->buffer << SPKFormatVariables::MAGIC_NUMBER
			<< SPKFormatVariables::VERSION
			<< objRef.size();
		context->nbConnectionPosition = context->buffer.getPosition();
		context->buffer << (unsigned int)0; // Number of connections will be known in the 2nd phase
		context->buffer << (unsigned int)0; // Data-length is not already known

		// Type list
		for(unsigned int t = 0; t < objRef.size(); t++)
			context->buffer << objRef[t]->getClassName();
	}

	void SPKSaver::serializeConnection(const Ref<SPKObject>& sender, const std::string& ctrl,
		const Ref<SPKObject>& receiver, const std::string& attr, unsigned int fieldId, const std::string& field)
	{
		// End the last object
		serializer->saveObjectHeader(0);

		ClassDescription cd = sender->getDescription();
		context->nbConnections++;

		context->buffer << context->refId(sender.get());
		context->buffer << ctrl;
		context->buffer << context->refId(receiver.get());
		context->buffer << attr;
		context->buffer << fieldId;
		context->buffer << field;
	}

	bool SPKSaver::endSave()
	{
		if(!context)
			return false;

		// End the last object
		serializer->saveObjectHeader(0);

		context->buffer.setPosition(context->nbConnectionPosition);
		context->buffer << context->nbConnections;
		context->buffer << unsigned int(context->buffer.getSize() - context->buffer.getPosition() - 4);
		context->os.write(context->buffer.getData(), context->buffer.getSize());

		SPK_DELETE(context);
		context = 0;
		return true;
	}

	Serializer* SPKSaver::getSerializer()
	{
		return serializer;
	}
}
}
