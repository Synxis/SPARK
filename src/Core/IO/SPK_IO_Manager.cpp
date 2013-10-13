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

#include <cctype>
#include <SPARK.h> // not SPARK Core because we need all SPARK to register objects and converters

namespace SPK
{
namespace IO
{
	// -----------------------------------------------------------------------------------
	// Helper used to build a vector of all objects in a system
	// -----------------------------------------------------------------------------------
	class GraphBuilder : public SerializerConcept<GraphBuilder>
	{
	public:
		GraphBuilder(std::vector<SPKObject*>& n) : nodes(n) {}

		template<typename T>
		void addObject(const Ref<T>& value)
		{
			if(SPKObject* object = (SPKObject*)value.get())
			{
				for(unsigned int t = 0; t < nodes.size(); t++)
					if(nodes[t] == object)
						return;
				
				nodes.push_back(object);
				object->getDescription().serialize(*this);
			}
		}

		template<typename T>
		void serialize(const char*, const Ref<T>& value, const SPK::IO::Context&)
		{
			addObject(value);
		}

		template<typename T>
		void serialize(const char*, const Pair<Ref<T> >& value, const SPK::IO::Context&)
		{
			addObject(value.value1);
			addObject(value.value2);
		}

		template<typename T>
		void serialize(const char*, const Triplet<Ref<T> >& value, const SPK::IO::Context&)
		{
			addObject(value.value1);
			addObject(value.value2);
			addObject(value.value3);
		}

		template<typename T>
		void serialize(const char*, const Quadruplet<Ref<T> >& value, const SPK::IO::Context&)
		{
			addObject(value.value1);
			addObject(value.value2);
			addObject(value.value3);
			addObject(value.value4);
		}

		template<typename T>
		void serialize(const char*, const std::vector<Ref<T> >& value, const SPK::IO::Context&)
		{
			for(unsigned int t = 0; t < value.size(); t++)
				addObject(value[t]);
		}

		template<typename T>
		void serialize(const char*, const T&, const SPK::IO::Context&)
		{
		}

	private:
		std::vector<SPKObject*>& nodes;
	};

	// -----------------------------------------------------------------------------------
	// Helper used to serialize all connections of a system
	// -----------------------------------------------------------------------------------
	class ConnectionSerializer : public SerializerConcept<ConnectionSerializer>
	{
	public:
		ConnectionSerializer(Saver* s) : saver(s) {}

		void serializeConnections(const Ref<SPKObject>& object)
		{
			if(object)
			{
				for(OutConnectionsIterator it(object); !it.end(); ++it)
					saver->serializeConnection(it->sender(), it->control(), it->receiver(), it->attribute(), it->id(), it->field());
				object->getDescription().serialize(*this);
			}
		}

		template<typename T>
		void serialize(const char*, const Ref<T>& value, const SPK::IO::Context&)
		{
			serializeConnections(value);
		}

		template<typename T>
		void serialize(const char*, const Pair<Ref<T> >& value, const SPK::IO::Context&)
		{
			serializeConnections(value.value1);
			serializeConnections(value.value2);
		}

		template<typename T>
		void serialize(const char*, const Triplet<Ref<T> >& value, const SPK::IO::Context&)
		{
			serializeConnections(value.value1);
			serializeConnections(value.value2);
			serializeConnections(value.value3);
		}

		template<typename T>
		void serialize(const char*, const Quadruplet<Ref<T> >& value, const SPK::IO::Context&)
		{
			serializeConnections(value.value1);
			serializeConnections(value.value2);
			serializeConnections(value.value3);
			serializeConnections(value.value4);
		}

		template<typename T>
		void serialize(const char*, const std::vector<Ref<T> >& value, const SPK::IO::Context&)
		{
			for(unsigned int t = 0; t < value.size(); t++)
				serializeConnections(value[t]);
		}

		template<typename T>
		void serialize(const char*, const T&, const SPK::IO::Context&)
		{
		}

	private:
		Saver* saver;
	};

	// -----------------------------------------------------------------------------------
	// String utility
	// -----------------------------------------------------------------------------------
	std::string lowerString(const std::string& str)
	{
		std::string result(str);

		for (size_t i = 0; i < str.size(); ++i)
			result[i] = tolower(str[i]);

		return result;
	}

	// -----------------------------------------------------------------------------------
	// Registration utilities
	// -----------------------------------------------------------------------------------
	template<typename T>
	bool registerGeneric(const std::string& ext, T* t, std::map<std::string, T*>& table, bool overrideIfFound)
	{
		const std::string name = lowerString(ext);
		typename std::map<std::string, T*>::iterator it = table.find(name);
		if (it != table.end())
		{
			if(overrideIfFound)
			{
				SPK_LOG_WARNING("registerGeneric<T> - overriding " << name << "...")
				SPK_DELETE(it->second);
				table.erase(it);
			}
			else
				return false;
		}

		table.insert(std::make_pair(name,t));
		return true;
	}

