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

#include "Extensions/Actions/SPK_SpawnParticlesAction.h"
#include "Core/SPK_Logger.h"
#include "Core/SPK_Emitter.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_System.h"

namespace SPK
{
	SpawnParticlesAction::SpawnParticlesAction(unsigned int minNb,unsigned int maxNb,size_t groupIndex,Emitter* emitter) :
		minNb(minNb),
		maxNb(maxNb),
		baseEmitter(NULL),
		groupIndex(groupIndex)
	{
		setEmitter(emitter);
	}

	SpawnParticlesAction::SpawnParticlesAction(const SpawnParticlesAction& action) :
        Action(action),
		minNb(action.minNb),
		maxNb(action.maxNb),
		groupIndex(action.groupIndex),
		invalidBaseEmitter(action.invalidBaseEmitter),
		emitterPool()
	{
		baseEmitter = dynamic_cast<Emitter*>(copyChild(action.baseEmitter));
	}

	SpawnParticlesAction::~SpawnParticlesAction()
	{
		destroyChild(baseEmitter);
		resetPool();
	}

	void SpawnParticlesAction::setNb(unsigned int min,unsigned int max)
	{
		if (min <= max)
		{
			this->minNb = minNb;
			this->maxNb = maxNb;
		}
		else
		{
			SPK_LOG_WARNING("SpawnParticlesAction::setNb(unsigned int,unsigned int) - min is higher than max - Values are swapped");
			this->minNb = maxNb;
			this->maxNb = minNb;
		}
	}

	void SpawnParticlesAction::setEmitter(Emitter* emitter)
	{
		resetPool();
		decrementChild(baseEmitter);
		baseEmitter = emitter;
		incrementChild(baseEmitter);

		if (baseEmitter == NULL)
			invalidBaseEmitter = true;
		else if (baseEmitter->getZone()->isShared())
		{
			invalidBaseEmitter = true;
			SPK_LOG_WARNING("SpawnParticlesAction::setEmitter(Emitter*) - The base emitter is invalid (its zone is shared) and cannot be used");
		}
		else
			invalidBaseEmitter = false;
	}

	void SpawnParticlesAction::apply(Particle& particle) const
	{
		if (invalidBaseEmitter)
		{
			SPK_LOG_WARNING("SpawnParticlesAction::apply(Particle&) - The base emitter of the spawn particle action is invalid, nothing happens");
			return;
		}

		EmitterPair& emitterPair = getNextAvailableEmitter();

		Emitter* emitter = emitterPair.obj;
		emitter->setTank(baseEmitter->getTank());

		Zone* zone = emitter->getZone();
		zone->setTransformPosition(particle.position());
		zone->updateTransform();

		Group* group = particle.getGroup().getSystem().getGroup(groupIndex);
		emitterPair.group = group;
		group->addParticles(SPK_RANDOM(minNb,maxNb + 1),emitter);
	}

	SpawnParticlesAction::EmitterPair& SpawnParticlesAction::getNextAvailableEmitter() const
	{
		for (size_t i = 0; i < emitterPool.size(); ++i)
		{
			EmitterPair& emitterPair = emitterPool.front();
			emitterPool.pop_front();
			emitterPool.push_back(emitterPair);
			if (emitterPool.back().obj->getNbReferences() == 1) // the emitter is available
				return emitterPool.back();
		}

		// No emitter is available is the pool, a new one must be created
		EmitterPair emitterPair(dynamic_cast<Emitter*>(Registerable::copyRegisterable(baseEmitter)));
		emitterPair.obj->resetTransform();
		emitterPool.push_back(emitterPair);
		return emitterPool.back();
	}

	void SpawnParticlesAction::flushCurrentGroups()
	{
		for (std::deque<EmitterPair>::const_iterator it = emitterPool.begin(); it != emitterPool.end(); ++it)
			if (it->obj->getNbReferences() > 1)
				it->group->flushBufferedParticles();
	}

	void SpawnParticlesAction::resetPool()
	{
		flushCurrentGroups();
		for (std::deque<EmitterPair>::const_iterator it = emitterPool.begin(); it != emitterPool.end(); ++it)
		{
			it->obj->decrement();
			delete it->obj;
		}
		emitterPool.clear();
	}
}
