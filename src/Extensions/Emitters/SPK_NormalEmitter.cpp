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

#include "Extensions/Emitters/SPK_NormalEmitter.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Zone.h"

namespace SPK
{
	NormalEmitter::NormalEmitter(
		Zone* zone,
		bool full,
		int tank,
		float flow,
		float forceMin,
		float forceMax,
		Zone* normalZone,
		bool inverted) :
		Emitter(zone,full,tank,flow,forceMin,forceMax),
		normalZone(NULL),
		inverted(inverted)
	{
		setNormalZone(normalZone);
	}

	NormalEmitter::NormalEmitter(const NormalEmitter& emitter) :
		Emitter(emitter),
		inverted(emitter.inverted)
	{
		normalZone = dynamic_cast<Zone*>(copyChild(emitter.normalZone));
	}

	NormalEmitter::~NormalEmitter()
	{
		destroyChild(normalZone);
	}

	void NormalEmitter::setNormalZone(Zone* zone)
	{
		decrementChild(normalZone);
		incrementChild(zone);
		normalZone = zone;
	}

	Nameable* NormalEmitter::findByName(const std::string& name)
	{
		Nameable* object = Emitter::findByName(name);
		if (object != NULL) return object;

		if (normalZone != NULL)
			object = normalZone->findByName(name);

		return object;
	}

	void NormalEmitter::propagateUpdateTransform()
	{
		Emitter::propagateUpdateTransform();
		if (normalZone != NULL && !normalZone->isShared())
			normalZone->updateTransform(this);
	}

	void NormalEmitter::generateVelocity(Particle& particle,float speed) const
	{
		if (inverted) speed = -speed;
		const Zone* zone = (normalZone == NULL ? getZone() : normalZone);
		particle.velocity() = zone->computeNormal(particle.position()) * speed;
	}
}
