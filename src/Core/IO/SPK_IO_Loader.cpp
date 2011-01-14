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

#include <iostream>
#include <fstream>

#include <SPARK_Core.h>

namespace SPK
{
namespace IO
{
	System* Loader::load(std::istream& is) const
	{
		Graph graph;
		if (innerLoad(is,graph))
			return graph.system.get();
		else
			return NULL;
	}

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

	Loader::Node::Node(const WeakRef<SPKObject>& object) :
		object(object),
		descriptor(object->exportAttributes())
	{}

	bool Loader::Graph::addNode(size_t key,const std::string& name)
	{
		SPK_ASSERT(!nodesValidated,"Loader::Graph::addNode(size_t,string) - Graph has been processed. Nodes cannot be added any longer");

		WeakRef<SPKObject> object = IOManager::get().createObject(name);
		
		if (object == NULL)
		{
			SPK_LOG_ERROR("Loader::Graph::addNode(size_t,string) - The type \"" << name << "\" is not registered");
			return false;
		}

		if (getNode(key,false) != NULL)
		{
			SPK_LOG_ERROR("Loader::Graph::addNode(size_t,string) - duplicate key for nodes : " << key);
			return false;
		}
		
		Node* node = SPK_NEW(Node,object);
		nodes.push_back(node);
		key2Ptr.insert(std::make_pair(key,node));
		return true;
	}

	bool Loader::Graph::validateNodes()
	{
		size_t nbSystems = 0;
		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			WeakRef<System> systemRef = (*it)->object.cast<System>();
			if (systemRef != NULL) 
			{
				system = systemRef;
				++nbSystems;
			}
		}

		if (nbSystems != 1) // One and only one system is allowed
		{
			SPK_LOG_ERROR("Loader::Graph::validateNodes() - One and only one system is allowed in the graph");
			destroyAllNodes(true);
			return nodesValidated = false;
		}

		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			WeakRef<Group> groupRef = (*it)->object.cast<Group>();
			if (groupRef != NULL) // Associates each group with the system
				IOManager::get().linkGroup(*groupRef,*system);
		}

		return nodesValidated = true;
	}

	WeakRef<System>& Loader::Graph::finalize()
	{
		SPK_ASSERT(nodesValidated,"Loader::Graph::finalize() - Graph has not been validated before finalization");

		// First imports all descriptors to set up the objects
		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
			(*it)->object->importAttributes((*it)->descriptor);

		// Then deletes unused nodes to avoid memory leaks
		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			// Deletes the unused referenceables
			WeakRef<Referenceable> dummyRef = (*it)->object.cast<Referenceable>();
			if (dummyRef != NULL)
			{
				if (dummyRef->getNbReferences() == 0)
					destroyUnusedObject(**it);
			}
			else
			{
				WeakRef<Group> groupRef = (*it)->object.cast<Group>();
				if (&groupRef->getSystem() != system)
					destroyUnusedObject(**it);
			}
		}

		return system;
	}

	Loader::Node* Loader::Graph::getNode(size_t key,bool withCheck) const
	{
		if (!withCheck)
			SPK_ASSERT(nodesValidated,"Loader::Graph::getNode(size_t,boolean) - Graph has not been validated, nodes cannot be gotten");

		std::map<size_t,Node*>::const_iterator it = key2Ptr.find(key);
		if (it != key2Ptr.end())
			return it->second;
		else
			return NULL;
	}

	void Loader::Graph::destroyAllNodes(bool destroyRefs)
	{
		key2Ptr.clear();
		for (std::list<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			if (destroyRefs)
				SPK_DELETE((*it)->object.get());
			SPK_DELETE(*it);
			nodes.clear();
		}
	}

	void Loader::Graph::destroyUnusedObject(Node& node) const
	{
		WeakRef<SPKObject>& obj = node.object;
		SPK_LOG_WARNING("Loader::Graoh::destroyUnusedObject(const WeakRef<SPKObject>&) - The loaded object " << obj->getName() << " of type " << obj->getClassName() << "is never used");
		SPK_DELETE(obj.get());
		node.object = NULL;
	}
}}
