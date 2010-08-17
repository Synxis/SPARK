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

#ifndef H_SPK_SPHERE
#define H_SPK_SPHERE

#include "Core/SPK_Zone.h"

namespace SPK
{
	class SPK_PREFIX Sphere : public Zone
	{
	SPK_IMPLEMENT_REGISTERABLE(Sphere);

	public :

		static inline Sphere* create(const Vector3D& position = Vector3D(),float radius = 1.0f);

		////////////
		// Radius //
		////////////

		void setRadius(float radius);
		inline float getRadius() const;

		///////////////
		// Interface //
		///////////////

		virtual void generatePosition(Vector3D& v,bool full,float radius = 0.0f) const;
		virtual bool contains(const Vector3D& v,float radius = 0.0f) const;
		virtual bool intersects(const Vector3D& v0,const Vector3D& v1,float radius = 0.0f) const;
		virtual Vector3D computeNormal(const Vector3D& v) const;

	private :

		float radius;

		inline Sphere(const Vector3D& position = Vector3D(),float radius = 1.0f);
	};

	inline Sphere::Sphere(const Vector3D& position,float radius) :
		Zone(position),
		radius(radius)
	{}

	inline Sphere* Sphere::create(const Vector3D& position,float radius)
	{
		return SPK_NEW(Sphere,position,radius);
	}

	inline float Sphere::getRadius() const
	{
		return radius;
	}
}

#endif
