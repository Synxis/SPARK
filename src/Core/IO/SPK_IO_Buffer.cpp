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

#include <SPARK_Core.h>

namespace SPK
{
namespace IO
{
	const bool Buffer::USE_LITTLE_ENDIANS = Buffer::isLittleEndians();

	Buffer::Buffer(size_t capacity) :
		capacity(capacity),
		size(0),
		position(0)
	{
		buf = SPK_NEW_ARRAY(char, capacity);
	}

	Buffer::Buffer(size_t capacity,std::istream& is) :
		capacity(capacity),
		size(0),
		position(0)
	{
		buf = SPK_NEW_ARRAY(char,capacity);
		is.read(buf,capacity);
		size = capacity;
	}

	Buffer::~Buffer()
	{
		SPK_DELETE_ARRAY(buf);
	}

	const char* Buffer::get(size_t length) const
	{
		size_t oldPos = position;
		position += length;
		if (position >= size) position = size;
		return buf + oldPos;
	}

	template<> std::string Buffer::get<std::string>() const	
	{ 
		char c;
		std::string str;
		while ((c = get<char>()) != '\0' && position < size) { str += c; }
		return str;
	}

	template<> Vector3D Buffer::get<Vector3D>() const	
	{ 
		float x = get<float>();
		float y = get<float>();
		float z = get<float>();
		return Vector3D(x,y,z); 
	}

	void Buffer::put(char c) 
	{ 
		updateSize(position + 1); 
		buf[position++] = c; 
	}

	void Buffer::put(const char* c,size_t length) 
	{ 
		updateSize(position + length);
		std::memcpy(buf + position,c,length);
		position += length;
	}

	void Buffer::updateSize(size_t newPosition)
	{
		size_t newCapacity = capacity;
		while (newPosition >= newCapacity)
			newCapacity <<= 1;	

		if (newCapacity != capacity)
		{
			char* newBuf = SPK_NEW_ARRAY(char, newCapacity);
			std::memcpy(newBuf, buf, size);
			SPK_DELETE_ARRAY(buf);
			buf = newBuf;
			capacity = newCapacity;
		}
		if (newPosition > size)
			size = newPosition;
	}

	bool Buffer::isLittleEndians()
	{
		uint32 test = 0x01;
		return (reinterpret_cast<char*>(&test)[0]) == 0x01;
	}
}}
