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
	ZonedModifier::ZonedModifier(
		unsigned int PRIORITY,
		bool NEEDS_DATASET,
		bool CALL_INIT,
		int ZONE_TEST_FLAG,
		ZoneTest zoneTest,
		const Ref<Zone>& zone) :
		Modifier(PRIORITY,NEEDS_DATASET,CALL_INIT),
		ZONE_TEST_FLAG(ZONE_TEST_FLAG),
		zoneTest(zoneTest),
		zone()
	{
		setZone(zone,zoneTest);
	}

	ZonedModifier::ZonedModifier(const ZonedModifier& zonedModifier) :
		Modifier(zonedModifier),
		ZONE_TEST_FLAG(zonedModifier.ZONE_TEST_FLAG),
		zoneTest(zonedModifier.zoneTest)
	{
		zone = copyChild(zonedModifier.zone);
	}

	ZonedModifier::~ZonedModifier(){}

	void ZonedModifier::setZone(const Ref<Zone>& zone)
	{
		this->zone = zone == NULL ? SPK_DEFAULT_ZONE : zone;
	}

	void ZonedModifier::setZoneTest(ZoneTest zoneTest)
	{
		if ((1 << zoneTest) & ZONE_TEST_FLAG)
			this->zoneTest = zoneTest;
		else
		{
			SPK_LOG_WARNING("ZonedModifier::setZoneTest(ZoneTest) - This ZoneTest is not valid for this modifier");
			bool validTestFound = false;
			for (size_t i = 0; i < NB_ZONE_TESTS; ++i)
			{
				if ((1 << i) & ZONE_TEST_FLAG)
				{
					this->zoneTest = static_cast<ZoneTest>(1 << i);
					validTestFound = true;
					break;
				}
				if (!validTestFound)
					SPK_LOG_ERROR("ZonedModifier::setZoneTest(ZoneTest) - The ZONE_TEST_FLAG of modifier is empty"); 
			}
		}
	}

	Nameable* ZonedModifier::findByName(const std::string& name)
	{
		Nameable* object = Nameable::findByName(name);
		if (object != NULL) return object;

		object = zone->findByName(name);

		return object;
	}

	void ZonedModifier::propagateUpdateTransform()
	{
		if (!zone->isShared())
			zone->updateTransform(this);
	}
}
