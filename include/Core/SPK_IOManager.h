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

#ifndef H_SPK_IOMANAGER
#define H_SPK_IOMANAGER

#include <iostream>
#include <fstream>
#include <map>
#include <string>

namespace SPK
{
	class SPK_PREFIX IOManager
	{
	public : 

		static IOManager& get();
	
		template<typename T> void registerObject();
		template<typename T> void unregisterObject();
		template<typename T> inline bool isObjectRegistered() const;

		inline void registerLoader(const std::string& ext,const Loader& loader);
		inline void unregisterLoader(const std::string& ext);
		inline WeakRef<Loader> getLoader(const std::string& ext) const;

		inline void registerSaver(const std::string& ext,const Saver& saver);
		inline void unregisterSaver(const std::string& ext);
		inline WeakRef<Saver> getSaver(const std::string& ext) const;

		System* load(const std::string& path) const;
		System* load(const std::string& ext,std::istream& is) const;

		bool save(const std::string& path,const System* system) const;
		bool save(const std::string& ext,std::ostream& os,const System* system) const;

	private :

		typedef SPKObject* (*createSerializableFn)();
		
		std::map<std::string,createSerializableFn> registeredObjects;
		std::map<std::string,Loader*> registeredLoaders;
		std::map<std::string,Saver*> registeredSavers;

		IOManager() {};
		IOManager(const IOManager&) {}
		IOManager& operator=(const IOManager&);

		static std::string formatExtension(const std::string& ext);
		static std::string getExtension(const std::string& path);

		template<typename T> static void registerGeneric(const std::string& ext,const T& t,std::map<std::string,T*>& table);
		template<typename T> static void unregisterGeneric(const std::string& ext,std::map<std::string,T*>& table);
		template<typename T> static WeakRef<T> getGeneric(const std::string& ext,const std::map<std::string,T*>& table);
	};

	template<typename T> void IOManager::registerObject()
	{
		const std::string name = T::getSerializableName();
		if (registeredObjects.find(name) != registeredObjects.end())
			SPK_LOG_WARNING("IOManager::registerSerializable<T> - " << name << " is already registered");

		registeredObjects.insert(std::pair(name,&(T::createSerializable)));
	}

	template<typename T> void IOManager::unregisterObject()
	{
		const std::string name = T::getSerializableName();
		std::map<std::string,createSerializableFn>::iterator it = registeredObjects.find(name);
		if (it == registeredObjects.end())
		{
			SPK_LOG_WARNING("IOConverter::unregisterSerializable<T> - " << name << " is not registered and therefore cannot be unregistered");
		}
		else
			registeredObjects.erase(it)
	}

	template<typename T> inline bool IOManager::isObjectRegistered() const
	{
		return registeredObjects.find(T::getSerializableName()) != registeredObjects.end()	
	}

	inline void IOManager::registerLoader(const std::string& ext,const Loader& loader) { registerGeneric<Loader>(ext,loader,registeredLoaders); }
	inline void IOManager::unregisterLoader(const std::string& ext) { unregisterGeneric<Loader>(ext,registeredLoaders); }
	inline WeakRef<Loader> IOManager::getLoader(const std::string& ext) const { return getGeneric<Loader>(ext,registeredLoaders); }

	inline void IOManager::registerSaver(const std::string& ext,const Saver& saver) { registerGeneric<Saver>(ext,saver,registeredSavers); }
	inline void IOManager::unregisterSaver(const std::string& ext) { unregisterGeneric<Saver>(ext,registeredSavers); }
	inline WeakRef<Saver> IOManager::getSaver(const std::string& ext) const { return getGeneric<Saver>(ext,registeredSavers); }

	template<typename T>
	void IOManager::registerGeneric(const std::string& ext,const T& t,std::map<std::string,T*>& table)
	{
		const std::string name = formatExtension(ext);
		std::map<std::string,T*>::iterator it = table.find(name);
		if (it != table.end())
		{
			SPK_LOG_WARNING("IOManager::registerGeneric<T> - " << name << " is already registered. Previous one is overriden");
			delete it->second;
			table.erase(it);
		}

		table.insert(std::pair<std::string,T*>(name,t.clone()));
	}

	template<typename T>
	void IOManager::unregisterGeneric(const std::string& ext,std::map<std::string,T*>& table)
	{
		const std::string name = formatExtension(ext);
		std::map<std::string,T*>::iterator it = table.find(name);
		if (it == table.end())
		{
			SPK_LOG_WARNING("IOManager::registerGeneric<T> - " << name << " is not registered and therefore cannot be unregistered")
		}
		else
		{
			delete it->second;
			table.erase(it);
		}
	}

	template<typename T>
	WeakRef<T> IOManager::getGeneric(const std::string& ext,const std::map<std::string,T*>& table)
	{
		const std::string name = formatExtension(ext);
		std::map<std::string,T*>::const_iterator it = table.find(name);
		return it->second;
	}
}
#endif 
