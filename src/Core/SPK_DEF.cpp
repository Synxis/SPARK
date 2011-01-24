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

#include <ctime>
#include <limits>

#include <SPARK.h>
#include "Extensions/Zones/SPK_Point.h" // for default zone

namespace SPK
{
#ifdef SPK_TRACE_MEMORY
	SPKMemoryTracer& SPKMemoryTracer::getInstance() 
	{
		static SPKMemoryTracer instance;
		return instance;
	}
#endif

	SPKContext SPKContext::instance = SPKContext();

	// This allows SPARK initialization at application start up
	SPKContext::SPKContext() :
		defaultZone()
	{
		// Inits the random seed
		randomSeed = static_cast<unsigned int>(std::time(NULL));
		// little tweak to ensure the randomSeed is uniformly distributed along all the range
		for (size_t i = 0; i < 2; ++i)
			randomSeed = generateRandom(static_cast<unsigned int>(1),std::numeric_limits<unsigned int>::max());
	
		// Registers all core objects for loading
		// registerCoreForLoading();	
	}

	// This allows SPARK finalization at application exit
	SPKContext::~SPKContext()
	{
		release();
	}

	void SPKContext::release()
	{
		// Default zone is destroyed
		defaultZone.reset();
	}

	const Ref<Zone>& SPKContext::getDefaultZone()
	{
		if (defaultZone == NULL)
		{
			// Creates the zone by default
			defaultZone = Point::create();
			defaultZone->setShared(true);
		}
		return defaultZone;
	}

	/*void SPKContext::registerIOConverters()
	{

	}

	void SPKContext::registerCoreObjects()
	{
		IO::IOManager& manager = IO::IOManager::getInstance();

		// Core
		manager.registerObject<System>();	
		manager.registerObject<Group>();
		
		// Interpolators
		manager.registerObject<FloatDefaultInitializer>();	
		manager.registerObject<ColorDefaultInitializer>();
		manager.registerObject<FloatRandomInitializer>();
		manager.registerObject<ColorRandomInitializer>();
		manager.registerObject<FloatSimpleInterpolator>();
		manager.registerObject<ColorSimpleInterpolator>();
		manager.registerObject<FloatRandomInterpolator>();
		manager.registerObject<ColorRandomInterpolator>();
		manager.registerObject<FloatGraphInterpolator>();
		manager.registerObject<ColorGraphInterpolator>();

		// Zones
		manager.registerObject<Point>();
		manager.registerObject<Sphere>();
		manager.registerObject<Plane>();
		manager.registerObject<Ring>();

		// Emitters
		manager.registerObject<StaticEmitter>();
		manager.registerObject<RandomEmitter>();
		manager.registerObject<StraightEmitter>();
		manager.registerObject<SphericEmitter>();
		manager.registerObject<NormalEmitter>();

		// Modifiers
		manager.registerObject<Gravity>();
		manager.registerObject<Friction>();
		manager.registerObject<Obstacle>();
		manager.registerObject<Rotator>();
		manager.registerObject<Collider>();
		manager.registerObject<Destroyer>();
		manager.registerObject<Vortex>();
		manager.registerObject<EmitterAttacher>();
		manager.registerObject<PointMass>();
		manager.registerObject<RandomForce>();
		manager.registerObject<LinearForce>();

		// Actions
		manager.registerObject<ActionSet>();
		manager.registerObject<SpawnParticlesAction>();
	}*/
}
