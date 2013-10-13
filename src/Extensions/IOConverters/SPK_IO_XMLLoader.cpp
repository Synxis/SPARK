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

#ifndef SPK_NO_XML

#include <ctime>
#include <sstream>
#include <pugixml.hpp>
#include <SPARK_Core.h>
#include "Extensions/IOConverters/SPK_IO_XMLLoader.h"

namespace SPK
{
namespace IO
{
	struct XMLLoader::LoadContext
	{
		struct Object
		{
			Ref<SPKObject> obj;
			pugi::xml_node node;
		};

		std::vector<Object> objects;
		std::vector<pugi::xml_node> connections;
		pugi::xml_document doc;
		pugi::xml_node root;
		unsigned int systemRef;

		LoadContext() : systemRef(0) {}

		// Ref =! 0
		void setObject(unsigned int ref, const Ref<SPKObject>& obj, pugi::xml_node node)
		{
			ref -= 1;
			while(objects.size() <= ref)
				objects.push_back(Object());
			objects[ref].obj = obj;
			objects[ref].node = node;
		}

		template<typename T>
		Ref<T> getObject(unsigned int ref) const
		{
			if(ref > 0 && ref <= objects.size())
			{
				if(objects[ref - 1].obj && objects[ref - 1].obj->getDescription().doesInherit(T::description::getClassName()))
					return Ref<T>((T*)objects[ref - 1].obj.get());
			}
			return SPK_NULL_REF;
		}
	};

	/**
	* @brief Predicate for PugiXml, that finds a node based on the value of the attribute "id".
	*/
	struct FindAttribute
	{
		bool operator()(const pugi::xml_node& node)
		{
			return name == node.attribute("id").value();
		}

		FindAttribute(const char* n) : name(n) {}

		std::string name;
	};

	/**
	* @brief Formats a value in a string
	*/
	template<typename T>
	void decodeValue(const std::string& str, T& value)
	{
		std::istringstream is(str);
		is >> std::boolalpha;
		is >> value;
	}

	/**
	* @brief Deserializer
	*
	* Load values from a xml node.
	*/
	class XMLDeserializer : public DeserializerConcept<XMLDeserializer>
	{
	public:
		XMLDeserializer(pugi::xml_node objNode, XMLLoader::LoadContext& c) :
			context(c),
			parent(objNode),
			current(objNode.first_child())
		{
		}

		// Load value from xml
		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Ref<T> >& setValue)
		{
			Ref<T> obj = context.getObject<T>(attrib.attribute("ref").as_int());
			setValue(obj);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Pair<Ref<T> > >& setValue)
		{
			Ref<T> obj1 = context.getObject<T>(attrib.attribute("ref1").as_int());
			Ref<T> obj2 = context.getObject<T>(attrib.attribute("ref2").as_int());
			setValue(Pair<Ref<T> >(obj1, obj2));
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Triplet<Ref<T> > >& setValue)
		{
			Ref<T> obj1 = context.getObject<T>(attrib.attribute("ref1").as_int());
			Ref<T> obj2 = context.getObject<T>(attrib.attribute("ref2").as_int());
			Ref<T> obj3 = context.getObject<T>(attrib.attribute("ref3").as_int());
			setValue(Triplet<Ref<T> >(obj1, obj2, obj3));
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Quadruplet<Ref<T> > >& setValue)
		{
			Ref<T> obj1 = context.getObject<T>(attrib.attribute("ref1").as_int());
			Ref<T> obj2 = context.getObject<T>(attrib.attribute("ref2").as_int());
			Ref<T> obj3 = context.getObject<T>(attrib.attribute("ref3").as_int());
			Ref<T> obj4 = context.getObject<T>(attrib.attribute("ref4").as_int());
			setValue(Quadruplet<Ref<T> >(obj1, obj2, obj3, obj4));
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<std::vector<Ref<T> > >& setValue)
		{
			std::vector<Ref<T> > value;
			for(pugi::xml_node ref = attrib.first_child(); ref; ref = ref.next_sibling())
			{
				Ref<T> obj = context.getObject<T>(ref.attribute("ref").as_int());
				if(obj)
					value.push_back(obj);
			}
			setValue(value);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<T>& setValue)
		{
			T value = T();
			decodeValue(attrib.attribute("value").value(), value);
			setValue(value);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Pair<T> >& setValue)
		{
			Pair<T> value = Pair<T>();
			decodeValue(attrib.attribute("value1").value(), value.value1);
			decodeValue(attrib.attribute("value2").value(), value.value2);
			setValue(value);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Triplet<T> >& setValue)
		{
			Triplet<T> value = Triplet<T>();
			decodeValue(attrib.attribute("value1").value(), value.value1);
			decodeValue(attrib.attribute("value2").value(), value.value2);
			decodeValue(attrib.attribute("value3").value(), value.value3);
			setValue(value);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<Quadruplet<T> >& setValue)
		{
			Quadruplet<T> value = Quadruplet<T>();
			decodeValue(attrib.attribute("value1").value(), value.value1);
			decodeValue(attrib.attribute("value2").value(), value.value2);
			decodeValue(attrib.attribute("value3").value(), value.value3);
			decodeValue(attrib.attribute("value4").value(), value.value4);
			setValue(value);
		}

