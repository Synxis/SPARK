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

#ifndef H_SPARK_CORE
#define H_SPARK_CORE

#include "Core/SPK_DEF.h"
#include "Core/SPK_Logger.h"
#include "Core/SPK_Vector3D.h"
#include "Core/SPK_Color.h"
#include "Core/SPK_Meta.h"
#include "Core/SPK_Types.h"
#include "Core/SPK_TypeOperations.h"
#include "Core/SPK_Getters.h"
#include "Core/SPK_Setters.h"
#include "Core/IO/SPK_IO_Context.h"
#include "Core/IO/SPK_IO_Deserializer.h"
#include "Core/IO/SPK_IO_Serializer.h"
#include "Core/SPK_Connections.h"
#include "Core/SPK_DescriptionDefines.h"
#include "Core/SPK_Traits.h"
#include "Core/SPK_Attributes.h"
#include "Core/SPK_ClassDescription.h"
#include "Core/SPK_StaticDescription.h"
#include "Core/SPK_Transform.h"
#include "Core/SPK_Object.h"
#include "Core/SPK_Controls.h"
#include "Core/SPK_Transformable.h"
#include "Core/SPK_ConnectionIterators.h"
#include "Core/SPK_RenderBuffer.h"
#include "Core/SPK_DataSet.h"
#include "Core/SPK_ArrayData.h"
#include "Core/SPK_DataHandler.h"
#include "Core/SPK_Controller.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Interpolator.h"
#include "Core/SPK_Emitter.h"
#include "Core/SPK_Modifier.h"
#include "Core/SPK_ZonedModifier.h"
#include "Core/SPK_Renderer.h"
#include "Core/SPK_Action.h"
#include "Core/SPK_System.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Iterator.h"
#include "Core/SPK_Octree.h"
#include "Core/SPK_Factory.h"
#include "Core/IO/SPK_IO_Loader.h"
#include "Core/IO/SPK_IO_Saver.h"
#include "Core/IO/SPK_IO_Manager.h"
#include "Core/IO/SPK_IO_Buffer.h"

#endif
