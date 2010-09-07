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

#ifndef H_SPK_RANDOMEMITTER
#define H_SPK_RANDOMEMITTER

#include "Core/SPK_Emitter.h"

namespace SPK
{
	/**
	* @class RandomEmitter
	* @brief An emitter that emits in a random direction
	*/
	class SPK_PREFIX RandomEmitter : public Emitter
	{
	SPK_IMPLEMENT_REGISTERABLE(RandomEmitter)

	public :

		/**
		* @brief Creates a new RandomEmitter
		* @return A new RandomEmitter
		*/
		static inline RandomEmitter* create(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f);

	private :

		inline RandomEmitter(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f);

		inline RandomEmitter(const RandomEmitter& emitter);

		virtual void generateVelocity(Particle& particle,float speed) const;
	};

	inline RandomEmitter::RandomEmitter(Zone* zone,bool full,int tank,float flow,float forceMin,float forceMax) :
		Emitter(zone,full,tank,flow,forceMin,forceMax)
	{}

	inline RandomEmitter::RandomEmitter(const RandomEmitter& emitter) : Emitter(emitter) {}

	inline RandomEmitter* RandomEmitter::create(Zone* zone,bool full,int tank,float flow,float forceMin,float forceMax)
	{
		return SPK_NEW(RandomEmitter,zone,full,tank,flow,forceMin,forceMax);
	}
}

#endif