		template<typename T>
		void loadValue(pugi::xml_node attrib, const BoundSetter<std::vector<T> >& setValue)
		{
			std::string vec = attrib.attribute("value").value();
			std::vector<T> value;
			T buffer;
			size_t pos = 0;
			size_t next = vec.find(';');
			if(next - pos > 1)
			{
				decodeValue(vec.substr(pos + 1, next - pos - 1), buffer);
				value.push_back(buffer);
			}
			pos = next;
			while(pos != std::string::npos)
			{
				next = vec.find(';', pos + 1);
				if(next - pos > 1)
				{
					decodeValue(vec.substr(pos + 1, next - pos - 1), buffer);
					value.push_back(buffer);
				}
				pos = next;
			}
			setValue(value);
		}

		/** @brief Reimplementation */
		unsigned int sizeOfAttribute(const char* attrName)
		{
			pugi::xml_node attrib = parent.find_child(FindAttribute(attrName));
			
			// Count the 'item' elements in it
			unsigned int nbItems = 0;
			for(pugi::xml_node node = attrib.first_child(); node; node = node.next_sibling())
			{
				if(node.name() != std::string("item"))
				{
					SPK_LOG_ERROR("XMLLoader::load(std::istream&) - Found non-item element '" << node.name() << "' for structured attribute '" << attrName << "'.");
				}
				else
					nbItems++;
			}

			return nbItems;
		}

		/** @brief Reimplementation */
		template<typename T>
		void deserialize(const char* attrName, const BoundSetter<T>& setValue, const Context& dc)
		{
			pugi::xml_node attrib = parent.find_child(FindAttribute(attrName));
			if(!attrib)
			{
				SPK_LOG_WARNING("XMLLoader::load(std::istream&) - Missing attribute '" << attrName << "'. Default value will be used");
			}
			if(!dc.isStructuredAttribute())
				loadValue(attrib, setValue);
			else
			{
				// Find item node
				pugi::xml_node item = attrib.child("item");
				unsigned int id = dc.structured.id;
				while(id > 0)
				{
					item = item.next_sibling("item");
					id--;
				}
				
				// Find field node
				pugi::xml_node field = item.find_child(FindAttribute(dc.structured.fieldName));
				if(!field)
				{
					SPK_LOG_WARNING("XMLLoader::load(std::istream&) - Missing field '" << dc.structured.fieldName << "' in attribute '" << attrName << "'. Default value will be used");
				}
				loadValue(field, setValue);
			}
		}

