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

#ifndef H_SPK_IO_XMLLOADER
#define H_SPK_IO_XMLLOADER

#include <sstream>

class TiXmlElement;

namespace SPK
{
namespace IO
{
	class SPK_PREFIX XMLLoader : public Loader
	{
	private :

		virtual bool innerLoad(std::istream& is,Graph& graph) const;

		const std::string* getValue(const TiXmlElement& element) const;
		Ref<SPKObject> getRef(const TiXmlElement& element,const std::map<int,size_t>& ref2Index,const Graph& graph) const;
		Ref<SPKObject> getObject(const TiXmlElement* element,const std::vector<const TiXmlElement*>& objElements,const Graph& graph) const;

		void findObjects(const TiXmlElement& parent,std::vector<const TiXmlElement*>& objElements,std::map<int,size_t>& ref2Index,bool objectLevel) const;
		void parseAttributes(const TiXmlElement& element,Descriptor& desc,const Graph& graph,const std::vector<const TiXmlElement*>& objElements,const std::map<int,size_t>& ref2Index) const;

		template<typename T,typename U> void setAttributeValue(Attribute& attribute,const TiXmlElement& element,void (Attribute::*f)(U,bool)) const;
		template<typename T> void setAttributeValueArray(Attribute& attribute,const TiXmlElement& element,void (Attribute::*f)(const T*,size_t,bool)) const;

		template<typename T> static bool convert(const std::string& str,T& value);
		template<typename T> static bool convert2Array(const std::string& str,std::vector<T>& values);
	};

	template<typename T> 
	bool XMLLoader::convert(const std::string& str,T& value)
	{
		std::istringstream is(str);
		is >> std::boolalpha;
		return is >> value && is.eof();
	}

	template<typename T> 
	bool XMLLoader::convert2Array(const std::string& str,std::vector<T>& values)
	{
		values.clear();

		if (str.empty())
			return false;

		size_t oldPos = 0;
		size_t pos = 0;
		T tmp = T();

		do
		{
			pos = str.find(';',oldPos);
			std::string subStr = str.substr(oldPos,pos);
			if (convert(subStr,tmp))
				values.push_back(tmp);
			oldPos = pos + 1;
		}
		while (pos != std::string::npos);

		return !values.empty();
	}

	template<typename T,typename U> // T is the explicit form of U as U can be of type "const type&" and we need a mutable type within the method
	void XMLLoader::setAttributeValue(Attribute& attribute,const TiXmlElement& element,void (Attribute::*f)(U,bool)) const
	{
		T tmp = T();
		const std::string* value = getValue(element);
		if (value != NULL && convert(*value,tmp))
			(attribute.*f)(tmp,false);
	}

	template<typename T> // Here we dont need an explicit template param
	void XMLLoader::setAttributeValueArray(Attribute& attribute,const TiXmlElement& element,void (Attribute::*f)(const T*,size_t,bool)) const
	{
		std::vector<T> tmp;
		const std::string* value = getValue(element);
		if (value != NULL && convert2Array(*value,tmp)) // Ok because convertArray ensures the vector is not empty 
			(attribute.*f)(&tmp[0],tmp.size(),false);
	}

	// Specialization for bool as vector<bool> is handled differently in the standard (we cannot cast a vector<bool> to a bool* with &v[0])
	template<>
	void SPK_PREFIX XMLLoader::setAttributeValueArray(Attribute& attribute,const TiXmlElement& element,void (Attribute::*f)(const bool*,size_t,bool)) const;
}}

#endif