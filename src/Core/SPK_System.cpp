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

#include <algorithm>
#include <limits> // for max float value

#include "Core/SPK_System.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_Registerable.h"

namespace SPK
{
	StepMode System::stepMode(STEP_MODE_REAL);
	float System::constantStep(0.0f);
	float System::minStep(0.0f);
	float System::maxStep(0.0f);

	bool System::clampStepEnabled(false);
	float System::clampStep(1.0f);

	Vector3D System::cameraPosition;

	System::System(const System& system) :
		Transformable(system),
		Nameable(system),
		deltaStep(0.0f),
		AABBComputationEnabled(system.AABBComputationEnabled),
		AABBMin(system.AABBMin),
		AABBMax(system.AABBMax),
		initialized(system.initialized)
	{
		Registerable::copyBuffer.clear();

		for (std::vector<Group*>::const_iterator it = system.groups.begin(); it != system.groups.end(); ++it)
			groups.push_back(SPK_NEW(Group,*this,**it));
	}
	
	System::~System()
	{
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			SPK_DELETE(*it);
	}

	Group* System::createGroup(size_t capacity)
	{
		if (capacity == 0)
		{
			SPK_LOG_WARNING("System::addGroup(size_t) - The capacity of a Group cannot be 0, NULL is returned");
			return NULL;
		}

		Group* newGroup = SPK_NEW(Group,*this,capacity);
		groups.push_back(newGroup);
		return newGroup;
	}

	Group* System::createGroup(const Group& group)
	{
		Group* newGroup = SPK_NEW(Group,*this,group);
		groups.push_back(newGroup);
		return newGroup;
	}

	void System::destroyGroup(Group* group)
	{
		std::vector<Group*>::iterator it = std::find(groups.begin(),groups.end(),group);
		if (it != groups.end())
		{
			SPK_DELETE(*it);
			groups.erase(it);
		}
		else
		{
			SPK_LOG_WARNING("System::removeGroup(Group*) - The group " << group << " was not found in the system and cannot be removed");
		}
	}

	size_t System::getNbParticles() const
	{
		size_t nbParticles = 0;
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			nbParticles += (*it)->getNbParticles();
		return nbParticles;
	}

	bool System::updateParticles(float deltaTime)
	{
		if (!initialized)
		{
			SPK_LOG_WARNING("System::updateParticles(float) - An uninitialized system cannot be updated");
			return true;
		}

		bool alive = true;

		if ((clampStepEnabled)&&(deltaTime > clampStep))
			deltaTime = clampStep;

		if (stepMode != STEP_MODE_REAL)
		{
			deltaTime += deltaStep;

			float updateStep;
			if (stepMode == STEP_MODE_ADAPTIVE)
			{
				if (deltaTime > maxStep)
					updateStep = maxStep;
				else if (deltaTime < minStep)
					updateStep = minStep;
				else
					updateStep = deltaTime;
			}
			else
				updateStep = constantStep;
	
			while(deltaTime >= updateStep)
			{
				if ((alive)&&(!innerUpdate(updateStep)))
					alive = false;
				deltaTime -= updateStep;
			}
			deltaStep = deltaTime;
		}	
		else
			alive = innerUpdate(deltaTime);

		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			(*it)->sortParticles();

		if (isAABBComputationEnabled())
		{
			const float maxFloat = std::numeric_limits<float>::max();
			AABBMin.set(maxFloat,maxFloat,maxFloat);
			AABBMax.set(-maxFloat,-maxFloat,-maxFloat);

			for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			{
				(*it)->computeAABB();

				AABBMin.setMin((*it)->getAABBMin());
				AABBMax.setMax((*it)->getAABBMax());
			}
		}
		else
		{
			const Vector3D pos = getWorldTransformPos();
			AABBMin = AABBMax = pos;
		}

		return alive;
	}

	void System::renderParticles() const
	{
		if (!initialized)
		{
			SPK_LOG_WARNING("System::updateParticles(float) - An uninitialized system cannot be rendered");
			return;
		}

		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			(*it)->renderParticles();
	}

	void System::initialize()
	{
		if (initialized)
			return; // the system is already initialized

		initialized = true;
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			(*it)->initData();
	}

	Nameable* System::findByName(const std::string& name)
	{
		Nameable* object = Nameable::findByName(name);
		if (object != NULL) return object;

		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
		{
			object = (*it)->findByName(name);
			if (object != NULL) return object;
		}

		return NULL;
	}

	bool System::innerUpdate(float deltaTime)
	{
		bool alive = false;
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			alive |= (*it)->updateParticles(deltaTime);
		return alive;
	}

	void System::propagateUpdateTransform()
	{
		for (std::vector<Group*>::const_iterator it = groups.begin(); it != groups.end(); ++it)
			(*it)->propagateUpdateTransform();
	}
}