	private:
		XMLLoader::LoadContext& context;
		pugi::xml_node parent, current;
	};

	Ref<System> XMLLoader::load(std::istream& is)
	{
		// Context + load document
		LoadContext context;
		pugi::xml_parse_result result = context.doc.load(is);

		if (!result)
		{
			SPK_LOG_ERROR("XMLLoader::load(std::istream&) - Error while parsing XML : " << result.description() << " at character " << result.offset);
			return SPK_NULL_REF;
		}

		context.root = context.doc.document_element();
		if (!context.root || std::string("SPARK") != context.root.name())
		{
			SPK_LOG_ERROR("XMLLoader::load(std::istream&) - The root element is not conform (must be <SPARK>)");
			return SPK_NULL_REF;
		}

		// Process nodes
		for(pugi::xml_node node = context.root.first_child(); node; node = node.next_sibling())
		{
			if(node.name() == std::string("connect"))
				context.connections.push_back(node);
			else
				processNode(node, context);
		}

		// Process objects
		for(unsigned int t = 0; t < context.objects.size(); t++)
		{
			if(context.objects[t].obj)
			{
				XMLDeserializer xml(context.objects[t].node, context);
				context.objects[t].obj->getDescription().deserialize(xml);
			}
		}

		// Process connections
		for(unsigned int t = 0; t < context.connections.size(); t++)
			processConnection(t, context);
		
		// Return system
		if(!context.systemRef)
		{
			SPK_LOG_ERROR("XMLLoader::load(std::istream&) - No system found");
			return SPK_NULL_REF;
		}
		return context.getObject<System>(context.systemRef);
	}

	void XMLLoader::processConnection(unsigned int id, LoadContext& context)
	{
		unsigned int senderRef = context.connections[id].attribute("sender").as_int();
		unsigned int receiverRef = context.connections[id].attribute("receiver").as_int();
		Ref<SPKObject> sender = context.getObject<SPKObject>(senderRef);
		Ref<SPKObject> receiver = context.getObject<SPKObject>(receiverRef);
		std::string control = context.connections[id].attribute("control").value();
		std::string attribute = context.connections[id].attribute("attribute").value();
		std::string field = context.connections[id].attribute("field").value();
		unsigned int fieldId = context.connections[id].attribute("fieldId").as_int();
		ConnectionStatus status = connect(sender, control, receiver, attribute, fieldId, field);
		if(status == CONNECTION_STATUS_OK_FIELD_NOT_NEEDED)
		{
			SPK_LOG_WARNING("XMLLoader::load(std::istream&) - Connection succeeded, but field is not needed: [" << senderRef << "]::"
				<< control << " -> [" << receiverRef << "]::" << attribute << " (\"" << field << "\"," << fieldId << ")");
		}
		else if(status != CONNECTION_STATUS_OK)
		{
			SPK_LOG_ERROR("XMLLoader::load(std::istream&) - Connection failed: [" << senderRef << "]::"
				<< control << " -> [" << receiverRef << "]::" << attribute << " (\"" << field << "\"," << fieldId << ") : error " << status);
		}
	}

	void XMLLoader::processNode(const pugi::xml_node& node, LoadContext& context)
	{
		int ref = node.attribute("ref").as_int();
		Ref<SPKObject> obj = Factory::getInstance().createObject(node.name());
		context.setObject(ref, obj, node);
		
		if(!obj)
		{
			SPK_LOG_ERROR("XMLLoader::processNode(const pugi::xml_node&, LoadContext&) - Unrecognized object type (\"" + std::string(node.name()) + "\")");
			return;
		}

		if(obj->getDescription().doesInherit("System"))
		{
			if(context.systemRef != 0)
			{
				SPK_LOG_ERROR("XMLLoader::processNode(const pugi::xml_node&, LoadContext&) - Too many systems");
			}
			else
				context.systemRef = ref;
		}
	}
}
}

#endif
