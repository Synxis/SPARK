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

#include "Extensions/Modifiers/SPK_Obstacle.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Iterator.h"

namespace SPK
{
	Obstacle::Obstacle(Zone* zone,float bouncingRatio,float friction) :
		Modifier(MODIFIER_PRIORITY_COLLISION,false,false),
		zone(zone),
		bouncingRatio(bouncingRatio),
		friction(friction)
	{
		incrementChild(zone);
	}

	Obstacle::Obstacle(const Obstacle& obstacle) :
		Modifier(obstacle),
		bouncingRatio(obstacle.bouncingRatio),
		friction(obstacle.friction)
	{
		zone = dynamic_cast<Zone*>(copyChild(obstacle.zone));
	}

	Obstacle::~Obstacle()
	{
		destroyChild(zone);
	}

	void Obstacle::setZone(Zone* zone)
	{
		decrementChild(this->zone);
		this->zone = zone;
		incrementChild(zone);
	}

	void Obstacle::modify(Group& group,DataSet* dataSet,float deltaTime) const
	{
		if (zone == NULL)
			return;

		for (GroupIterator particleIt(group); !particleIt.end(); ++particleIt)
		{
			if (zone->intersects(particleIt->oldPosition(),particleIt->position(),particleIt->getParam(PARAM_SIZE) * group.getRadius()))
			{
				Vector3D oldVelocity = particleIt->position() - particleIt->oldPosition(); 
				particleIt->position() = particleIt->oldPosition();

				Vector3D& velocity = particleIt->velocity();
				Vector3D normal = zone->computeNormal(particleIt->position());

				float dist = dotProduct(velocity,normal);

				normal *= dist - 0.001f;
				velocity -= normal;			// tangent component
				velocity *= friction;
				normal *= bouncingRatio;	// normal component
				if (dist > 0.0f)
					normal.revert();
				velocity -= normal;
			}
		}
	}

	Nameable* Obstacle::findByName(const std::string& name)
	{
		Nameable* object = Nameable::findByName(name);
		if (object != NULL) return object;

		if (zone != NULL)
			object = zone->findByName(name);

		return object;
	}

	void Obstacle::propagateUpdateTransform()
	{
		if (!zone->isShared())
			zone->updateTransform(this);
	}
}
