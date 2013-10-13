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
#include "Extensions/IOConverters/SPK_IO_SPKLoader.h"

namespace SPK
{
namespace IO
{
	// Load context
	// -----------------------------
	struct SPKLoader::LoadContext
	{
		struct Object
		{
			struct FieldKey
			{
				std::string name;
				unsigned int id;

				FieldKey(const std::string& s, unsigned int i) : name(s), id(i) {}
				bool operator<(const FieldKey& key) const
				{
					if(name == key.name)
						return id < key.id;
					else
						return name < key.name;
				}
			};

			struct StructuredAttribute
			{
				std::map<FieldKey, unsigned int> fields;
				unsigned int size;

				StructuredAttribute() : size(0) {}
			};

			std::map<std::string, unsigned int> standardAttributes;
			std::map<std::string, StructuredAttribute> structuredAttributes;
			Ref<SPKObject> obj;
		};

		std::vector<Object> objects;
		Buffer buffer;
		unsigned int systemRef;


		LoadContext(std::istream& is, unsigned int dl);

		template<typename T>
		Ref<T> getObject(unsigned int ref) const;
	};

	SPKLoader::LoadContext::LoadContext(std::istream& is, unsigned int dl) :
		buffer(dl, is),
		systemRef(0)
	{
	}

	template<typename T>
	inline Ref<T> SPKLoader::LoadContext::getObject(unsigned int ref) const
	{
		if(ref > 0 && ref <= objects.size())
		{
			if(objects[ref - 1].obj && objects[ref - 1].obj->getDescription().doesInherit(T::description::getClassName()))
				return Ref<T>((T*)objects[ref - 1].obj.get());
		}
		return SPK_NULL_REF;
	}

	template<>
	inline Ref<SPKObject> SPKLoader::LoadContext::getObject(unsigned int ref) const
	{
		if(ref > 0 && ref <= objects.size())
			return objects[ref - 1].obj;
		return SPK_NULL_REF;
	}

	// Helpers
	// -----------------------------
	template<typename T>
	struct TypeMismatchBoundSetter : public BoundSetter<T>
	{
		void operator()(typename Arg<T>::type) const
		{
			SPK_LOG_WARNING("SPKLoader::load(std::istream&) - Warning: saved value and attribute value do not have the same type");
		}
	};

