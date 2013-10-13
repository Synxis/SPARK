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
#include "Extensions/IOConverters/SPK_IO_XMLSaver.h"

namespace SPK
{
namespace IO
{
	/**
	* @internal
	* @brief Variables used in the save process
	*/
	struct XMLSaver::SaveContext
	{
		/**
		* @internal
		* @brief Variables related to SPARK objects
		*/
		struct Object
		{
			pugi::xml_node node;
			SPKObject* object;
			unsigned int refCount;
			unsigned int id;
		};

		std::ostream& os;
		pugi::xml_document doc;
		pugi::xml_node root;
		std::vector<Object> objects;

		SaveContext(std::ostream& s) :
			os(s)
		{
		}

		int obj(SPKObject* obj)
		{
			for(unsigned int t = 0; t < objects.size(); t++)
				if(objects[t].object == obj)
					return (int)t;
			return -1;
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
	std::string formatValue(const T& value)
	{
		std::ostringstream os;
		os << std::boolalpha;
		os << value;
		return os.str();
	}

	/**
	* @internal
	* @brief Serializer used to create the xml nodes.
	*/
	class XMLSerializer : public SerializerConcept<XMLSerializer>
	{
	public:
		XMLSerializer(XMLSaver* s) : saver(s) {}

		/**
		* @brief Returns the context of a SPARK object; creates it if necessary
		*/
		XMLSaver::SaveContext::Object& getObjectContext(SPKObject* object)
		{
			XMLSaver::SaveContext* context = saver->context;
			int id = context->obj(object);
			XMLSaver::SaveContext::Object& obj = context->objects[id];

			if(obj.refCount == 0)
			{
				ClassDescription desc = object->getDescription();
				obj.node = context->root.append_child(desc.getClassName());
				obj.refCount = 1;
				obj.node.append_attribute("ref").set_value(formatValue(obj.id).c_str());
			}

			return obj;
		}

		/**
		* @internal
		* @brief Returns the specified attribute for the specified node, creating it if necessary
		*/
		pugi::xml_attribute getAttribute(pugi::xml_node& node, const std::string& name)
		{
			if(!node.attribute(name.c_str())) return node.append_attribute(name.c_str());
			else return node.attribute(name.c_str());
		}

		/**
		* @brief Stores a value in a xml node
		*/
		template<typename T>
		void storeValue(pugi::xml_node& node, const T& value)
		{
			node.append_attribute("value").set_value(formatValue(value).c_str());
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Pair<T>& value)
		{
			node.append_attribute("value1").set_value(formatValue(value.value1).c_str());
			node.append_attribute("value2").set_value(formatValue(value.value2).c_str());
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Triplet<T>& value)
		{
			node.append_attribute("value1").set_value(formatValue(value.value1).c_str());
			node.append_attribute("value2").set_value(formatValue(value.value2).c_str());
			node.append_attribute("value3").set_value(formatValue(value.value3).c_str());
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Quadruplet<T>& value)
		{
			node.append_attribute("value1").set_value(formatValue(value.value1).c_str());
			node.append_attribute("value2").set_value(formatValue(value.value2).c_str());
			node.append_attribute("value3").set_value(formatValue(value.value3).c_str());
			node.append_attribute("value4").set_value(formatValue(value.value4).c_str());
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const std::vector<T>& value)
		{
			std::string str;
			for(unsigned int t = 0; t < value.size(); t++)
			{
				if(t != 0)
					str += ";";
				str += formatValue(value[t]);
			}
			node.append_attribute("value").set_value(str.c_str());
		}

		/**
		* @brief Stores a reference in an attribute
		*/
		template<typename T>
		void storeReferenceInAttribute(pugi::xml_node& node, const Ref<T>& value, const char* aName)
		{
			if(value.get())
			{
				XMLSaver::SaveContext::Object& tgt = getObjectContext(value.get());
				node.append_attribute(aName).set_value(formatValue(tgt.id).c_str());
			}
			else
				node.append_attribute(aName).set_value("0");
		}

		/**
		* @brief Stores a value in a xml node, for references
		*/
		template<typename T>
		void storeValue(pugi::xml_node& node, const Ref<T>& value)
		{
			storeReferenceInAttribute(node, value, "ref");
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Pair<Ref<T> >& value)
		{
			storeReferenceInAttribute(node, value.value1, "ref1");
			storeReferenceInAttribute(node, value.value2, "ref2");
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Triplet<Ref<T> >& value)
		{
			storeReferenceInAttribute(node, value.value1, "ref1");
			storeReferenceInAttribute(node, value.value2, "ref2");
			storeReferenceInAttribute(node, value.value3, "ref3");
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const Quadruplet<Ref<T> >& value)
		{
			storeReferenceInAttribute(node, value.value1, "ref1");
			storeReferenceInAttribute(node, value.value2, "ref2");
			storeReferenceInAttribute(node, value.value3, "ref3");
			storeReferenceInAttribute(node, value.value4, "ref4");
		}

		template<typename T>
		void storeValue(pugi::xml_node& node, const std::vector<Ref<T> >& value)
		{
			for(unsigned int t = 0; t < value.size(); t++)
			{
				if(value[t].get())
				{
					XMLSaver::SaveContext::Object& tgt = getObjectContext(value[t].get());
					node.append_child(value[t]->getDescription().getClassName()) //T::description::getClassName())
						.append_attribute("ref").set_value(formatValue(tgt.id).c_str());
				}
			}
		}

		/**
		* @brief Reimplementation
		*/
		void emptyAttribute(const char* name, const Context& sc)
		{
			XMLSaver::SaveContext::Object& obj = getObjectContext(sc.object);
			pugi::xml_node attr = obj.node.append_child("attrib");
			attr.append_attribute("id").set_value(name);
		}

		/**
		* @brief Reimplementation
		*/
		template<typename T>
		void serialize(const char* attrName, const T& value, const SPK::IO::Context& sc)
		{
			XMLSaver::SaveContext* context = saver->context;
			XMLSaver::SaveContext::Object& obj = getObjectContext(sc.object);

			if(!sc.isStructuredAttribute())
			{
				pugi::xml_node attr = obj.node.append_child("attrib");
				attr.append_attribute("id").set_value(attrName);
				storeValue(attr, value);
			}
			else
			{
				pugi::xml_node attr;
				if(sc.structured.id == 0 && sc.structured.fieldIndex == 0)
				{
					attr = obj.node.append_child("attrib");
					attr.append_attribute("id").set_value(attrName);
				}
				else
					attr = obj.node.find_child(FindAttribute(attrName));

				pugi::xml_node item = attr.last_child();
				if(!item || sc.structured.fieldIndex == 0)
					item = attr.append_child("item");
				pugi::xml_node field = item.append_child("field");
				field.append_attribute("id").set_value(sc.structured.fieldName);
				storeValue(field, value);
			}
		}

	private:
		XMLSaver* saver;
	};

	XMLSaver::XMLSaver() :
		context(0),
		serializer(0)
	{
		serializer = SPK_NEW(XMLSerializer, this);
	}

	XMLSaver::~XMLSaver()
	{
		SPK_DELETE(serializer);
	}

	Serializer* XMLSaver::getSerializer()
	{
		return serializer;
	}

	void XMLSaver::beginSave(std::ostream& os, const std::vector<SPKObject*>& objRef)
	{
		context = SPK_NEW(SaveContext, os);

		// Objects
		for(unsigned int t = 0; t < objRef.size(); t++)
		{
			SaveContext::Object obj;
			obj.object = objRef[t];
			obj.refCount = 0;
			obj.id = t + 1;
			context->objects.push_back(obj);
		}

		// Header
		time_t currentTime = time(NULL);
		tm* timeinfo = localtime(&currentTime);
		std::string headerComment(" File generated by SPARK on ");
		headerComment += asctime(timeinfo);
		headerComment.replace(headerComment.size() - 1,1,1,' '); // replace the '\n' generated by a space
		context->doc.append_child(pugi::node_comment).set_value(headerComment.c_str());
		if (!author.empty())
			context->doc.append_child(pugi::node_comment).set_value((" Author : " + author + " ").c_str());

		// Root
		context->root = context->doc.append_child("SPARK");
		context->root.append_attribute("version").set_value(VERSION);
	}

	void XMLSaver::serializeConnection(const Ref<SPKObject>& sender, const std::string& ctrl,
			const Ref<SPKObject>& receiver, const std::string& attr, unsigned int fieldId, const std::string& field)
	{
		pugi::xml_node connection = context->root.append_child("connect");
		connection.append_attribute("sender").set_value(context->objects[context->obj(sender.get())].id);
		connection.append_attribute("control").set_value(ctrl.c_str());
		connection.append_attribute("receiver").set_value(context->objects[context->obj(receiver.get())].id);
		connection.append_attribute("attribute").set_value(attr.c_str());
		if(field != "")
		{
			connection.append_attribute("fieldId").set_value(fieldId);
			connection.append_attribute("field").set_value(field.c_str());
		}
	}

	bool XMLSaver::endSave()
	{
		if(!context)
			return false;

		context->doc.save(context->os,
			layout.indent.c_str(),
			layout.lineBreak ? (pugi::format_default) : (pugi::format_default | pugi::format_raw));
		SPK_DELETE(context);
		context = 0;
		return true;
	}
}
}

#endif
