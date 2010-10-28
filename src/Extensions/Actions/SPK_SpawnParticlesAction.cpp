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

#include <SPARK_Core.h>
#include "Extensions/Actions/SPK_SpawnParticlesAction.h"

namespace SPK
{
	SpawnParticlesAction::SpawnParticlesAction(
		unsigned int minNb,
		unsigned int maxNb,
		size_t groupIndex,
		const Ref<Emitter>& emitter) :
		minNb(minNb),
		maxNb(maxNb),
		baseEmitter(),
		groupIndex(groupIndex)
	{
		setEmitter(emitter);
	}

	SpawnParticlesAction::SpawnParticlesAction(const SpawnParticlesAction& action) :
        Action(action),
		minNb(action.minNb),
		maxNb(action.maxNb),
		groupIndex(action.groupIndex),
		emitterPool()
	{
		baseEmitter = copyChild(action.baseEmitter);
	}

	SpawnParticlesAction::~SpawnParticlesAction(){}

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

	void SpawnParticlesAction::setEmitter(const Ref<Emitter>& emitter)
	{
		resetPool();
		baseEmitter = emitter;
	}

	void SpawnParticlesAction::apply(Particle& particle) const
	{
		if (!checkEmitterValidity())
			return;

		const Ref<Emitter>& emitter = getNextAvailableEmitter();
		emitter->setTank(baseEmitter->getTank());

		const Ref<Zone>& zone = emitter->getZone();
		zone->setTransformPosition(particle.position());
		zone->updateTransform();

		Group* group = particle.getGroup().getSystem().getGroup(groupIndex);
		group->addParticles(SPK_RANDOM(minNb,maxNb + 1),emitter);
	}

	bool SpawnParticlesAction::checkEmitterValidity() const
	{
		if (baseEmitter == NULL)
		{
			SPK_LOG_WARNING("SpawnParticlesAction::checkEmitterValidity() - The base emitter is NULL and cannot be used");
			return false;
		}
		else if (baseEmitter->getZone()->isShared())
		{
			SPK_LOG_WARNING("SpawnParticlesAction::checkEmitterValidity() - The base emitter is invalid (its zone is shared) and cannot be used");
			return false;
		}

		return true;
	}

	const Ref<Emitter>& SpawnParticlesAction::getNextAvailableEmitter() const
	{
		for (size_t i = 0; i < emitterPool.size(); ++i)
		{
			emitterPool.push_back(emitterPool.front());
			emitterPool.pop_front();
			if (emitterPool.back()->getNbReferences() == 1) // the emitter is available
				return emitterPool.back();
		}

		// No emitter is available is the pool, a new one must be created
		emitterPool.push_back(copyRegisterable(baseEmitter));
		emitterPool.back()->resetTransform();
		return emitterPool.back();
	}

	void SpawnParticlesAction::resetPool()
	{
		emitterPool.clear();
	}

	Nameable* SpawnParticlesAction::findByName(const std::string& name)
	{
		Nameable* object = Nameable::findByName(name);
		if (object != NULL) return object;

		if (baseEmitter != NULL)
			object = baseEmitter->findByName(name);

		return object;
	}
}
