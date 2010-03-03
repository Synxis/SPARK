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

#ifndef H_SPK_SPAWNPARTICLESACTION
#define H_SPK_SPAWNPARTICLESACTION

#include <deque>

#include "Core/SPK_Action.h"

namespace SPK
{
	class Emitter;

	class SPK_PREFIX SpawnParticlesAction : public Action
	{
	SPK_IMPLEMENT_REGISTERABLE(SpawnParticlesAction)

	public :

		static inline SpawnParticlesAction* create(unsigned int minNb = 1,
			unsigned int maxNb = 1,
			size_t groupIndex = 0,
			Emitter* emitter = NULL);

		virtual ~SpawnParticlesAction();

		inline void setNb(unsigned int nb);
		void setNb(unsigned int min,unsigned int max);
		inline unsigned int  getMinNb() const;
		inline unsigned int  getMaxNb() const;

		void setEmitter(Emitter* emitter);
		inline Emitter* getEmitter() const;

		inline void setGroupIndex(size_t index);
		inline size_t getGroupIndex() const;

		virtual void apply(Particle& particle) const;

		void resetPool();

	private :

		unsigned int minNb;
		unsigned int maxNb;

		Emitter* baseEmitter;
		bool invalidBaseEmitter;

		size_t groupIndex;

		struct EmitterPair
		{
			Emitter* obj;
			Group* group;

			inline EmitterPair(Emitter* obj) :
				obj(obj),
				group(NULL)
			{}		
		};

		mutable std::deque<EmitterPair> emitterPool;
		
		SpawnParticlesAction(unsigned int minNb = 1,
			unsigned int maxNb = 1,
			size_t groupIndex = 0,
			Emitter* emitter = NULL);

		SpawnParticlesAction(const SpawnParticlesAction& action);

		EmitterPair& getNextAvailableEmitter() const;
		void flushCurrentGroups();
	};

	inline void SpawnParticlesAction::setNb(unsigned int nb)
	{
		setNb(nb,nb);
	}

	inline unsigned int SpawnParticlesAction::getMinNb() const
	{
		return minNb;
	}

	inline unsigned int SpawnParticlesAction::getMaxNb() const
	{
		return maxNb;
	}

	inline SpawnParticlesAction* SpawnParticlesAction::create(unsigned int minNb,unsigned int maxNb,size_t groupIndex,Emitter* emitter)
	{
		return new SpawnParticlesAction(minNb,maxNb,groupIndex,emitter);
	}

	inline Emitter* SpawnParticlesAction::getEmitter() const
	{
		return baseEmitter;
	}

	inline void SpawnParticlesAction::setGroupIndex(size_t index)
	{
		groupIndex = index;
	}

	inline size_t SpawnParticlesAction::getGroupIndex() const
	{
		return groupIndex;
	}
}

#endif

