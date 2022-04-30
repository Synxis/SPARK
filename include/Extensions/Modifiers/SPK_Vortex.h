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

#ifndef H_SPK_VORTEX
#define H_SPK_VORTEX

namespace SPK
{
	/**
	* @brief A modifier defining a vortex in the universe
	*
	* A vortex is a spinning flow around a center. Its center (its eye) is defined as a line in the universe.<br>
	* In addition to its center line, a vortex is defined by 2 speeds :
	* <ul>
	* <li>the rotation speed which defines the speed of rotation around the vortex eye.</li>
	* <li>the attraction speed which defined the speeds at which a particle is attracted by the eye (or repeled).</li>
	* </ul>
	* The rotation speed can be defined either in units per second or in radians per seconds. In the first case,
	* the closer to the eye particles are, the faster they will spin around. In the second case, the speed is constant.<br>
	* <br>
	* The attraction speed can be either constant or linear (function of the distance to the eye). In the second case,
	* the farther particles are from the eye, the faster the are attracted (or repeled). First case can be assimilated
	* to a Archimedes' spiral while second is more a logarithmic spiral (Bernoulli's).
	* <br>
	* The vertex eye radius can be defined. The user has also the possibility to tell the vortex to destroy particles 
	* that enter its eye automatically.<br>
	* <br>
	* Note that this modifier does not update particle's velocity. Therefore particles modified by a vortex does not have any inertia.<br>
	* This is because defining an accurate vortex in a discrete environment cannot be performed by using the derivatives of the position and velocity.
	*/
	class SPK_PREFIX Vortex : public Modifier
	{
	public :

		/**
		* @brief Creates a new vortex
		* @param position : the position of the eye
		* @param direction : the direction of the eye
		* @param rotationSpeed : the speed of rotation
		* @param attractionSpeed : the speed of attraction
		* @return a new vortex
		*/
		static  Ref<Vortex> create(const Vector3D& position = Vector3D(),const Vector3D& direction = Vector3D(0.0f,1.0f,0.0f),float rotationSpeed = 1.0f,float attractionSpeed = 0.0f);

		////////////////////
		// Eue definition //
		////////////////////

		/**
		* @brief Sets the position of the eye of the vortex
		* 
		* The eye of the vortex is an infinite line defined by a position and a direction.
		*
		* @param position : the position of the eye of the vortex
		*/
		inline void setPosition(const Vector3D& position);

		/**
		* @brief Gets the position of the eye
		* @return the position of the eye
		*/
		inline const Vector3D& getPosition() const;

		/**
		* @brief Gets the transformed position of the eye
		* @return the transformed position of the eye
		*/
		const Vector3D& getTransformedPosition() const;

		/**
		* @brief Sets the direction of the eye of the vortex
		*
		* The eye of the vortex is an infinite line defined by a position and a direction.<br>
		* Note that the direction is normalized internally.
		*
		* @param dir : the direction of the eye of the vortex
		*/
		inline void setDirection(const Vector3D& dir);

		/**
		* @brief Gets the direction of the eye
		* @return the direction of the eye (normalized)
		*/
		inline const Vector3D& getDirection() const;

		/**
		* @brief Gets the transformed direction of the eye
		* @return the transformed direction of the eye (normalized)
		*/
		const Vector3D& getTransformedDirection() const;

		/**
		* @brief Sets the eye radius
		*
		* Note that an negative radius is inverted internally
		*
		* @param eyeRadius : the eye radius
		*/
		void setEyeRadius(float eyeRadius);

		/**
		* @brief Returns the eye radius
		* @return the eye radius
		*/
		inline float getEyeRadius() const;

		////////////
		// Speeds //
		////////////

		/**
		* @brief Sets the rotation speed of the vortex
		*
		* The rotation speed can either be in units per second or in radians per second (angular).<br>
		* In the case the rotation speed is angular, all particles are rotated around the eye at the same speed.<br>
		* In the other case, the closer particles are to the eye, the faster they rotate.<br>
		* <br>
		* The sens of rotation is defined by the sens of the speed (and depends also on whether we are in a right or left handed system).<br>
		* A speed of 0 means no rotation.
		*
		* @param rotationSpeed : the speed of rotation (either in units per unit of time or in radians per unit of time dependent on the second parameter)
		* @param angular :  true to have the rotation in radians per unit of time, false to have it in unit per unit of time.
		*/
		inline void setRotationSpeed(float rotationSpeed, bool angular);
		inline void setRotationSpeed(float rotationSpeed);
		inline void setRotationSpeedAngular(bool a);

		/**
		* @brief Gets the rotation speed
		* @return the rotation speed
		*/
		inline float getRotationSpeed() const;

		/**
		* @brief Tells whether rotation speed is angular
		* @return true if rotation speed is angular, false if not
		*/
		inline bool isRotationSpeedAngular() const;

