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


#ifndef H_SPK_ZONEINTERSECTION
#define H_SPK_ZONEINTERSECTION

#include "Core/SPK_Zone.h"
#include <vector>

namespace SPK
{
	/**
	* @class ZoneIntersection
	* @brief A zone allowing to use several zones easily
	*
	* A ZoneIntersection is defined by a list of zones and its position. The position is the isobarycenter
	* of all centers. A point is considered to be be inside this zone if it's in all
	* zones in the list.
	*/
	class SPK_PREFIX ZoneIntersection : public Zone
	{
		SPK_IMPLEMENT_REGISTERABLE(ZoneIntersection)

	public :

		/////////////////
		// Constructor //
		/////////////////

		/**
		* @brief Constructor of ZoneIntersection
		* @param amountEstimation : an estimation of the number of zones
		* which will be included in this ZoneIntersection (used for an optimisation of the vector)
		*/
		ZoneIntersection(size_t amountEstimation);

		/**
		* @brief Creates and registers a new ZoneIntersection
		* @param amountEstimation : an estimation of the number of zones
		* which will be included in this ZoneIntersection (used for an optimisation of the vector)
		*/
		static inline ZoneIntersection* create(size_t amountEstimation);

		////////////
		// Setter //
		////////////

		/**
		* @brief Sets the position of this ZoneIntersection
		* All zone inside this one are moved. If there are no zones, nothing will happen.
		* @param p : the new position of this ZoneIntersection
		*/
		virtual void setPosition(const Vector3D& p);

		/**
		* @brief Add a zone to this ZoneIntersection
		* @param zone : the zone to add
		* @param check : check if the zone is already included
		*/
		virtual void addZone(Zone* zone, bool check = true);

		/**
		* @brief Remove a zone from this ZoneIntersection
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
		* @brief Gets whether the specified zone is included in this ZoneIntersection
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

		/**
		* @brief Moves a point at the border of the ZoneIntersection
		* @param point : the point that will be moved to the border of the ZoneIntersection
		* @param inside : ignored, the point will be always inside (if possible)
		*/
		virtual void moveAtBorder(Vector3D& v,bool inside) const;
		virtual Vector3D computeNormal(const Vector3D& point) const;

	private :

		std::vector<Zone*> Zones;
	};


	inline ZoneIntersection* ZoneIntersection::create(size_t amountEstimation)
	{
		ZoneIntersection* obj = new ZoneIntersection(amountEstimation);
		registerObject(obj);
		return obj;
	}
}

#endif
