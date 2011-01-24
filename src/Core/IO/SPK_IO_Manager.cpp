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

#include <cctype>
#include <SPARK_Core.h>

namespace SPK
{
namespace IO
{	
	IOManager& IOManager::getInstance()
	{
		static IOManager instance;
		return instance;
	}

	void IOManager::unregisterAll()
	{
		registeredObjects.clear();
		registeredLoaders.clear();
		registeredSavers.clear();
	}

	Ref<System> IOManager::load(const std::string& path) const
	{
		std::string name = getExtension(path);
		Loader* loader = getLoader(name);
		
		if (loader == NULL)
		{
			SPK_LOG_ERROR("IOManager::load(const std::string&) - The extension " << name << " is not known");
			return SPK_NULL_REF;
		}
		
		return loader->load(path);
	}

	Ref<System> IOManager::load(const std::string& ext,std::istream& is) const
	{
		std::string name = formatExtension(ext);
		Loader* loader = getLoader(name);
		
		if (loader == NULL)
		{
			SPK_LOG_ERROR("IOManager::load(const std::string&,std::ostream&) - The extension " << name << " is not known");
			return SPK_NULL_REF;
		}
		
		return loader->load(is);
	}

	bool IOManager::save(const std::string& path,const Ref<System>& system) const
	{
		std::string name = getExtension(path);
		Saver* saver = getSaver(name);

		if (saver == NULL)
		{
			SPK_LOG_ERROR("IOManager::saver(const std::string&,const System*) - The extension " << name << " is not known");
			return false;
		}
		
		return saver->save(path,system);
	}

	bool IOManager::save(const std::string& ext,std::ostream& os,const Ref<System>& system) const
	{
		std::string name = formatExtension(ext);
		Saver* saver = getSaver(name);

		if (saver == NULL)
		{
			SPK_LOG_ERROR("IOManager::saver(const std::string&,std::ostream& is,const System*) - The extension " << name << " is not known");
			return false;
		}

		return saver->save(os,system);
	}

	std::string IOManager::formatExtension(const std::string& ext)
	{
		std::string result(ext);

		for (size_t i = 0; i < ext.size(); ++i)
			result[i] = tolower(ext[i]);

		return result;
	}

	std::string IOManager::getExtension(const std::string& path)
	{
		size_t index = path.find_last_of('.');
		
		if (index != std::string::npos && index != path.size() - 1)
			return formatExtension(path.substr(index + 1));

		return "";
	}

	Ref<SPKObject> IOManager::createObject(const std::string& id) const
	{
		std::map<std::string,createSerializableFn>::const_iterator it = registeredObjects.find(id);
		if (it != registeredObjects.end())
			return (*it->second)();
		else
			return SPK_NULL_REF;
	}

	void IOManager::linkGroup(Group& group,System& system) const
	{
		group.system = &system;
		group.reallocate(1); // dummy allocation
	}
}}