		/**
		* @brief Sets the attraction speed of the vortex
		*
		* The attraction speed is the speed that move particles towards the eye of the vortex.<br>
		* It can either be constant or linear (function of the distance of the particle from the eye).<br>
		* <br>
		* A constant attraction speed is defined in units per unit of time,
		* a linear attraction speed is also defined in units per unit of time and the value is the speed at a distance of 1.0f from the eye.<br>
		* <br>
		* A negative speed means particles are repelled by the eye.<br>
		* A speed of 0.0f means particles are neither attracted nor repelled by the eye of the vortex.
		*
		* @param attractionSpeed : the attraction speed of the vortex in units per unit of time
		* @param linear : true to set the speed function of the distance from the eye, false to set it constant
		*/
		void setAttractionSpeed(float attractionSpeed, bool linear);
		inline void setAttractionSpeed(float attractionSpeed);
		inline void setAttractionSpeedLinear(bool l);

		/**
		* @brief Gets the attraction speed
		* @return the attraction speed
		*/
		inline float getAttractionSpeed() const;

		/**
		* @brief Tells whether attraction speed is function of the distance from the eye
		* @return true if attraction speed is linear, false if it is constant
		*/
		inline bool isAttractionSpeedLinear() const;

		///////////////////////
		// Killing particles //
		///////////////////////

		/**
		* @brief Tells whether particles is the eye must be killed or not
		* @param kill : true to kill particles in the eye, false not to
		*/
		inline void enableParticleKilling(bool kill);

		/**
		* @brief Tells whether particles are killed when in the eye
		* @return true if particles are killed in the eye, false if not
		*/
		inline bool isParticleKillingEnabled() const;

	public :
		spark_description(Vortex, Modifier)
		(
			spk_attribute(Vector3D, position, setPosition, getPosition);
			spk_attribute(Vector3D, direction, setDirection, getDirection);
			spk_attribute(float, rotationSpeed, setRotationSpeed, getRotationSpeed);
			spk_attribute(float, attractionSpeed, setAttractionSpeed, getAttractionSpeed);
			spk_attribute(bool, angularRotationSpeed, setRotationSpeedAngular, isRotationSpeedAngular);
			spk_attribute(bool, linearAttractionSpeed, setAttractionSpeedLinear, isAttractionSpeedLinear);
			spk_attribute(float, eyeRadius, setEyeRadius, getEyeRadius);
			spk_attribute(bool, killParticles, enableParticleKilling, isParticleKillingEnabled);
		);

	protected :

		virtual void innerUpdateTransform();

	private :

		Vector3D position;
		Vector3D direction;

		Vector3D tPosition;
		Vector3D tDirection;

		float rotationSpeed;
		float attractionSpeed;

		bool angularSpeedEnabled;
		bool linearSpeedEnabled;

		float eyeRadius;
		bool killingParticleEnabled;

		Vortex(const Vector3D& position = Vector3D(),const Vector3D& direction = Vector3D(0.0f,1.0f,0.0f),float rotationSpeed = 1.0f,float attractionSpeed = 0.0f);
		Vortex(const Vortex& vortex);

		virtual void modify(Group& group,DataSet* dataSet,float deltaTime) const;
	};

	inline Ref<Vortex> Vortex::create(const Vector3D& position,const Vector3D& direction,float rotationSpeed,float attractionSpeed)
	{
		return SPK_NEW(Vortex,position,direction,rotationSpeed,attractionSpeed);
	}

	inline void Vortex::setPosition(const Vector3D& position)
	{
		this->position = position;
		transformPos(tPosition,position);
	}

	inline const Vector3D& Vortex::getPosition() const
	{
		return position;
	}

	inline const Vector3D& Vortex::getTransformedPosition() const
	{
		return tPosition;
	}

	inline void Vortex::setDirection(const Vector3D& dir)
	{
		direction = dir;
		direction.normalize();
		transformDir(tDirection,direction);
		tDirection.normalize();
	}

	inline const Vector3D& Vortex::getDirection() const
	{
		return direction;
	}

	inline const Vector3D& Vortex::getTransformedDirection() const
	{
		return tDirection;
	}

	inline float Vortex::getEyeRadius() const
	{
		return eyeRadius;
	}

	inline void Vortex::setRotationSpeedAngular(bool a)
	{
		angularSpeedEnabled = a;
	}

	inline void Vortex::setRotationSpeed(float r, bool angular)
	{
		rotationSpeed = r;
		angularSpeedEnabled = angular;
	}

	inline void Vortex::setRotationSpeed(float r)
	{
		rotationSpeed = r;
	}

	inline float Vortex::getRotationSpeed() const
	{
		return rotationSpeed;
	}

	inline bool Vortex::isRotationSpeedAngular() const
	{
		return angularSpeedEnabled;
	}

	inline void Vortex::setAttractionSpeedLinear(bool l)
	{
		linearSpeedEnabled = l;
	}

	inline void Vortex::setAttractionSpeed(float attractionSpeed,bool linear)
	{
		this->attractionSpeed = attractionSpeed;
		linearSpeedEnabled = linear;
	}

	inline void Vortex::setAttractionSpeed(float a)
	{
		attractionSpeed = a;
	}

	inline float Vortex::getAttractionSpeed() const
	{
		return attractionSpeed;
	}

	inline bool Vortex::isAttractionSpeedLinear() const
	{
		return linearSpeedEnabled;
	}

	inline void Vortex::enableParticleKilling(bool kill)
	{
		killingParticleEnabled = kill;
	}

	inline bool Vortex::isParticleKillingEnabled() const
	{
		return killingParticleEnabled;
	}
}

#endif
