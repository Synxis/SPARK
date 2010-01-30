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


#include "Extensions/Zones/SPK_ZoneIntersection.h"
#include "Core/SPK_Particle.h"

namespace SPK
{
	ZoneIntersection::ZoneIntersection(size_t amountEstimation)
		:Zone()
	{
		Zones.reserve(amountEstimation);
	}

	void ZoneIntersection::setPosition(const Vector3D& p)
	{
		Vector3D decal = p - getPosition();
		Vector3D pos; float nb = 1.0f/Zones.size();
		for(size_t t = 0; t < Zones.size(); t++)
		{
			Zones[t]->setPosition( Zones[t]->getPosition() + decal );
			pos += Zones[t]->getPosition() * nb;
		}
		Zone::setPosition(pos);
	}

	void ZoneIntersection::addZone(Zone* zone, bool check)
	{
		if(!zone) return;
		if(check && isZoneIncluded(zone)) return;

		Zones.push_back(zone);
		incrementChildReference(zone);
	}

	void ZoneIntersection::removeZone(Zone* zone)
	{
		bool found = false;
		for (size_t t = 0; t < Zones.size();)
		{
			if(Zones[t] == zone)
			{
				Zones.erase(Zones.begin()+t);
				found = true;
			}
			else
				t++;
		}
		if(found) decrementChildReference(zone);
	}

	size_t ZoneIntersection::getNumberZones()
	{
		return Zones.size();
	}

	bool ZoneIntersection::isZoneIncluded(Zone* z)
	{
		for (size_t t = 0; t < Zones.size(); t++)
			if(Zones[t] == z) return true;
		return false;
	}

	std::vector<Zone*>& ZoneIntersection::getZones()
	{
		return Zones;
	}

	void ZoneIntersection::generatePosition(Particle& particle,bool full) const
	{
		if(Zones.size() == 0) return;

		Zones[random((int)0,(int)Zones.size()-1)]->generatePosition(particle,full);
	}

	bool ZoneIntersection::contains(const Vector3D& v) const
	{
		for (size_t t = 0; t < Zones.size(); t++)
			if( !Zones[t]->contains(v) ) return false;
		return true;
	}

	bool ZoneIntersection::intersects(const Vector3D& v0,const Vector3D& v1,Vector3D* intersection,Vector3D* normal) const
	{
		for (size_t t = 0; t < Zones.size(); t++)
			if( !Zones[t]->intersects(v0,v1,intersection,normal) ) return false;
		return true;
	}

	void ZoneIntersection::moveAtBorder(Vector3D& v,bool inside) const
	{
		Vector3D point = v;
		for (size_t t = 0; t < Zones.size(); t++)
		{
			point = v;
			Zones[t]->moveAtBorder(point,true);
			if(contains(point))
			{
				v = point;
				return;
			}
		}

		if(Zones.size() != 0) Zones[0]->moveAtBorder(v,inside);
		else v = Vector3D();
	}

	Vector3D ZoneIntersection::computeNormal(const Vector3D& point) const
	{
		Vector3D v = point;
		
		for (size_t t = 0; t < Zones.size(); t++)
		{
			v = point;
			Zones[t]->moveAtBorder(v,false);
			if(contains(point))
				return Zones[t]->computeNormal(v);
		}

		if(Zones.size() != 0) return Zones[0]->computeNormal(point);
		else return Vector3D();
	}
}