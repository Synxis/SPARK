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

#ifndef H_SPARK
#define H_SPARK

// Defines
#include "Core/SPK_DEF.h"

// Core
#include "Core/SPK_Logger.h"
#include "Core/SPK_Vector3D.h"
#include "Core/SPK_Color.h"
#include "Core/SPK_RenderBuffer.h"
#include "Core/SPK_DataSet.h"
#include "Core/SPK_ArrayData.h"
#include "Core/SPK_DataHandler.h"
#include "Core/SPK_Registerable.h"
#include "Core/SPK_Transformable.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Interpolator.h"
#include "Core/SPK_Emitter.h"
#include "Core/SPK_Modifier.h"
#include "Core/SPK_ZonedModifier.h"
#include "Core/SPK_Renderer.h"
#include "Core/SPK_Action.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Iterator.h"
#include "Core/SPK_System.h"

// Interpolators
#include "Extensions/Interpolators/SPK_DefaultInitializer.h"
#include "Extensions/Interpolators/SPK_RandomInitializer.h"
#include "Extensions/Interpolators/SPK_SimpleInterpolator.h"
#include "Extensions/Interpolators/SPK_RandomInterpolator.h"
#include "Extensions/Interpolators/SPK_GraphInterpolator.h"

// Zones
#include "Extensions/Zones/SPK_Point.h"
#include "Extensions/Zones/SPK_Sphere.h"
#include "Extensions/Zones/SPK_Plane.h"
#include "Extensions/Zones/SPK_Ring.h"

// Emitters
#include "Extensions/Emitters/SPK_StaticEmitter.h"
#include "Extensions/Emitters/SPK_RandomEmitter.h"
#include "Extensions/Emitters/SPK_StraightEmitter.h"
#include "Extensions/Emitters/SPK_SphericEmitter.h"
#include "Extensions/Emitters/SPK_NormalEmitter.h"

// Modifiers
#include "Extensions/Modifiers/SPK_BasicModifiers.h"
#include "Extensions/Modifiers/SPK_Obstacle.h"
#include "Extensions/Modifiers/SPK_Rotator.h"
#include "Extensions/Modifiers/SPK_Collider.h"
#include "Extensions/Modifiers/SPK_Destroyer.h"
#include "Extensions/Modifiers/SPK_Vortex.h"
#include "Extensions/Modifiers/SPK_EmitterAttacher.h"
#include "Extensions/Modifiers/SPK_PointMass.h"
#include "Extensions/Modifiers/SPK_RandomForce.h"

// Actions
#include "Extensions/Actions/SPK_ActionSet.h"
#include "Extensions/Actions/SPK_SpawnParticlesAction.h"

// Renderers
#include "Extensions/Renderers/SPK_PointRendererInterface.h"
#include "Extensions/Renderers/SPK_LineRendererInterface.h"
#include "Extensions/Renderers/SPK_QuadRendererInterface.h"
#include "Extensions/Renderers/SPK_Oriented3DRendererInterface.h"

#endif
