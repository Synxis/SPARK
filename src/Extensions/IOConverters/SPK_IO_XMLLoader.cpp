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

#include <ctime>

#define TIXML_USE_STL
#include <tinyxml.h>

#include <SPARK_Core.h>
#include "Extensions/IOConverters/SPK_IO_XMLLoader.h"

namespace SPK
{
namespace IO
{
	bool XMLLoader::innerLoad(std::istream& is,Graph& graph) const
	{
		TiXmlDocument doc;
		is >> doc;

		if (doc.Error())
		{
			SPK_LOG_ERROR("An error occured while parsing XML : " << doc.ErrorDesc() << " at line " << doc.ErrorRow() << " and column " << doc.ErrorCol()); 
			return false;
		}

		const TiXmlElement* root = doc.RootElement(); 
		if (root == NULL || root->ValueStr() != "SPARK")
		{
			SPK_LOG_ERROR("An error occured while parsing XML : The root element is not conform (must be <SPARK>)");
			return false;
		}

		// Iterates in XML to find the objects and fill the graph
		std::vector<const TiXmlElement*> objElements; // This will allow to find back are objects description once created
		std::map<int,size_t> ref2Index; // This map allows to find back objects index in the vector from their reference id
		
		findObjects(*root,objElements,ref2Index,true);
		for (size_t i = 0; i < objElements.size(); ++i)
			graph.addNode(i,objElements[i]->ValueStr());

		if (!graph.validateNodes())
			return false;

		// Fills up descriptors
		for (size_t i = 0; i < objElements.size(); ++i)
		{
			const TiXmlElement& element = *objElements[i];

			Node* node = graph.getNode(i);
			if (node != NULL)
			{
				Descriptor& desc = node->getDescriptor();	
				parseAttributes(element,desc,graph,objElements,ref2Index);
			}
		}

		return true;
	}

	const std::string* XMLLoader::getValue(const TiXmlElement& element) const
	{
		const std::string* tmp = NULL;
		tmp = element.Attribute(std::string("value"));
		if (tmp == NULL)
		{
			const TiXmlText* txt = (element.FirstChild() != NULL ? element.FirstChild()->ToText() : NULL);
			if (txt != NULL)
				return &txt->ValueStr();
		}
		return tmp;
	}

	Ref<SPKObject> XMLLoader::getRef(const TiXmlElement& element,const std::map<int,size_t>& ref2Index,const Graph& graph) const
	{
		const std::string* tmpStr = NULL;
		tmpStr = element.Attribute(std::string("ref"));
		if (tmpStr == NULL)
		{
			const TiXmlText* txt = (element.FirstChild() != NULL ? element.FirstChild()->ToText() : NULL);
			if (txt != NULL)
				tmpStr = &txt->ValueStr();
		}

		if (tmpStr == NULL) // No string value for ref is found
			return SPK_NULL_REF;
		
		int refID = -1;
		if (convert(*tmpStr,refID))	// The string value can be converted into a refID
		{
			std::map<int,size_t>::const_iterator it = ref2Index.find(refID);
			if (it != ref2Index.end()) // The refID is found in the list of object	
				return graph.getNode(it->second)->getObject();
		}

		return SPK_NULL_REF;
	}

	Ref<SPKObject> XMLLoader::getObject(const TiXmlElement* element,const std::vector<const TiXmlElement*>& objElements,const Graph& graph) const
	{
		if (element != NULL)
			for (size_t i = 0; i < objElements.size(); ++i) // This can be optimized in complexity by using a map
				if (objElements[i] == element)
					return graph.getNode(i)->getObject();

		return SPK_NULL_REF;
	}

	void XMLLoader::findObjects(const TiXmlElement& parent,std::vector<const TiXmlElement*>& objElements,std::map<int,size_t>& ref2Index,bool objectLevel) const
	{
		for (const TiXmlElement* element = parent.FirstChildElement(); element != NULL; element = element->NextSiblingElement())
		{
			if (objectLevel && element->ValueStr() != "attrib" && element->ValueStr() != "Ref")
			{
				objElements.push_back(element);

				int referenceID = -1;
				if (element->Attribute("ref",&referenceID) != NULL)
					if (!ref2Index.insert(std::make_pair(referenceID,objElements.size() - 1)).second)
						SPK_LOG_ERROR("An object with an already used reference was found");
			}

			findObjects(*element,objElements,ref2Index,!objectLevel);
		}
	}

