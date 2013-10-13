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

namespace SPK
{
	// Static constant definition
	const ValueType ToSPKType<void>::type = ValueType();

	const char* getBaseTypeName(BaseType type, bool spkName)
	{
		const char* name = "";
		meta::typeRelations::process<meta::get_spktype_name, const char*&>(name, type, spkName);
		return name;
	}

	const char* getTypeSpecifierName(TypeSpecifier ts, bool spkName)
	{
		switch(ts)
		{
		case TS_SINGLE: return spkName ? "TS_SINGLE" : "Singleton";
		case TS_PAIR: return spkName ? "TS_PAIR" : "Pair";
		case TS_TRIPLET: return spkName ? "TS_TRIPLET" : "Triplet";
		case TS_QUADRUPLET: return spkName ? "TS_QUADRUPLET" : "Quadruplet";
		case TS_ARRAY: return spkName ? "TS_ARRAY" : "Array";
		default: return "";
		}
	}

	std::string getValueTypeName(ValueType type, bool spkName)
	{
		const char* baseName = getBaseTypeName(type.base, spkName);
		const char* specifierName = getTypeSpecifierName(type.specifier, spkName);

		if(spkName)
			return std::string("(") + baseName + "," + specifierName + ")";
		else
			return type.specifier == TS_SINGLE ? baseName : std::string() + specifierName + "<" + baseName + ">";
	}
}
