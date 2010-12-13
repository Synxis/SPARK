//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2009															//
// Julien Fryer - julienfryer@gmail.com											//
// Thibault Lescoat -  info-tibo <at> orange <dot> fr							//
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


#ifndef H_SPK_ZONEUNION
#define H_SPK_ZONEUNION

#include "Core/SPK_Zone.h"
#include <vector>

namespace SPK
{
	/**
	* @class ZoneUnion
	* @brief A zone allowing to use several zones easily
	*
	* A ZoneUnion is defined by a list of zones and its position. The position is the isobarycenter
	* of all centers. A point is considered to be be inside this zone if it's in at least one of all
	* zones in the list.
	*/
	class SPK_PREFIX ZoneUnion : public Zone
	{
		SPK_IMPLEMENT_REGISTERABLE(ZoneUnion)

	public :

		/////////////////
		// Constructor //
		/////////////////

		/**
		* @brief Constructor of ZoneUnion
		* @param amountEstimation : an estimation of the number of zones
		* which will be included in this ZoneUnion (used for an optimisation of the vector)
		*/
		ZoneUnion(size_t amountEstimation);

		/**
		* @brief Creates and registers a new ZoneUnion
		* @param amountEstimation : an estimation of the number of zones
		* which will be included in this ZoneUnion (used for an optimisation of the vector)
		*/
		static ZoneUnion* create(size_t amountEstimation);

		////////////
		// Setter //
		////////////

		/**
		* @brief Sets the position of this ZoneUnion
		* All zone inside this one are moved. If there are no zones, nothing will happen.
		* @param p : the new position of this ZoneUnion
		*/
		virtual void setPosition(const Vector3D& p);

		/**
		* @brief Add a zone to this ZoneUnion
		* @param zone : the zone to add
		* @param check : check if the zone is already included
		*/
		virtual void addZone(Zone* zone, bool check = true);

		/**
		* @brief Remove a zone from this ZoneUnion
		* @param zone : the zone to remove
		*/
		virtual void removeZone(Zone* zone);

		////////////
		// Getter //
		////////////

		/**
		* @brief Gets the number of zones inside this one
		* @return amount of zones
		*/
		virtual size_t getNumberZones();

		/**
		* @brief Gets whether the specified zone is included in this ZoneUnion
		* @return true if included, else false
		*/
		virtual bool isZoneIncluded(Zone* z);

		/**
		* @brief Gets the list
		* @return the internal std::list used to store zones
		*/
		virtual std::vector<Zone*>& getZones();

		///////////////
		// Interface //
		///////////////

		virtual void generatePosition(Particle& particle,bool full) const;
		virtual bool contains(const Vector3D& v) const;
		virtual bool intersects(const Vector3D& v0,const Vector3D& v1,Vector3D* intersection,Vector3D* normal) const;
		virtual void moveAtBorder(Vector3D& v,bool inside) const;
		virtual Vector3D computeNormal(const Vector3D& point) const;

	private :

		std::vector<Zone*> Zones;
	};


	inline ZoneUnion* ZoneUnion::create(size_t amountEstimation)
	{
		ZoneUnion* obj = new ZoneUnion(amountEstimation);
		registerObject(obj);
		return obj;
	}
}

#endif
