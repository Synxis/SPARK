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

#ifndef H_SPK_OBSTACLE
#define H_SPK_OBSTACLE

namespace SPK
{
	class SPK_PREFIX Obstacle : public ZonedModifier
	{
	public :

		static  Ref<Obstacle> create(
			const Ref<Zone>& zone = SPK_NULL_REF,
			float bouncingRatio = 1.0f,
			float friction = 1.0f,
			ZoneTest zoneTest = ZONE_TEST_INTERSECT);

		////////////////////
		// Bouncing ratio //
		////////////////////
		
		/**
		* @brief Sets the bouncing ratio of this obstacle
		*
		* The bouncing ratio is the multiplier applied to the normal component of the rebound.
		*
		* @param bouncingRatio : the bouncing ratio of this obstacle
		*/
		inline void setBouncingRatio(float bouncingRatio);

			/**
		* @brief Gets the bouncing ratio of this obstacle
		* @return the bouncing ratio of this obstacle
		*/
		inline float getBouncingRatio() const;

		//////////////
		// Friction //
		//////////////

		/**
		* @brief Sets the friction of this obstacle
		*
		* The bouncing ratio is the multiplier applied to the tangent component of the rebound.
		*
		* @param friction : the friction of this obstacle
		*/
		inline void setFriction(float friction);

		/**
		* @brief Gets the friction of this obstacle
		* @return the friction of this obstacle
		*/
		inline float getFriction() const;

	public :
		spark_description(Obstacle, ZonedModifier)
		(
			spk_attribute(float, bouncingRatio, setBouncingRatio, getBouncingRatio);
			spk_attribute(float, friction, setFriction, getFriction);
		);

	private :

		float bouncingRatio;
		float friction;

		Obstacle(
			const Ref<Zone>& zone = SPK_NULL_REF,
			float bouncingRatio = 1.0f,
			float friction = 1.0f,
			ZoneTest zoneTest = ZONE_TEST_INTERSECT);

		Obstacle(const Obstacle& obstacle);

		virtual void init(Particle& particle,DataSet* dataSet) const;
		virtual void modify(Group& group,DataSet* dataSet,float deltaTime) const;
	};

	inline Ref<Obstacle> Obstacle::create(const Ref<Zone>& zone,float bouncingRatio,float friction,ZoneTest zoneTest)
	{
		return SPK_NEW(Obstacle,zone,bouncingRatio,friction,zoneTest);
	}

	inline void Obstacle::setBouncingRatio(float bouncingRatio)
	{
		this->bouncingRatio = bouncingRatio;
	}

	inline float Obstacle::getBouncingRatio() const
	{
		return bouncingRatio;
	}

	inline void Obstacle::setFriction(float friction)
	{
		this->friction = friction;
	}

	inline float Obstacle::getFriction() const
	{
		return friction;
	}
}

#endif
