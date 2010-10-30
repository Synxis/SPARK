//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2010 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_DESCRIPTOR
#define H_SPK_DESCRIPTOR

namespace SPK
{
	class Attribute
	{
	public : 

		Attribute(std::string& name,AttributeType type);
		
	private :

		std::string& name;
	}

	class Descriptor
	{
	public :

		Descriptor(Attribute* attributes,size_t nb)
		
		setAttributeFloat(const std::string& name,float value);
		setAttributeInt(const std::string& name,int value);
		setAttributeColor(const std::string& name,Color value);
		setAttributeVector3D(const std::string& name,const Vector3D& value);
		setAttributeChar(const std::string& name,char attribute);
		setAttributePtr(const std::string& name,const Serializable* value);

		float getAttributeFloat(const std::string& name);
		int getAttributeInt(const std::string& name);
		Color getAttributeColor(const std::string& name);
		Vector3D getAttributeVector3D(const std::string& name);
		Char getAttributeChar(const std::string& name);
		Serializable* getAttributePtr(const std::string& name);

	private :

		char* buffer;
		size_t bufferSize;
		size_t offset;

		char[4] signature;

		void Attribute(const std::string& name,void* value,size_t size)
		void* getAttribute(const str::string& name,char* offset,);

		std::Set<Attribute> attributesPerName;
		std::Set<Attribute> attributesPerID;
	};
}

#endif
