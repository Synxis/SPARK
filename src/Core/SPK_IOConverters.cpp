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

#include <iostream>
#include <fstream>

#include <SPARK_Core.h>

namespace SPK
{
	System* Loader::load(const std::string& path) const
	{
		std::ifstream is(path.c_str(),std::ios::out | std::ios::binary | std::ios::trunc);
		if (is)
		{
			System* system = load(is);
			is.close();
			return system;
		}
		else
		{
			SPK_LOG_ERROR("Loader::load(const std::string&) - Impossible to read from the file " << path);
			return NULL;
		}
	}

	bool Saver::save(const std::string& path,const WeakRef<const System>& system) const 
	{
		std::ofstream os(path.c_str(),std::ios::out | std::ios::binary | std::ios::trunc);
		if (os)
		{
			bool success = save(os,system);
			os.close();
			return success;
		}
		else
		{
			SPK_LOG_ERROR("Saver::saver(const std::string&,const WeakRef<System>&) - Impossible to write to the file " << path);
			return false;
		}
	}
}