	void XMLLoader::parseAttributes(
		const TiXmlElement& element,
		Descriptor& desc,
		const Graph& graph,
		const std::vector<const TiXmlElement*>& objElements,
		const std::map<int,size_t>& ref2Index) const
	{
		// name
		const std::string* name = element.Attribute(std::string("name"));
		if (name != NULL && !name->empty())
			desc.getAttribute("name")->setValueString(*name); // We assume name is not NULL

		for (const TiXmlElement* attrib = element.FirstChildElement(); attrib != NULL; attrib = attrib->NextSiblingElement())
			if (attrib->ValueStr() == "attrib")
			{
				name = attrib->Attribute(std::string("id"));
				if (name != NULL)
				{
					Attribute* attribute = desc.getAttribute(*name);
					if (attribute != NULL)
					{
						switch(attribute->getType())
						{
						case ATTRIBUTE_TYPE_CHAR :		setAttributeValue<char>(*attribute,*attrib,&Attribute::setValueChar); break;
						case ATTRIBUTE_TYPE_BOOL :		setAttributeValue<bool>(*attribute,*attrib,&Attribute::setValueBool); break;
						case ATTRIBUTE_TYPE_INT32 :		setAttributeValue<long int>(*attribute,*attrib,&Attribute::setValueInt32); break;
						case ATTRIBUTE_TYPE_UINT32 :	setAttributeValue<unsigned long int>(*attribute,*attrib,&Attribute::setValueUint32); break;
						case ATTRIBUTE_TYPE_FLOAT :		setAttributeValue<float>(*attribute,*attrib,&Attribute::setValueFloat); break;
						case ATTRIBUTE_TYPE_VECTOR :	setAttributeValue<Vector3D>(*attribute,*attrib,&Attribute::setValueVector); break;
						case ATTRIBUTE_TYPE_COLOR :		setAttributeValue<Color>(*attribute,*attrib,&Attribute::setValueColor); break;
						case ATTRIBUTE_TYPE_STRING :	setAttributeValue<std::string>(*attribute,*attrib,&Attribute::setValueString); break;
						
						case ATTRIBUTE_TYPE_CHARS :		setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesChar); break;
						case ATTRIBUTE_TYPE_BOOLS :		setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesBool); break;
						case ATTRIBUTE_TYPE_INT32S :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesInt32); break;
						case ATTRIBUTE_TYPE_UINT32S :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesUint32); break;
						case ATTRIBUTE_TYPE_FLOATS :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesFloat); break;
						case ATTRIBUTE_TYPE_VECTORS :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesVector); break;
						case ATTRIBUTE_TYPE_COLORS :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesColor); break;
						case ATTRIBUTE_TYPE_STRINGS :	setAttributeValueArray(*attribute,*attrib,&Attribute::setValuesString); break;

						case ATTRIBUTE_TYPE_REF : {
							Ref<SPKObject> obj = getRef(*attrib,ref2Index,graph);
							if (obj == NULL)
								obj = getObject(attrib->FirstChildElement(),objElements,graph); 
							if (obj != NULL)
								attribute->setValueRef(obj);
							break; }

						case ATTRIBUTE_TYPE_REFS : {
							std::vector<Ref<SPKObject>> objects;
							for (const TiXmlElement* child = attrib->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
							{
								Ref<SPKObject> tmp;
								if (child->ValueStr() == "Ref")
									tmp = getRef(*child,ref2Index,graph);
								else 
									tmp = getObject(child,objElements,graph);
								if (tmp != NULL)
									objects.push_back(tmp);
							}
							if (!objects.empty())
								attribute->setValuesRef(&objects[0],objects.size());	
							break; }

						default : SPK_LOG_ERROR("XML ERROR 3");
						}
					}
				}
			}
	}
}}
