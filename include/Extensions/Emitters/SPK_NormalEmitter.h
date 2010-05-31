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

#ifndef H_SPK_NORMALEMITTER
#define H_SPK_NORMALEMITTER

#include "Core/SPK_Emitter.h"

namespace SPK
{
	/**
	* @class NormalEmitter
	* @brief An emitter that emits particles following a Zone normals
	*
	* The Zone used to derive the direction of emission can either be the emitter's zone
	* or another zone that can be set with setNormalZone(Zone*).<br>
	* If the normal zone is NULL the emitter's zone is used.
	*/
	class SPK_PREFIX NormalEmitter : public Emitter
	{
	SPK_IMPLEMENT_REGISTERABLE(NormalEmitter)

	public :

		/** @brief Creates a new normalEmitter */
		static inline NormalEmitter* create(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f,
			Zone* normalZone = NULL,
			bool inverted = false);

		virtual ~NormalEmitter();

		/////////////////
		// Normal Zone //
		/////////////////

		/**
		* @brief the zone used to compute normals
		*
		* Note that if the normal zone is NULL, the emitter's zone is used.
		*
		* @param zone : the zone used to compute normals (NULL to used the emitter's zone)
		*/
		void setNormalZone(Zone* zone);

		/**
		* @brief Gets the normal zone of this normalEmitter
		* @return the normal zone of this normalEmitter
		*/
		inline Zone* getNormalZone() const;

		/**
		* @brief Sets whether normals are inverted or not
		* @param inverted true to use inverted normals, false not to
		*/
		inline void setInverted(bool inverted);

		/**
		* @brief Tells whether normals are inverted for this normalEmitter
		* @return true if normals are inverted, false if not
		*/
		inline bool isInverted() const;

		/////////////
		// Herited //
		/////////////

		virtual Nameable* findByName(const std::string& name);

	protected :

		virtual void propagateUpdateTransform();

	private :
	
		bool inverted;
		Zone* normalZone;

		NormalEmitter(
			Zone* zone = NULL,
			bool full = true,
			int tank = -1,
			float flow = 1.0f,
			float forceMin = 1.0f,
			float forceMax = 1.0f,
			Zone* normalZone = NULL,
			bool inverted = false);

		NormalEmitter(const NormalEmitter& emitter);

		virtual void generateVelocity(Particle& particle,float speed) const;
	};

	inline NormalEmitter* NormalEmitter::create(
		Zone* zone,
		bool full,
		int tank,
		float flow,
		float forceMin,
		float forceMax,
		Zone* normalZone,
		bool inverted)
	{
		return new NormalEmitter(
			zone,
			full,
			tank,
			flow,
			forceMin,
			forceMax,
			normalZone,
			inverted);
	}

	inline Zone* NormalEmitter::getNormalZone() const
	{
		return normalZone;
	}

	inline void NormalEmitter::setInverted(bool inverted)
	{
		this->inverted = inverted;
	}

	inline bool NormalEmitter::isInverted() const
	{
		return inverted;
	}
}

#endif
