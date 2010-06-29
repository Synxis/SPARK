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

#ifndef H_SPK_STATICEMITTER
#define H_SPK_STATICEMITTER

#include "Core/SPK_Emitter.h"
#include "Core/SPK_Vector3D.h"

namespace SPK
{
	/**
	* @class StaticEmitter
	* @brief An Emitter that emits particles with no initial velocity
	*/
	class StaticEmitter : public Emitter
	{
	SPK_IMPLEMENT_REGISTERABLE(StaticEmitter)

	public :

		/**
		* @brief Creates a new StaticEmitter
		* @return A new StaticEmitter
		*/
		static inline StaticEmitter* create(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f);

	private :

		inline StaticEmitter(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f);

		virtual inline void generateVelocity(Particle& particle,float speed) const;
	};

	inline StaticEmitter::StaticEmitter(Zone* zone,bool full,int tank,float flow,float forceMin,float forceMax) :
		Emitter(zone,full,tank,flow,forceMin,forceMax)
	{}

	inline StaticEmitter* StaticEmitter::create(Zone* zone,bool full,int tank,float flow,float forceMin,float forceMax)
	{
		return new StaticEmitter(zone,full,tank,flow,forceMin,forceMax);
	}

	inline void StaticEmitter::generateVelocity(Particle& particle,float speed) const
	{
		particle.velocity().set(0.0f,0.0f,0.0f); // no initial velocity
	}
}

#endif