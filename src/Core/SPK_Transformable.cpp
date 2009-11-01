//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
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


#include "Core/SPK_Transformable.h"
#include "Core/SPK_Zone.h"

namespace SPK
{
	const float Transformable::IDENTITY[] = 
	{
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f,
	};

	Transformable::Transformable(bool CHECK_UPDATED) :
		CHECK_UPDATED(CHECK_UPDATED),
		currentUpdate(0),
		lastUpdate(0),
		lastParentUpdate(0),
		parent(NULL)
	{
		std::memcpy(local,IDENTITY,sizeof(float) * TRANSFORM_LENGTH);
		std::memcpy(world,IDENTITY,sizeof(float) * TRANSFORM_LENGTH);
	}

	void Transformable::setTransformNC(const float* transform)
	{
		for (size_t i = 0; i < TRANSFORM_LENGTH; ++i)
			local[i] = transform[(i >> 2) + ((i & 3) << 2)];	// conversion from column major to row major matrix
		notifyForUpdate();
	}

	void Transformable::updateTransform(const Transformable* parent)
	{
		if (!CHECK_UPDATED ||							// The transformable updates even if its transform has not been updated
			isUpdateNotified() ||						// the local transform or instance param have been updated
			parent != this->parent ||					// the parent has changed
			(parent != NULL &&
			lastParentUpdate != parent->currentUpdate))	// the parent transform has been modified
		{
			this->parent = parent;

			if (parent == NULL)
				memcpy(world,local,sizeof(float) * TRANSFORM_LENGTH);
			else
			{
				multiply(world,parent->world,local);
				lastParentUpdate = parent->currentUpdate;
			}

			lastUpdate = currentUpdate;
			innerUpdateTransform();	;
		}
	}

	void Transformable::transformPos(Vector3D& tPos,const Vector3D& pos)
	{
		multiply(tPos,pos,world);
	}

	void Transformable::transformDir(Vector3D& tDir,const Vector3D& dir,const Zone* zone)
	{
		Vector3D tmp(dir);

		if (zone != NULL)
		{
			tmp += zone->getPosition();
			multiply(tDir,tmp,world);	
			tDir -= zone->getTransformedPosition();
		}
		else
		{
			Vector3D tPos;
			multiply(tPos,Vector3D(),world);
			multiply(tDir,tmp,world);
			tDir -= tPos;
		}
	}
}
