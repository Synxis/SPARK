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

#ifndef H_SPK_IOMANAGER
#define H_SPK_IOMANAGER

#include <iostream>
#include <fstream>
#include <map>
#include <string>

namespace SPK
{
namespace IO
{
	/**
	* @brief Handles IO operations
	*
	* This class can save and load a system, and it is possible to change
	* the Loaders and Savers used (you can write one and register it inside
	* the manager).
	* Note that all registered loaders and savers are automatically deleted
	* when the manager is deleted (ie, end of program).
	* Advanced note: This deletion is done using SPK_DELETE, which means that
	* you must use the same version of your compiler (and the same configuration)
	* if you want to create loaders/savers.
	*/
	class SPK_PREFIX Manager
	{
	public:
		/**
		* @brief Get the instance of the manager
		*/
		static Manager& get();

		/**
		* @brief Registers a loader in the manager.
		* If a loader is already registered with the specified
		* extension, it is replaced.
		*/
		void registerLoader(const std::string& ext, Loader* loader);

		/**
		* @brief Unregisters the loader associated with the specified extension.
		*/
		void unregisterLoader(const std::string& ext);

		/**
		* @brief Returns a specific loader.
		*/
		Loader* getLoader(const std::string& ext) const;

		/**
		* @brief Registers a saver in the manager.
		* If a saver is already registered with the specified
		* extension, it is replaced.
		*/
		void registerSaver(const std::string& ext, Saver* saver);

		/**
		* @brief Unregisters the saver associated with the specified extension.
		*/
		void unregisterSaver(const std::string& ext);

		/**
		* @brief Returns a specific saver.
		*/
		Saver* getSaver(const std::string& ext) const;
		
		/**
		* @brief Loads a system from a file.
		*/
		Ref<System> load(const std::string& path) const;

		/**
		* @brief Loads a system from the specified input stream, with the saver specified via its registration extension.
		*/
		Ref<System> load(const std::string& ext, std::istream& is) const;

		/**
		* @brief Saves the specified system to a file, and returns whether it succeeded.
		*/
		bool save(const std::string& path, const Ref<System>& system) const;

		/**
		* @brief Saves the specified system to a stream using the saver specified via its registration extension.
		* Returns whether it succeeded.
		*/
		bool save(const std::string& ext, std::ostream& os, const Ref<System>& system) const;

	private:
		Manager();
		Manager(const Manager&) {}
		~Manager();
		Manager& operator=(const Manager&);
		
		static std::string getExtension(const std::string& path);
		void registerIOConverters();
		void unregisterIOConverters();

	private:
		std::map<std::string, Loader*> loaders;
		std::map<std::string, Saver*> savers;
	};
}}
#endif
