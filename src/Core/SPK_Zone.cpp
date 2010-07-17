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

#include "Core/SPK_Zone.h"
#include "Core/SPK_Particle.h"

namespace SPK
{
	Zone::checkFn Zone::TEST_FN[Zone::NB_TEST_TYPES] =
	{
		&Zone::checkInside,
		&Zone::checkOutside,
		&Zone::checkIntersect,
		&Zone::checkEnter,
		&Zone::checkLeave,
	};

	bool Zone::checkInside(const Particle& particle) const
	{
		return contains(particle.position());
	}

	bool Zone::checkOutside(const Particle& particle) const
	{
		return !contains(particle.position());
	}

	bool Zone::checkIntersect(const Particle& particle) const
	{
		return intersects(particle.oldPosition(),particle.position(),particle.getParam(PARAM_SCALE) * particle.getGroup().getPhysicalRadius());
	}

	bool Zone::checkEnter(const Particle& particle) const
	{
		if (contains(particle.position()))
			return intersects(particle.oldPosition(),particle.position(),particle.getParam(PARAM_SCALE) * particle.getGroup().getPhysicalRadius());
		return false;
	}

	bool Zone::checkLeave(const Particle& particle) const
	{
		if (!contains(particle.position()))
			return intersects(particle.oldPosition(),particle.position(),particle.getParam(PARAM_SCALE) * particle.getGroup().getPhysicalRadius());
		return false;
	}
}
