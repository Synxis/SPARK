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

#ifndef H_SPK_ZONE
#define H_SPK_ZONE

#include "Core/SPK_DEF.h"
#include "Core/SPK_Vector3D.h"
#include "Core/SPK_Registerable.h"
#include "Core/SPK_Transformable.h"

namespace SPK
{
	class Zone : public Registerable, 
				 public Transformable
	{
	public :

		////////////////
		// Destructor //
		////////////////

		virtual inline ~Zone() {}

		//////////////
		// Position //
		//////////////

		inline void setPosition(const Vector3D& position);
		inline const Vector3D& getPosition() const;
		inline const Vector3D& getTransformedPosition() const;

		///////////////
		// Interface //
		///////////////

		virtual void generatePosition(Vector3D& v,bool full,float radius = 0.0f) const = 0;
		virtual bool contains(const Vector3D& v,float radius = 0.0f) const = 0;
		virtual bool intersects(const Vector3D& v0,const Vector3D& v1,float radius = 0.0f) const = 0;
		virtual Vector3D computeNormal(const Vector3D& v) const = 0;
		//virtual void moveAtBorder(Vector3D& v,bool inside) const = 0;

	protected :

		inline Zone(const Vector3D& position = Vector3D());

		virtual inline void innerUpdateTransform();
		static inline void normalizeOrRandomize(Vector3D& v);

	private :

		Vector3D position;
		Vector3D tPosition;
	};

	inline Zone::Zone(const Vector3D& position) :
		Registerable(),
		Transformable(),
		position(position),
		tPosition(position)
	{}

	inline void Zone::setPosition(const Vector3D& v)
	{
		position = tPosition = v;
		notifyForTransformUpdate();
	}

	inline const Vector3D& Zone::getPosition() const
	{
		return position;
	}

	inline const Vector3D& Zone::getTransformedPosition() const
	{
		return tPosition;
	}

	inline void Zone::innerUpdateTransform()
	{
		transformPos(tPosition,position);
	}

	inline void Zone::normalizeOrRandomize(Vector3D& v)
	{
		while(!v.normalize())
		{
			do v = SPK_RANDOM(Vector3D(-1.0f,-1.0f,-1.0f),Vector3D(1.0f,1.0f,1.0f));
			while (v.getSqrNorm() > 1.0f);
		}
	}
}

#endif
