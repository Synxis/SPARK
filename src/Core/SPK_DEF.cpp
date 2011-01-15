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

#include <SPARK_Core.h>
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
		registerCoreForLoading();	
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

	void registerCoreForLoading()
	{
		//Loader::Table& table = Loader::table();

		// Core
		/*table.registerSerializable<System>();	
		table.registerSerializable<Group>();
		
		// Interpolators
		table.registerSerializable<FloatDefaultInitializer>();	
		table.registerSerializable<ColorDefaultInitializer>();
		table.registerSerializable<FloatRandomInitializer>();
		table.registerSerializable<ColorRandomInitializer>();
		table.registerSerializable<FloatSimpleInterpolator>();
		table.registerSerializable<ColorSimpleInterpolator>();
		table.registerSerializable<FloatRandomInterpolator>();
		table.registerSerializable<ColorRandomInterpolator>();
		table.registerSerializable<FloatGraphInterpolator>();
		table.registerSerializable<ColorGraphInterpolator>();

		// Zones
		table.registerSerializable<Point>();
		table.registerSerializable<Sphere>();
		table.registerSerializable<Plane>();
		table.registerSerializable<Ring>();

		// Emitters
		table.registerSerializable<StaticEmitter>();
		table.registerSerializable<RandomEmitter>();
		table.registerSerializable<StraightEmitter>();
		table.registerSerializable<SphericEmitter>();
		table.registerSerializable<NormalEmitter>();

		// Modifiers
		table.registerSerializable<Gravity>();
		table.registerSerializable<Friction>();
		table.registerSerializable<Obstacle>();
		table.registerSerializable<Rotator>();
		table.registerSerializable<Collider>();
		table.registerSerializable<Destroyer>();
		table.registerSerializable<Vortex>();
		table.registerSerializable<EmitterAttacher>();
		table.registerSerializable<PointMass>();
		table.registerSerializable<RandomForce>();
		table.registerSerializable<LinearForce>();

		// Actions
		table.registerSerializable<ActionSet>();
		table.registerSerializable<SpawnParticlesAction>();*/
	}
}