	template<typename T>
	void unregisterGeneric(const std::string& ext, std::map<std::string, T*>& table)
	{
		const std::string name = lowerString(ext);
		typename std::map<std::string, T*>::iterator it = table.find(name);
		if (it != table.end())
		{
			SPK_DELETE(it->second);
			table.erase(it);
		}
	}

	template<typename T>
	T* getGeneric(const std::string& ext, const std::map<std::string, T*>& table)
	{
		const std::string name = lowerString(ext);
		typename std::map<std::string, T*>::const_iterator it = table.find(name);
		return it != table.end() ? it->second : 0;
	}

	// -----------------------------------------------------------------------------------
	// IO::Manager
	// -----------------------------------------------------------------------------------
	Manager& Manager::get()
	{
		static Manager instance;
		return instance;
	}

	Manager::Manager()
	{
		// Ensure MemoryTracer is created before the manager, because it will be used in the destructor
#ifdef SPK_TRACE_MEMORY
		SPK::SPKMemoryTracer::get();
#endif

		registerIOConverters();
	}

	Manager::~Manager()
	{
		unregisterIOConverters();
	}

	void Manager::registerIOConverters()
	{
		// SPK Converters
		registerLoader("spk", SPK_NEW(SPKLoader));
		registerSaver("spk", SPK_NEW(SPKSaver));

#ifndef SPK_NO_XML
		// XML converters
		registerLoader("xml", SPK_NEW(XMLLoader));
		registerSaver("xml", SPK_NEW(XMLSaver));
#endif
	}

	void Manager::unregisterIOConverters()
	{
		for (std::map<std::string,Loader*>::const_iterator it = loaders.begin(); it != loaders.end(); ++it)
			SPK_DELETE(it->second);
		loaders.clear();
		for (std::map<std::string,Saver*>::const_iterator it = savers.begin(); it != savers.end(); ++it)
			SPK_DELETE(it->second);
		savers.clear();
	}

	std::string Manager::getExtension(const std::string& path)
	{
		size_t index = path.find_last_of('.');
		
		if (index != std::string::npos && index != path.size() - 1)
			return lowerString(path.substr(index + 1));

		return "";
	}

	void Manager::registerLoader(const std::string& ext, Loader* loader)
	{
		registerGeneric(ext, loader, loaders, true);
	}

	void Manager::unregisterLoader(const std::string& ext)
	{
		unregisterGeneric(ext, loaders);
	}

	Loader* Manager::getLoader(const std::string& ext) const
	{
		return getGeneric(ext, loaders);
	}

	void Manager::registerSaver(const std::string& ext, Saver* saver)
	{
		registerGeneric(ext, saver, savers, true);
	}

	void Manager::unregisterSaver(const std::string& ext)
	{
		unregisterGeneric(ext, savers);
	}

	Saver* Manager::getSaver(const std::string& ext) const
	{
		return getGeneric(ext, savers);
	}

	Ref<System> Manager::load(const std::string& path) const
	{
		std::string ext = getExtension(path);
		std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
		Ref<System> system = load(ext, file);
		file.close();
		return system;
	}

	Ref<System> Manager::load(const std::string& ext, std::istream& is) const
	{
		Loader* loader = getLoader(ext);

		if(!loader)
		{
			SPK_LOG_ERROR("IO::Manager::load - no loader found for extension '" << ext << "'")
			return SPK_NULL_REF;
		}

		Ref<System> system = loader->load(is);

		if(system)
			SPK_LOG_INFO("IO::Manager::load - Loaded '" << ext << "' system")
		else
			SPK_LOG_ERROR("IO::Manager::load - Failed to load system for extension '" << ext << "'")
		return system;
	}

	bool Manager::save(const std::string& path, const Ref<System>& system) const
	{
		std::string ext = getExtension(path);
		std::ofstream file(path.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
		bool r = save(ext, file, system);
		file.close();
		return r;
	}

	bool Manager::save(const std::string& ext, std::ostream& os, const Ref<System>& system) const
	{
		if(!system)
		{
			SPK_LOG_ERROR("IO::Manager::save - cannot save null system")
			return false;
		}

		Saver* saver = getSaver(ext);

		if(!saver)
		{
			SPK_LOG_ERROR("IO::Manager::save - no saver found for extension '" << ext << "'")
			return false;
		}

		// Get list of objects to serialize
		std::vector<SPKObject*> objects;
		objects.push_back(system.get());
		GraphBuilder gb(objects);
		system->getDescription().serialize(gb);

		// Serialize the system
		saver->beginSave(os, objects);
		for(unsigned int t = 0; t < objects.size(); t++)
			objects[t]->getDescription().serialize(*saver->getSerializer());

		// Connections
		ConnectionSerializer cs(saver);
		system->getDescription().serialize(cs);
		
		bool succeeded = saver->endSave();
		if(succeeded)
			SPK_LOG_INFO("IO::Manager::save - Saved '" << ext << "' system")
		else
			SPK_LOG_ERROR("IO::Manager::save - Failed to save system for extension '" << ext << "'")
		return succeeded;
	}
}
}
