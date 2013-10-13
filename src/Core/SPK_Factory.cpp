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
#include <SPARK.h>

namespace SPK
{
	Factory& Factory::getInstance()
	{
		static Factory instance;
		return instance;
	}

	Factory::Factory()
	{
		registerAll();
	}

	Factory::~Factory()
	{
		registeredTypes.clear();
	}

	void Factory::registerAll()
	{
		// Core
		registerType<System>();	
		registerType<Group>();
		
		// Interpolators
		registerType<FloatDefaultInitializer>();	
		registerType<ColorDefaultInitializer>();
		registerType<FloatRandomInitializer>();
		registerType<ColorRandomInitializer>();
		registerType<FloatSimpleInterpolator>();
		registerType<ColorSimpleInterpolator>();
		registerType<FloatRandomInterpolator>();
		registerType<ColorRandomInterpolator>();
		registerType<FloatGraphInterpolator>();
		registerType<ColorGraphInterpolator>();

		// Zones
		registerType<Point>();
		registerType<Sphere>();
		registerType<Plane>();
		registerType<Ring>();
		registerType<Box>();
		registerType<Cylinder>();

		// Emitters
		registerType<StaticEmitter>();
		registerType<RandomEmitter>();
		registerType<StraightEmitter>();
		registerType<SphericEmitter>();
		registerType<NormalEmitter>();

		// Modifiers
		registerType<Gravity>();
		registerType<Friction>();
		registerType<Obstacle>();
		registerType<Rotator>();
		registerType<Collider>();
		registerType<Destroyer>();
		registerType<Vortex>();
		registerType<EmitterAttacher>();
		registerType<PointMass>();
		registerType<RandomForce>();
		registerType<LinearForce>();

		// Actions
		registerType<ActionSet>();
		registerType<SpawnParticlesAction>();
	}

	Ref<SPKObject> Factory::createObject(const std::string& className) const
	{
		MapType::const_iterator it = registeredTypes.find(className);
		if(it != registeredTypes.end())
			return it->second();
		else
			return SPK_NULL_REF;
	}

	unsigned int Factory::getRegistrationNb() const
	{
		return registeredTypes.size();
	}
}
