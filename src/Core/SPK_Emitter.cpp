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

#include <SPARK_Core.h>

namespace SPK
{
	Emitter::Emitter(const Ref<Zone>& zone,bool full,int tank,float flow,float forceMin,float forceMax) :
		Transformable(),
		active(true),
		full(full),
		zone(!zone ? SPK_DEFAULT_ZONE : zone),
		flow(1.0f),
		fraction(SPK_RANDOM(0.0f,1.0f))
	{
		setTank(tank);
		setFlow(flow);
		setForce(forceMin,forceMax);
	}

	Emitter::Emitter(const Emitter& emitter) :
		Transformable(emitter),
		active(emitter.active),
		full(emitter.full),
		flow(emitter.flow),
		minTank(emitter.minTank),
		maxTank(emitter.maxTank),
		forceMin(emitter.forceMin),
		forceMax(emitter.forceMax),
		fraction(SPK_RANDOM(0.0f,1.0f))
	{
		zone = emitter.copyChild(emitter.zone);
		resetTank();
	}

	Emitter::~Emitter() {}

	void Emitter::setTank(int minTank,int maxTank)
	{
		SPK_ASSERT(minTank >= 0 == maxTank >= 0,"Emitter::setTank(int,int) : min and max tank values must be of the same sign");
		if (minTank < 0 || maxTank < 0) minTank = maxTank = -1;
		if (minTank > maxTank)
		{
			SPK_LOG_WARNING("Emitter::setTank(int,int) : min tank is greater than max tank. Values are swapped");
			std::swap(minTank,maxTank);
		}
		this->minTank = minTank;
		this->maxTank = maxTank;
		resetTank();
		SPK_ASSERT(flow >= 0.0f || currentTank >= 0,"Emitter::setTank(int,int) : the flow and tank of an emitter cannot be both negative");
	}

	void Emitter::setFlow(float flow)
	{
		SPK_ASSERT(flow >= 0.0f || currentTank >= 0,"Emitter::setFlow(float) : the flow and tank of an emitter cannot be both negative");
		this->flow = flow;
	}

	void Emitter::setForce(float min,float max)
	{
		if (min <= max)
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

	void Emitter::setZone(const Ref<Zone>& zone)
	{
		this->zone = (!zone ? SPK_DEFAULT_ZONE : zone);
	}

	void Emitter::propagateUpdateTransform()
	{
		if (!zone->isShared())
			zone->updateTransform(this);
	}

	void Emitter::emitIt(Particle& particle) const
	{
		zone->generatePosition(particle.position(),full,particle.getRadius());
		generateVelocity(particle,SPK_RANDOM(forceMin,forceMax) / particle.getParam(PARAM_MASS));
	}

	Ref<SPKObject> Emitter::findByName(const std::string& name)
	{
		const Ref<SPKObject>& object = SPKObject::findByName(name);
		if (object) return object;

		return zone->findByName(name);
	}
}