	struct SPKHelper
	{
		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<T>* setValue)
		{
			T value = context.buffer.get<T>();
			if(setValue)
				(*setValue)(value);
		}

		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<Ref<T> >* setValue)
		{
			unsigned int ref = context.buffer.get32();
			Ref<T> value = context.getObject<T>(ref);
			if(setValue)
				(*setValue)(value);
		}

		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<Pair<Ref<T> > >* setValue)
		{
			unsigned int ref1 = context.buffer.get32();
			unsigned int ref2 = context.buffer.get32();
			Pair<Ref<T> > value(context.getObject<T>(ref1), context.getObject<T>(ref2));
			if(setValue)
				(*setValue)(value);
		}

		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<Triplet<Ref<T> > >* setValue)
		{
			unsigned int ref1 = context.buffer.get32();
			unsigned int ref2 = context.buffer.get32();
			unsigned int ref3 = context.buffer.get32();
			Triplet<Ref<T> > value(context.getObject<T>(ref1), context.getObject<T>(ref2), context.getObject<T>(ref3));
			if(setValue)
				(*setValue)(value);
		}

		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<Quadruplet<Ref<T> > >* setValue)
		{
			unsigned int ref1 = context.buffer.get32();
			unsigned int ref2 = context.buffer.get32();
			unsigned int ref3 = context.buffer.get32();
			unsigned int ref4 = context.buffer.get32();
			Quadruplet<Ref<T> > value(context.getObject<T>(ref1), context.getObject<T>(ref2), context.getObject<T>(ref3), context.getObject<T>(ref4));
			if(setValue)
				(*setValue)(value);
		}

		template<typename T>
		static inline void loadValue(SPKLoader::LoadContext& context, const BoundSetter<std::vector<Ref<T> > >* setValue)
		{
			unsigned int size = context.buffer.get32();
			std::vector<Ref<T> > value;
			for(unsigned int t = 0; t < size; t++)
			{
				Ref<T> obj = context.getObject<T>(context.buffer.get32());
				if(obj) value.push_back(obj);
			}
			if(setValue)
				(*setValue)(value);
		}
	};

	// Value deserialization helper
	// -----------------------------
	template<typename T>
	struct SPKLoader::ValueLoader
	{
		const BoundSetter<T>* setValue;
		LoadContext& context;

		ValueLoader(const BoundSetter<T>* s, LoadContext& c) : setValue(s), context(c) {}
		template<typename U>
		void call()
		{
			if(meta::IsSame<T, U>::value)
				SPKHelper::loadValue(context, setValue);
			else if(setValue)
			{
				TypeMismatchBoundSetter<U> warn;
				SPKHelper::loadValue(context, &warn);
			}
			else
			{
				BoundSetter<U>* nullSetter = 0;
				SPKHelper::loadValue(context, nullSetter);
			}
		}
	};

	// Deserializer
	// -----------------------------
	class SPKDeserializer : public DeserializerConcept<SPKDeserializer>
	{
	public:
		SPKDeserializer(unsigned int r, SPKLoader::LoadContext& c) : ref(r), context(c) {}

		unsigned int sizeOfAttribute(const char* attrName)
		{
			std::map<std::string, SPKLoader::LoadContext::Object::StructuredAttribute>::const_iterator it = context.objects[ref - 1].structuredAttributes.find(attrName);
			if(it != context.objects[ref - 1].structuredAttributes.end())
				return it->second.size;
			return 0;
		}

		template<typename T>
		void deserialize(const char* name, const BoundSetter<T>& setValue, const Context& dc)
		{
			if(!dc.isStructuredAttribute())
			{
				std::map<std::string, unsigned int>::const_iterator it = context.objects[ref - 1].standardAttributes.find(name);
				if(it != context.objects[ref - 1].standardAttributes.end())
				{
					// Position the buffer just before the typed value
					context.buffer.setPosition(it->second);

					// Get the value type
					unsigned int rawType = context.buffer.get32();
					ValueType valueType = *(ValueType*)&rawType;
					
					// Deserialize the value
					SPKLoader::ValueLoader<T> vl(&setValue, context);
					selectTemplate<SPKLoader::ValueLoader<T>&>(valueType, vl);
				}
				else
				{
					SPK_LOG_WARNING("SPKLoader::load(std::istream&) - Warning: attribute '" << name << "' not present in file");
				}
			}
			else
			{
				std::map<std::string, SPKLoader::LoadContext::Object::StructuredAttribute>::const_iterator it = context.objects[ref - 1].structuredAttributes.find(name);
				if(it != context.objects[ref - 1].structuredAttributes.end())
				{
					std::map<SPKLoader::LoadContext::Object::FieldKey, unsigned int>::const_iterator field =
						it->second.fields.find(SPKLoader::LoadContext::Object::FieldKey(dc.structured.fieldName, dc.structured.id));
					if(field != it->second.fields.end())
					{
						// Position the buffer just before the typed value
						context.buffer.setPosition(field->second);

						// Get the value type
						unsigned int rawType = context.buffer.get32();
						ValueType valueType = *(ValueType*)&rawType;
					
						// Deserialize the value
						SPKLoader::ValueLoader<T> vl(&setValue, context);
						selectTemplate<SPKLoader::ValueLoader<T>&>(valueType, vl);
					}
					else
					{
						SPK_LOG_WARNING("SPKLoader::load(std::istream&) - Warning: field '" << dc.structured.fieldName
							<< "'@" << dc.structured.id << " in attribute '" << name << "' not present in file");
					}
				}
				else
				{
					SPK_LOG_WARNING("SPKLoader::load(std::istream&) - Warning: attribute '" << name << "' not present in file");
				}
			}
		}

	private:
		SPKLoader::LoadContext& context;
		unsigned int ref;
	};
	
	// Loader
	// -----------------------------
	Ref<System> SPKLoader::load(std::istream& is)
	{
		// Header: Magic number
		if(is.peek() != SPKFormatVariables::MAGIC_NUMBER[0]
			|| (is.ignore() && is.peek() != SPKFormatVariables::MAGIC_NUMBER[1])
			|| (is.ignore() && is.peek() != SPKFormatVariables::MAGIC_NUMBER[2]))
		{
			SPK_LOG_ERROR("SPKLoader::load(std::istream&) - The stream does not contain a SPARK effect");
			return SPK_NULL_REF;
		}
		is.ignore();
			
		// Header: parse informations
		Buffer header(13, is);
		unsigned char version = header.get<unsigned char>();
		if(version != SPKFormatVariables::VERSION)
		{
			SPK_LOG_ERROR("SPKLoader::load(std::istream&) - Version of SPARK effect in stream (" << (int)version << ") does not match the version of the loader (" << (int)SPKFormatVariables::VERSION << ")");
			return SPK_NULL_REF;
		}
		unsigned int nbObjects = header.get32();
		unsigned int nbConnections = header.get32();
		unsigned int dataLength = header.get32();

		// Parse object list
		LoadContext context(is, dataLength);
		parseObjectList(context, nbObjects);

		// Parse the file
		for(unsigned int t = 0; t < nbObjects; t++)
			parseObject(context);

		// For number of connections, process data
		for(unsigned int t = 0; t < nbConnections; t++)
			parseConnection(context);

		// Deserialize to clone the setters of non-structured attributes
		for(unsigned int t = 0; t < nbObjects; t++)
		{
			Ref<SPKObject> rawObj = context.getObject<SPKObject>(t + 1);
			if(!rawObj) continue;
			SPKDeserializer deserializer(t + 1, context);
			rawObj->getDescription().deserialize(deserializer);
		}
		
		// Return system
		if(!context.systemRef)
		{
			SPK_LOG_ERROR("SPKLoader::load(std::istream&) - No system found");
			return SPK_NULL_REF;
		}
		return context.getObject<System>(context.systemRef);
	}

	void SPKLoader::parseObjectList(LoadContext& context, unsigned int nbObjects)
	{
		for(unsigned int t = 0; t < nbObjects; t++)
		{
			LoadContext::Object obj;
			obj.obj = Factory::getInstance().createObject(context.buffer.get<std::string>());
			context.objects.push_back(obj);
			
			if(obj.obj && obj.obj->getDescription().doesInherit("System"))
			{
				if(context.systemRef != 0)
				{
					SPK_LOG_ERROR("SPKLoader::load(std::istream&) - Too many systems");
				}
				else
					context.systemRef = t + 1;
			}
		}
	}

	void SPKLoader::parseConnection(LoadContext& context)
	{
		unsigned int senderId = context.buffer.get32();
		std::string control = context.buffer.get<std::string>();
		unsigned int receiverId = context.buffer.get32();
		std::string attribute = context.buffer.get<std::string>();
		unsigned int id = context.buffer.get32();
		std::string field = context.buffer.get<std::string>();
		
		Ref<SPKObject> sender = context.getObject<SPKObject>(senderId);
		Ref<SPKObject> receiver = context.getObject<SPKObject>(receiverId);
		ConnectionStatus status = connect(sender, control, receiver, attribute, id, field);

		if(status == CONNECTION_STATUS_OK_FIELD_NOT_NEEDED)
		{
			SPK_LOG_WARNING("XMLLoader::load(std::istream&) - Connection succeeded, but field is not needed: [" << senderId << "]::"
				<< control << " -> [" << receiverId << "]::" << attribute << " (\"" << field << "\"," << id << ")");
		}
		else if(status != CONNECTION_STATUS_OK)
		{
			SPK_LOG_ERROR("SPKLoader::load(std::istream&) - Connection failed: [" << senderId << "]::"
				<< control << " -> [" << receiverId << "]::" << attribute << " (\"" << field << "\"," << id << ") : error " << status);
		}
	}

	void SPKLoader::parseObject(LoadContext& context)
	{
		unsigned int ref = context.buffer.get32();
		unsigned int nbAttributes = context.buffer.get32();
		unsigned int dataLength = context.buffer.get32();

		Ref<SPKObject> rawObj = context.getObject<SPKObject>(ref);
		if(!rawObj)
		{
			context.buffer.skip(dataLength);
			return;
		}

		// Parse each attribute
		for(unsigned int a = 0; a < nbAttributes; a++)
		{
			std::string attrName = context.buffer.get<std::string>();
			bool isStructured = context.buffer.get<bool>();

			if(isStructured)
			{
				unsigned int count = context.buffer.get32();
				unsigned int fieldNb = context.buffer.get32();
				
				// Parse each field
				for(unsigned int f = 0; f < count * fieldNb; f++)
				{
					std::string fieldName = context.buffer.get<std::string>();
					unsigned int cellId = context.buffer.get32();
					context.objects[ref - 1].structuredAttributes[attrName].fields[LoadContext::Object::FieldKey(fieldName, cellId)] = context.buffer.getPosition();
					if(cellId >= context.objects[ref - 1].structuredAttributes[attrName].size)
						context.objects[ref - 1].structuredAttributes[attrName].size = cellId + 1;
					skipValue(context);
				}
			}
			else
			{
				context.objects[ref - 1].standardAttributes[attrName] = context.buffer.getPosition();
				skipValue(context);
			}
		}
	}

	void SPKLoader::skipValue(LoadContext& context)
	{
		// Type
		unsigned int rawType = context.buffer.get32();
		ValueType valueType = *(ValueType*)&rawType;

		// Value
		ValueLoader<bool> vl(0, context);
		selectTemplate<ValueLoader<bool>&>(valueType, vl);
	}
}
}
