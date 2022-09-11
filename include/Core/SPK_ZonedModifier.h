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

#ifndef H_SPK_ZONEDMODIFIER
#define H_SPK_ZONEDMODIFIER

namespace SPK
{
	/** Constants defining the zone tests to enabled for a given ZonedModifier */
	enum ZoneTestFlag
	{
		ZONE_TEST_FLAG_INSIDE = 1 << ZONE_TEST_INSIDE,			/**< Enables the zone test inside */
		ZONE_TEST_FLAG_OUTSIDE = 1 << ZONE_TEST_OUTSIDE,		/**< Enables the zone test outside */
		ZONE_TEST_FLAG_INTERSECT = 1 << ZONE_TEST_INTERSECT,	/**< Enables the zone test intersect */
		ZONE_TEST_FLAG_ENTER = 1 << ZONE_TEST_ENTER,			/**< Enables the zone test enter */
		ZONE_TEST_FLAG_LEAVE = 1 << ZONE_TEST_LEAVE,			/**< Enables the zone test leave */
		ZONE_TEST_FLAG_ALWAYS = 1 << ZONE_TEST_ALWAYS			/**< The zone is ignored and the test is always passed */
	};

	/** @brief An abstract modifier with a zone attached to it */
	class SPK_PREFIX ZonedModifier : public Modifier
	{
	public :

		/** Destructor of zonedModifier */
		virtual ~ZonedModifier();

		//////////
		// Zone //
		//////////

		/**
		* @brief Sets both the zone and the zone test
		* @param zone : the zone
		* @param zoneTest : the zone test
		*/
		inline void setZone(const Ref<Zone>& zone, ZoneTest zoneTest);

		/**
		* @brief Sets the zone
		*
		* Note that if the zone is NULL, the default zone is used.
		*
		* @param zone : the zone
		*/
		void setZone(const Ref<Zone>& zone);

		/**
		* @brief Gets the zone
		* @return the zone
		*/
		inline const Ref<Zone>& getZone() const;

		/**
		* @brief Sets the zone test
		*
		* If the zone test is not valid for this zonedModifier,
		* the first valid zone test is used.
		*
		* @param zoneTest : the zone test
		*/
		void setZoneTest(ZoneTest zoneTest);

		/**
		* @brief Gets the zone test
		* @return the zone test
		*/
		inline ZoneTest getZoneTest() const;

		///////////////////////
		// Virtual interface //
		///////////////////////

		virtual Ref<SPKObject> findByName(const std::string& name);

	public :
		spark_description(ZonedModifier, Modifier)
		(
			spk_attribute(Ref<Zone>, zone, setZone, getZone);
			spk_attribute(ZoneTest, zoneTest, setZoneTest, getZoneTest);
		);

	protected :

		static const unsigned int ZONE_TEST_FLAG_ALL = 0xFFFFFFFF;	/**< Enables all zone tests */

		/**
		* @brief Constructor of zonedModifier
		* @param PRIORITY : see Modifier
		* @param NEEDS_DATASET : see Modifier
		* @param CALL_INIT : see Modifier
		* @param ZONE_TEST_FLAG : the test flag specifying which zone tests are valid for this zonedModifier
		* @param zoneTest : the zone test by default
		* @param zone : the zone
		*/
		ZonedModifier(
			unsigned int PRIORITY,
			bool NEEDS_DATASET,
			bool CALL_INIT,
			bool NEEDS_OCTREE,
			int ZONE_TEST_FLAG,
			ZoneTest zoneTest,
			const Ref<Zone>& zone = SPK_NULL_REF);

		ZonedModifier(const ZonedModifier& zonedModifier);

		/**
		* @brief Check whether the zone test passes for a given particle
		* @return true is the zone test passes, false if not
		*/
		inline bool checkZone(const Particle& particle,Vector3D* normal = NULL) const;

		virtual void propagateUpdateTransform();

	private :

		static const size_t NB_ZONE_TESTS = 6;
		const int ZONE_TEST_FLAG;

		Ref<Zone> zone;
		ZoneTest zoneTest;
	};

	inline void ZonedModifier::setZone(const Ref<Zone>& zone, ZoneTest zoneTest)
	{
		setZone(zone);
		setZoneTest(zoneTest);
	}

	inline const Ref<Zone>& ZonedModifier::getZone() const
	{
		return zone;
	}

	inline ZoneTest ZonedModifier::getZoneTest() const
	{
		return zoneTest;
	}

	inline bool ZonedModifier::checkZone(const Particle& particle,Vector3D* normal) const
	{
		return zone->check(particle,zoneTest,normal);
	}
}

#endif
