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
	static bool compareNodePriority(const Saver::Node* node0,const Saver::Node* node1)
	{
		return node0->priority < node1->priority || (node0->priority == node1->priority && node0->refID < node1->refID);
	}

	bool Saver::save(std::ostream& os,const WeakRef<const System>& system) const
	{
		if (system == NULL)
		{
			SPK_LOG_WARNING("Saver::saver(std::ostream&,const WeakRef<const System>&) - Impossible to write a NULL Syste. Nothing is done");
			return false;			
		}

		Graph graph;
		constructGraph(graph,system);
		return innerSave(os,graph);
	}

	bool Saver::save(const std::string& path,const WeakRef<const System>& system) const 
	{
		if (system == NULL)
		{
			SPK_LOG_WARNING("Saver::saver(const std::string&,const WeakRef<const System>&) - Impossible to write a NULL Syste. Nothing is done");
			return false;			
		}
		
		std::ofstream os(path.c_str(),std::ios::out | std::ios::binary | std::ios::trunc);
		if (os)
		{
			bool success = save(os,system);
			os.close();
			return success;
		}
		else
		{
			SPK_LOG_ERROR("Saver::saver(const std::string&,const WeakRef<const System>&) - Impossible to write to the file " << path);
			return false;
		}
	}

	void Saver::constructGraph(Saver::Graph& graph,const WeakRef<const System>& system)
	{
		constructNode(graph,const_cast<System*>(system.get()),0);
		graph.nodes.sort(compareNodePriority);			 
	}

	void Saver::constructNode(Saver::Graph& graph,const WeakRef<SPKObject>& object,size_t level)
	{
		if (object == NULL)
			return;

		std::map<WeakRef<SPKObject>,Node*>::iterator it = graph.ptr2Nodes.find(object);
		if (it != graph.ptr2Nodes.end())
		{
			Node* node = it->second;
			++node->nbReferences;
			if (node->priority > level)
				node->priority = level;
		}
		else
		{
			Node* node = graph.createNode(object->exportAttributes());
			node->priority = level;
			node->refID = graph.nodes.size() + 1;
			node->nbReferences = level > 0 ? 1 : 0;
			graph.ptr2Nodes.insert(std::make_pair(object,node));

			Descriptor& descriptor = node->descriptor;
			for (size_t i = 0; i < descriptor.getNbAttributes(); ++i)
			{
				Attribute& attribute = descriptor.getAttribute(i);
				if (attribute.getType() == ATTRIBUTE_TYPE_REF && attribute.hasValue())
					constructNode(graph,attribute.getValueRef(),level + 1);
				else if (attribute.getType() == ATTRIBUTE_TYPE_REFS && attribute.hasValue())
				{
					std::vector<WeakRef<SPKObject>>& refs = attribute.getValuesRef();
					for (std::vector<WeakRef<SPKObject>>::iterator it = refs.begin(); it != refs.end(); ++it)
						constructNode(graph,*it,level + 1);
				}
			}
		}
	}

	Saver::Node::Node(const Descriptor& descriptor) :
		refID(-1),
		processed(false),
		nbReferences(0),
		descriptor(descriptor),
		priority(0)
	{}

	Saver::Graph::Graph() : posInitialized(false) {}

	Saver::Graph::~Graph() 
	{
		for (std::list<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
			SPK_DELETE(*it);
	}

	Saver::Node* Saver::Graph::getNode(const WeakRef<SPKObject>& ptr) 
	{ 
		std::map<WeakRef<SPKObject>,Node*>::iterator it = ptr2Nodes.find(ptr);
		if (it != ptr2Nodes.end())
			return it->second;
		else
			return NULL; 
	}

	Saver::Node* Saver::Graph::getNextUnprocessedNode() 
	{ 
		if (!posInitialized)
		{
			currentPosIt = nodes.begin();
			posInitialized = true;
		}
		if (currentPosIt != nodes.end())
			return *(currentPosIt++);
		else
			return NULL; 
	}

	Saver::Node* Saver::Graph::createNode(Descriptor& descriptor)
	{
		Node* node = SPK_NEW(Node,descriptor);
		nodes.push_back(node);
		return node;
	}
}}
