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

namespace SPK
{
	Emitter::Emitter(const Ref<Zone>& zone,bool full,int tank,float flow,float forceMin,float forceMax) :
		Referenceable(),
		Transformable(),
		active(true),
		zone(zone == NULL ? SPK_DEFAULT_ZONE : zone),
		flow(1.0f),
		fraction(0.0f)
	{
		setTank(tank);
		setFlow(flow);
		setForce(forceMin,forceMax);
	}

	Emitter::Emitter(const Emitter& emitter) :
		Referenceable(emitter),
		Transformable(emitter),
		active(emitter.active),
		flow(emitter.flow),
		tank(emitter.tank),
		forceMin(emitter.forceMin),
		forceMax(emitter.forceMax),
		fraction(0.0f)
	{
		zone = copyChild(emitter.zone);
	}

	Emitter::~Emitter() {}

	void Emitter::setTank(int tank)
	{
		SPK_ASSERT(flow >= 0.0f || tank >= 0,"Emitter::setTank(int) : the flow and tank of an emitter cannot be both negative");
		this->tank = tank;
	}

	void Emitter::setFlow(float flow)
	{
		SPK_ASSERT(flow >= 0.0f || tank >= 0,"Emitter::setFlow(float) : the flow and tank of an emitter cannot be both negative");
		this->flow = flow;
	}

	void Emitter::setForce(float min,float max)
	{
		if (forceMin <= forceMax)
		{
			forceMin = min;
			forceMax = max;
		}
		else
		{
			SPK_LOG_WARNING("Emitter::setForce(float,float) - min is higher than max - Values are swapped");
			forceMin = max;
			forceMax = min;
		}
	}

	void Emitter::setZone(const Ref<Zone>& zone,bool full)
	{
		this->zone = (zone == NULL ? SPK_DEFAULT_ZONE : zone);
		this->full = full;
	}

	void Emitter::propagateUpdateTransform()
	{
		if (!zone->isShared())
			zone->updateTransform(this);
	}

	void Emitter::emit(Particle& particle) const
	{
		zone->generatePosition(particle.position(),full);
		generateVelocity(particle,SPK_RANDOM(forceMin,forceMax) / particle.getParam(PARAM_MASS));
	}

	Nameable* Emitter::findByName(const std::string& name)
	{
		Nameable* object = Nameable::findByName(name);
		if (object != NULL) return object;

		return zone->findByName(name);
	}
}
