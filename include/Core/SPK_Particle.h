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

#ifndef H_SPK_PARTICLE
#define H_SPK_PARTICLE

#include <cmath> // for sqrt

#include "Core/SPK_DEF.h"
#include "Core/SPK_Group.h"

namespace SPK
{
	/**
	* @brief Single particle
	*/
	class Particle
	{
	friend class Group;

	template<typename T>
	friend class Iterator;
	template<typename T>
	friend class ConstIterator;

	public :

		/////////////
		// Setters //
		/////////////

		inline Vector3D& position();
		inline Vector3D& velocity();
		inline Vector3D& oldPosition();

		//inline void setEnergy(float energy);
		inline void setColor(Color color);
		inline void setParam(Param parameter,float value);
		inline void setParamNC(Param parameter,float value);

		/////////////
		// Getters //
		/////////////

		inline Group& getGroup();
		inline const Group& getGroup() const;
		inline size_t getIndex() const;

		inline const Vector3D& position() const;
		inline const Vector3D& velocity() const;
		inline const Vector3D& oldPosition() const;

		inline float getAge() const;
		inline float getEnergy() const;
		inline float getLifeTime() const;
		inline float getDistanceFromCamera() const;
		inline float getSqrDistanceFromCamera() const;
		inline Color getColor() const;
		inline float getParam(Param parameter) const;
		inline float getParamNC(Param parameter) const;

		inline bool isAlive() const;

		///////////////
		// Interface //
		///////////////

		inline void kill();

	private :

		inline Particle(Group& group,size_t index);

		Group& group;
		mutable size_t index;
	};

	inline Particle::Particle(Group& group,size_t index) :
		group(group),
		index(index)
	{}

	inline Vector3D& Particle::position()
	{
		return group.particleData.positions[index];
	}

	inline Vector3D& Particle::velocity()
	{
		return group.particleData.velocities[index];
	}

	inline Vector3D& Particle::oldPosition()
	{
		return group.particleData.oldPositions[index];
	}

	inline void Particle::setColor(Color color)
	{
		group.particleData.colors[index] = color;
	}

	inline void Particle::setParam(Param parameter,float value)
	{
		if (group.isEnabled(parameter))
			group.particleData.parameters[parameter][index] = value;
	}

	inline void Particle::setParamNC(Param parameter,float value)
	{
		group.particleData.parameters[parameter][index] = value;
	}

	inline const Group& Particle::getGroup() const
	{
		return group;
	}

	inline Group& Particle::getGroup()
	{
		return group;
	}

	inline size_t Particle::getIndex() const
	{
		return index;
	}

	inline const Vector3D& Particle::position() const
	{
		return group.particleData.positions[index];
	}

	inline const Vector3D& Particle::velocity() const
	{
		return group.particleData.velocities[index];
	}

	inline const Vector3D& Particle::oldPosition() const
	{
		return group.particleData.oldPositions[index];
	}

	inline float Particle::getAge() const
	{
		return group.particleData.ages[index];
	}

	inline float Particle::getEnergy() const
	{
		return group.particleData.energies[index];
	}

	inline float Particle::getLifeTime() const
	{
		return group.particleData.lifeTimes[index];
	}

	inline float Particle::getDistanceFromCamera() const
	{
		return sqrt(group.particleData.sqrDists[index]);
	}

	inline float Particle::getSqrDistanceFromCamera() const
	{
		return group.particleData.sqrDists[index];
	}

	inline Color Particle::getColor() const
	{
		return group.particleData.colors[index];
	}

	inline float Particle::getParam(Param parameter) const
	{
		if (group.isEnabled(parameter))
			return group.particleData.parameters[parameter][index];
		else
			return Group::DEFAULT_VALUES[parameter];
	}

	inline float Particle::getParamNC(Param parameter) const
	{
		return group.particleData.parameters[parameter][index];
	}

	inline bool Particle::isAlive() const
	{
		return group.particleData.energies[index] > 0.0f;
	}

	inline void Particle::kill()
	{
		group.particleData.energies[index]  = 0.0f;
	}
}

#endif
