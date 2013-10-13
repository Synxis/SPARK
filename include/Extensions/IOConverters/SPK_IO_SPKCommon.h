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

#ifndef H_SPK_IO_SPKCOMMON
#define H_SPK_IO_SPKCOMMON

namespace SPK
{
namespace IO
{
	// Defined in SPK_IO_SPKSaver.cpp
	struct SPKFormatVariables
	{
		static const unsigned char MAGIC_NUMBER[3];
		static const unsigned char VERSION;
	};

	/// SPK FILE FORMAT, VERSION 0
	/*
		size		item
		------------------------------------------------
		3			Magic number			+
		1			Version					| Header
		4			Data length				| chunk
		4			Number of nodes			+

		-			Name, 0-terminated						+
		4			Data length								|
		4			Signature								| Node
															| chunk
		1			Value defined ?			+ Attribute		|
		-			Value					+ chunk			+
	*/

	/// SPK FILE FORMAT, VERSION 1
	/*
		size		item
		------------------------------------------------
		3			Magic number			+
		1			Version					|
		4			Number of nodes			| Header chunk
		4			Number of connections	|
		4			Data length				+


		-			Type, 0-terminated      + Array of object's type (= stringlist), in order of reference


		4			Reference of object								+
		4			Number of attributes							|
		4			Data length										|
																	| Node
		-			Name		+ Attribute header					| chunk
		1			Structured?	+									|
																	| Layout:
		4			Field entries count	+ Structured attr header	|		Attribute header
		4			Field number		+							|		Typed value
																	|	OR:
		4			Type		+ Typed value						|		Attribute header
		-			Value		+									|		Structured attribute header
																	|		Field header	|
		-			Field name	+ Field header						|		Typed value		| repeat
		4			Id (cell)	+									+



		4			Sender id				+
		-			Control name			|
		4			Receiver id				| Connection
		-			Attribute name			| chunk
		4			Id						|
		-			Field name				+
	*/
}
}

#endif
