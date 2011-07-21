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
#include "Extensions/IOConverters/SPK_IO_SPKFormatHandler.h"

namespace SPK
{
namespace IO
{
	const char SPKFormatHandler::MAGIC_NUMBER[3] = { 0x53, 0x50, 0x4B }; // "SPK" in ASCII
	const char SPKFormatHandler::VERSION = 0;

	const size_t SPKFormatHandler::DATA_LENGTH_OFFSET = 4;
	const size_t SPKFormatHandler::HEADER_LENGTH = 12;

	const bool SPKFormatHandler::Buffer::USE_LITTLE_ENDIANS = SPKFormatHandler::Buffer::isLittleEndians();

	SPKFormatHandler::Buffer::Buffer(size_t capacity) :
		capacity(capacity),
		size(0),
		position(0)
	{
		buf = SPK_NEW_ARRAY(char,capacity);
	}

	SPKFormatHandler::Buffer::Buffer(size_t capacity,std::istream& is) :
		capacity(capacity),
		size(0),
		position(0)
	{
		buf = SPK_NEW_ARRAY(char,capacity);
		is.read(buf,capacity);
		size = capacity;
	}

	SPKFormatHandler::Buffer::~Buffer()
	{
		SPK_DELETE_ARRAY(buf);
	}

	bool SPKFormatHandler::Buffer::isLittleEndians()
	{
		uint32 test = 0x01;
		return (reinterpret_cast<char*>(&test)[0]) == 0x01;
	}
}}
