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

#ifndef H_SPK_EMITTER
#define H_SPK_EMITTER

#include "Core/SPK_DEF.h"
#include "Core/SPK_Registerable.h"
#include "Core/SPK_Transformable.h"

namespace SPK
{
	class Particle;

	/**
	* @brief An abstract class that defines an emitter of particles
	*
	* An Emitter is an object that launches particles by giving them an initial velocity and position.<br>
	* The position is derived from the zone of the emitter.<br>
	* The velocity is derived from the emitter itself.<br>
	* <br>
	* An Emitter has a flow and a tank of particles.
	* The flow defines the rate at which particles are launched and the tank defines the total number of Particles the Emitter can launch.<br>
	* An emitter is also defined by a range of forces (force min and force max) which defines the force at which particles are emitted (initial velocity = force / mass).<br>
	*/
	class SPK_PREFIX Emitter :	public Registerable, 
								public Transformable
	{
	friend class Group;

	public :

		////////////////
		// Destructor //
		////////////////

		/** @brief Destructor of emitter */
		virtual ~Emitter();

		////////////
		// Active //
		////////////

		/**
		* @brief Sets this emitter active or not.
		*
		* An inactive emitter will not emit in its parent Group during update.<br>
		*
		* @param active : true to activate this emitter, false to deactivate it
		*/
		inline void setActive(bool active);

		/**
		* @brief Tells whether this emitter is active or not
		* @return true if this emitter is active, false if is is inactive
		*/
		inline bool isActive() const;

		//////////
		// Tank //
		//////////

		/**
		* @brief Sets the number of particles in this emitter's tank
		*
		* Each time the emitter is updated, the number of particles emitted is deduced from the emitter tank.
		* When the tank reaches 0, the emitter will not emit any longer until it is refilled.<br>
		* <br>
		* A number of -1 (or any negative number) means the emitter has an infinite tank which will never be empty.<br>
		/ <br>
		* Note that having both negative tank and flow is illegal.
		*
		* @param tank : the number of particles in this emitters's tank
		*/
		void setTank(int tank);

		/**
		* @brief Gets the number of particles in this emitter's tank
		* @return the number of particles in this emitters's tank
		*/
		inline int getTank() const;

		//////////
		// Flow //
		//////////

		/**
		* @brief Sets the flow of this emitter
		*
		* The flow is in the unit : nb of particle per unit step.
		* A flow of -1 (or any negative number) indicates an infinite flow which means all particles in the emitters's tank are generated instantly.<br>
		* <br>
		* Note that having both negative tank and flow is illegal.
		*
		* @param flow : the flow of this emitter
		*/
		void setFlow(float flow);

		/**
		* @brief Gets the flow of this emitter
		* @return the flow of this emitter
		*/
		inline float getFlow() const;

		///////////
		// Force //
		///////////

		/**
		* @brief Sets the force of this emitter
		*
		* The force of the emitter varies for each launch of a Particle between a minimum and a maximum.
		* To have a fixed force for the Emitter, just have <i>min = max</i>.<br>
		* <br>
		* the speed at which a Particle will be launched is equal to : <i>force / mass of the Particle</i>.
		*
		* @param min : the minimum force of the emitter
		* @param max : the maximum force of the emitter
		*/
		void setForce(float min,float max);

		/**
		* @brief Gets the minimum force of this emitter
		* @return the minimum force of this emitter
		*/
		inline float getForceMin() const;

		/**
		* @brief Gets the maximum force of this emitter
		* @return the maximum force of this emitter
		*/
		inline float getForceMax() const;
		
		//////////
		// Zone //
		//////////

		/**
		* @brief Sets the zone of this emitter
		*
		* If the zone is NULL, the default Zone will be used (A Point centered at the origin)
		*
		* @param zone : the zone of this emitter
		* @param full : true to generate particles in the whole Zone, false to generate particles only at the zone's borders.
		*/
		void setZone(Zone* zone,bool full = true);

		/**
		* @brief Gets the zone of this emitter
		* @return the zone of this emitter
		*/
		inline Zone* getZone() const;

		/**
		* @brief Tells whether this emitter emits in the whole Zone or only at its borders
		* @return true if this emitter emits in the whole Zone, false if it is only at its borders
		*/
		inline bool isFullZone() const;

		void emit(Particle& particle) const;

	protected :

		Emitter(Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f);

		Emitter(const Emitter& emitter);

		void propagateUpdateTransform();

	private :

		bool active;

		int tank;
		float flow;

		float forceMin;
		float forceMax;

		Zone* zone;
		bool full;

		mutable float fraction;

		inline size_t updateTankFromTime(float deltaTime);
		inline size_t updateTankFromNb(size_t nb);

		virtual void generateVelocity(Particle& particle,float speed) const = 0;
	};

	inline void Emitter::setActive(bool active)
	{
		this->active = active;
	}
	
	inline bool Emitter::isActive() const
	{
		return active;
	}

	inline int Emitter::getTank() const
	{
		return tank;
	}

	inline float Emitter::getFlow() const
	{
		return flow;
	}

	inline float Emitter::getForceMin() const
	{
		return forceMin;
	}

	inline float Emitter::getForceMax() const
	{
		return forceMax;
	}

	inline Zone* Emitter::getZone() const
	{
		return zone;
	}

	inline bool Emitter::isFullZone() const
	{
		return full;
	}

	inline size_t Emitter::updateTankFromTime(float deltaTime)
	{
		if (deltaTime < 0.0f)
			return 0;

		size_t nbBorn;
		if (flow < 0.0f)
		{
			nbBorn = tank > 0 ? tank : 0;
			tank = 0;
		}
		else
		{
			fraction += flow * deltaTime;
			nbBorn = static_cast<size_t>(fraction);
			if (tank >= 0)
			{
				if (nbBorn > static_cast<size_t>(tank))
					nbBorn = tank;
				tank -= nbBorn;
			}
			fraction -= nbBorn;
		}
		return nbBorn;
	}

	inline size_t  Emitter::updateTankFromNb(size_t nb)
	{
		if (tank < 0)
			return nb;

		if (nb > static_cast<size_t>(tank))
			nb = tank;
		tank -= nb;
		return nb;
	}
}

#endif
