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

#ifndef H_SPK_IO_SPKLOADER
#define H_SPK_IO_SPKLOADER

#include "SPK_IO_SPKCommon.h"

namespace SPK
{
namespace IO
{
	class SPKDeserializer;
	struct SPKHelper;

	/** @brief A class to deserialize a System from a spk format stream */
	class SPK_PREFIX SPKLoader : public Loader
	{
	public:
		Ref<System> load(std::istream& is);

	private:
		friend class SPKDeserializer;
		friend struct SPKHelper;
		struct LoadContext;
		template<typename T> struct ValueLoader;

		void parseObjectList(LoadContext& context, unsigned int nbObjects);
		void parseConnection(LoadContext& context);
		void parseObject(LoadContext& context);
		void skipValue(LoadContext& context);
	};
}
}

#endif
