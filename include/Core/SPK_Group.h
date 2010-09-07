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

#ifndef H_SPK_GROUP
#define H_SPK_GROUP

#include <cstring> // for memcpy
#include <vector>
#include <list>
#include <deque>

#include "Core/SPK_DEF.h"
#include "Core/SPK_Logger.h"
#include "Core/SPK_Nameable.h"
#include "Core/SPK_Vector3D.h"
#include "Core/SPK_Color.h"
#include "Core/SPK_Interpolator.h"
#include "Core/SPK_Modifier.h"
#include "Core/SPK_DataSet.h"

namespace SPK
{
	class Emitter;
	class Renderer;
	class Action;
	class DataSet;
	class RenderBuffer;
	class System;

	/**
	* @brief Group of particles
	*/
	class SPK_PREFIX Group : public Nameable
	{
	friend class Particle;
	friend class System;
	friend class DataSet;

	public :

		void setLifeTime(float minLife,float maxLife);
		inline void setImmortal(bool immortal);
		inline void setStill(bool still);

		void setColorInterpolator(ColorInterpolator* interpolator);
		ColorInterpolator* getColorInterpolator() const;

		void setParamInterpolator(Param param,FloatInterpolator* interpolator);
		FloatInterpolator* getParamInterpolator(Param param) const;

		inline float getMinLifeTime() const;
		inline float getMaxLifeTime() const;
		inline bool isImmortal() const;
		inline bool isStill() const;

		inline bool isEnabled(Param param) const;

		inline size_t getNbParticles() const;
		inline size_t getCapacity() const;

		Particle getParticle(size_t index);
		const Particle getParticle(size_t index) const;

		void reallocate(size_t capacity);
		inline void empty();

		void addEmitter(Emitter* emitter);
		void removeEmitter(Emitter* emitter);
		inline Emitter* getEmitter(size_t index) const;
		inline size_t getNbEmitters() const;

		void addModifier(Modifier* modifier);
		void removeModifier(Modifier* modifier);
		inline Modifier* getModifier(size_t index) const;
		inline size_t getNbModifiers() const;

		void setRenderer(Renderer* renderer);
		inline Renderer* getRenderer() const;

		inline void enableDistanceComputation(bool distanceComputation);
		inline bool isDistanceComputationEnabled() const;

		inline void enableSorting(bool sorting);
		inline bool isSortingEnabled() const;

		inline const void* getColorAddress() const;
		inline const void* getPositionAddress() const;
		inline const void* getVelocityAddress() const;
		inline const void* getParamAddress(Param param) const;

		inline void setRadius(float radius);
		void setGraphicalRadius(float radius);
		void setPhysicalRadius(float radius);
		inline float getGraphicalRadius() const;
		inline float getPhysicalRadius() const;

		/**
		* @brief Gets a Vector3D holding the minimum coordinates of the AABB of the Group.
		*
		* Note that this method is only useful when the AABB computation of the system holding the Group is enabled.
		*
		* @return a Vector3D holding the minimum coordinates of the AABB of the Group
		*/
		inline const Vector3D& getAABBMin() const;

		/**
		* @brief Gets a Vector3D holding the maximum coordinates of the AABB of the Group.
		*
		* Note that this method is only useful when the AABB computation of the system holding the Group is enabled.
		*
		* @return a Vector3D holding the maximum coordinates of the AABB of the Group
		*/
		inline const Vector3D& getAABBMax() const;

		///////////////////
		// Add Particles //
		///////////////////

		/**
		* @brief Adds some Particles to this Group
		*
		* This method and all the methods of type addParticles will add a given number of Particles at the given position with the given velocity.<br>
		* Note that even if a Zone and an Emitter is passed, the position and the velocity will be the same for all Particles.<br>
		* <br>
		* In case a Zone is passed, Zone::generatePosition(Particle,bool) is used to generate the position.<br>
		* In case an Emitter is passed, Emitter::generateVelocity(Particle) with a mass of 1 is used to generate the velocity.
		* The velocity will then be updated with the Particle's mass when the Particle will be generated.<br>
		* In case a delta time is passed instead of a fixed number, the number will be computed thanks to the flow of the Emitter passed.<br>
		* <br>
		* Particles will be added to the Group at the next call to update(unsigned int) or flushAddedParticles().<br>
		* This is why any Emitter and Zone used to generate particles must be valid at the next call of either update(unsigned int) or flushAddedParticles().<br>
		* <br>
		* On some methods addParticles, the full variable is present. This variable defines where to generate positions in a Zone :
		* <ul>
		* <li>true to generate position somewhere within the whole Zone.</li>
		* <li>false to generate position somewhere on the Zone border.</li>
		* </ul>
		*
		* @param nb : the number of Particles to add
		* @param position : the position where the Particles will be added
		* @param velocity : the velocity of the Particles
		*/
		inline void addParticles(unsigned int nb,const Vector3D& position,const Vector3D& velocity);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param nb : the number of Particles to add
		* @param zone : the Zone that will be used to generate the position
		* @param emitter : the Emitter that will be used to generate the velocity
		* @param full : true to generate a position within the whole Zonz, false only at its borders
		*/
		void addParticles(unsigned int nb,Zone* zone,Emitter* emitter,bool full = true);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param nb : the number of Particles to add
		* @param zone : the Zone that will be used to generate the position
		* @param velocity : the velocity of the Particles
		* @param full : true to generate a position within the whole Zonz, false only at its borders
		*/
		void addParticles(unsigned int nb,Zone* zone,const Vector3D& velocity,bool full = true);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param nb : the number of Particles to add
		* @param position : the position where the Particles will be added
		* @param emitter : the Emitter that will be used to generate the velocity
		*/
		void addParticles(unsigned int nb,const Vector3D& position,Emitter* emitter);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param nb : the number of Particles to add
		* @param emitter : the Emitter that will be used to generate the velocity and whose Zone will be used to generate the position
		*/
		void addParticles(unsigned int nb,Emitter* emitter);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param zone : the Zone that will be used to generate the position
		* @param emitter : the Emitter that will be used to generate the velocity
		* @param deltaTime : the step time that will be used to determine how many particles to generate
		* @param full : true to generate a position within the whole Zonz, false only at its borders
		*/
		void addParticles(Zone* zone,Emitter* emitter,float deltaTime,bool full = true);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param position : the position where the Particles will be added
		* @param emitter : the Emitter that will be used to generate the velocity
		* @param deltaTime : the step time that will be used to determine how many particles to generate
		*/
		void addParticles(const Vector3D& position,Emitter* emitter,float deltaTime);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for a complete description.
		*
		* @param emitter : the Emitter that will be used to generate the velocity and whose Zone will be used to generate the position
		* @param deltaTime : the step time that will be used to determine how many particles to generate
		*/
		void addParticles(Emitter* emitter,float deltaTime);

		/**
		* @brief Adds some Particles to this Group
		*
		* This method is slightly different to other addParticles methods as Particles are generated not at a point but on a line defined by start and end.<br>
		* The step is the distance between each Particle on the line and the offset is the starting distance to compute the first Particle's position.<br>
		* The offset is useful to generate equidistant particles on several lines. the returned offset has to be used as the passed offset for the next line.<br>
		* <br>
		* This method is useful to generate equidistant particles on lines no matter the framerate.<br>
		* <br>
		* See addParticles(unsigned int,const Vector3D&,const Vector3D&) for some complementary information.
		*
		* @param start : the position of the start of the line
		* @param end : the position of the end of the line
		* @param emitter : the Emitter that will be used to generate the velocity
		* @param step : the distance between each generated Particle
		* @param offset : the starting distance of the beginning of the line
		* @return the new offset at the end of the line
		*/
		float addParticles(const Vector3D& start,const Vector3D& end,Emitter* emitter,float step,float offset = 0.0f);

		/**
		* @brief Adds some Particles to this Group
		*
		* See addParticles(const Vector3D&,const Vector3D&,const Emitter*,float,float) for a complete description.
		*
		* @param start : the position of the start of the line
		* @param end : the position of the end of the line
		* @param velocity : the velocity of the Particles
		* @param step : the distance between each generated Particle
		* @param offset : the starting distance of the beginning of the line
		* @return the new offset at the end of the line
		*/
		float addParticles(const Vector3D& start,const Vector3D& end,const Vector3D& velocity,float step,float offset = 0.0f);

		void flushBufferedParticles();

		inline System& getSystem() const;

		void setBirthAction(Action* action);
		inline Action* getBirthAction() const;

		void setDeathAction(Action* action);
		inline Action* getDeathAction() const;

		void destroyRenderBuffer();
		void destroyAllAdditionnalData();

		DataSet* getModifierDataSet(Modifier* modifier);
		inline DataSet* getParamInterpolatorDataSet(Param param);
		inline DataSet* getColorInterpolatorDataSet();
		inline DataSet* getRendererDataSet();
		inline RenderBuffer* getRenderBuffer();

		virtual Nameable* findByName(const std::string& name);

	private :

		static const size_t NB_PARAMETERS = 5;
		static const float DEFAULT_VALUES[NB_PARAMETERS];

		// This holds the structure of arrays (SOA) containing data of particles
		struct ParticleData
		{
			bool initialized;

			size_t nbParticles;
			size_t maxParticles;

			// Particles attributes
			Vector3D* positions;
			Vector3D* velocities;
			Vector3D* oldPositions;

			float* ages;
			float* energies;
			float* lifeTimes;
			float* sqrDists;

			// Particles parameters
			Color* colors;
			float* parameters[NB_PARAMETERS];

			ParticleData() :
				initialized(false),
				nbParticles(0),
				maxParticles(0),
				positions(NULL),
				velocities(NULL),
				oldPositions(NULL),
				ages(NULL),
				energies(NULL),
				lifeTimes(NULL),
				sqrDists(NULL),
				colors(NULL)
			{
				for (size_t i = 0; i < NB_PARAMETERS; ++i)
					parameters[i] = NULL;
			}
		};

		struct EmitterPair
		{
			Emitter* obj;
			size_t nbBorn;

			EmitterPair(Emitter* obj,size_t nbBorn) :
				obj(obj),
				nbBorn(nbBorn)
			{}
		};

		template<class T>
		struct DataHandlerDef
		{
			T* obj;
			DataSet* dataSet;

			DataHandlerDef() :
				obj(NULL),
				dataSet(NULL)
			{}

			DataHandlerDef(T* obj,DataSet* dataSet) :
				obj(obj),
				dataSet(dataSet)
			{}
		};

		struct RendererDef
		{
			Renderer* obj;
			DataSet* dataSet;
			RenderBuffer* renderBuffer;

			RendererDef() :
				obj(NULL),
				dataSet(NULL),
				renderBuffer(NULL)
			{}
		};

		typedef DataHandlerDef<Modifier> ModifierDef;
		typedef DataHandlerDef<ColorInterpolator> ColorInterpolatorDef;
		typedef DataHandlerDef<FloatInterpolator> FloatInterpolatorDef;

		// Functor used to sort modifiers by priority
		struct CompareModifierPriority
		{
			inline bool operator()(const ModifierDef& modifier0, const ModifierDef& modifier1) const
			{
				return modifier0.obj->getPriority() < modifier1.obj->getPriority();
			}
		};

		struct CreationData
		{
			unsigned int nb;
			Vector3D position;
			Vector3D velocity;
			Zone* zone;
			Emitter* emitter;
			bool full;
		};

		System& system;

		ParticleData particleData;
		size_t enabledParamIndices[NB_PARAMETERS];
		size_t nbEnabledParameters;

		ColorInterpolatorDef colorInterpolator;
		FloatInterpolatorDef paramInterpolators[NB_PARAMETERS];

		std::vector<Emitter*> emitters;
		mutable std::vector<EmitterPair> activeEmitters;

		std::vector<ModifierDef> modifiers;
		std::vector<ModifierDef> sortedModifiers;
		mutable std::vector<ModifierDef> activeModifiers;
		mutable std::vector<ModifierDef> initModifiers;

		RendererDef renderer;

		Action* birthAction;
		Action* deathAction;

		std::list<DataSet> dataSets;

		float minLifeTime;
		float maxLifeTime;
		bool immortal;
		bool still;

		bool distanceComputationEnabled;
		bool sortingEnabled;

		Vector3D AABBMin;
		Vector3D AABBMax;

		float physicalRadius;
		float graphicalRadius;

		Group(System& system,size_t capacity);
		Group(System& system,const Group& group);
		Group(const Group& group); // never used
		~Group();

		bool updateParticles(float deltaTime);
		void renderParticles();

		void initParticle(size_t index,size_t& emitterIndex,size_t& nbManualBorn);
		void swapParticles(size_t index0,size_t index1);

		void recomputeEnabledParamIndices();

		template<typename T>
		void reallocateArray(T*& t,size_t newSize,size_t copySize);

		DataSet* attachDataSet(DataHandler* dataHandler);
		void detachDataSet(DataSet* dataHandler);

		void sortParticles(int start,int end);
		virtual void propagateUpdateTransform();

		void sortParticles();
		void computeAABB();

		void addParticles(unsigned int nb,const Vector3D& position,const Vector3D& velocity,Zone* zone,Emitter* emitter,bool full = false);
		void emptyBufferedParticles();

		// creation data
		std::deque<CreationData> creationBuffer;
		unsigned int nbBufferedParticles;

		void prepareAdditionnalData();

		void initData();
	};

	template<typename T>
	void Group::reallocateArray(T*& t,size_t newSize,size_t copySize)
	{
		T* oldT = t;
		t = SPK_NEW_ARRAY(T,newSize);
		if (oldT != NULL && copySize != 0)
			memcpy(oldT,t,copySize * sizeof(T));
		SPK_DELETE_ARRAY(oldT);
	}

	inline void Group::setImmortal(bool immortal)
	{
		this->immortal = immortal;
	}

	inline void Group::setStill(bool still)
	{
		this->still = still;
	}

	inline float Group::getMinLifeTime() const
	{
		return minLifeTime;
	}

	inline float Group::getMaxLifeTime() const
	{
		return maxLifeTime;
	}

	inline bool Group::isImmortal() const
	{
		return immortal;
	}

	inline bool Group::isStill() const
	{
		return still;
	}

	inline bool Group::isEnabled(Param param) const
	{
		return particleData.parameters[param] != NULL;
	}

	inline size_t Group::getNbParticles() const
	{
		return particleData.nbParticles;
	}

	inline size_t Group::getCapacity() const
	{
		return particleData.maxParticles;
	}

	inline void Group::empty()
	{
		particleData.nbParticles = 0;
	}

	inline Emitter* Group::getEmitter(size_t index) const
	{
		SPK_ASSERT(index < getNbEmitters(),"Group::getEmitter(size_t) - Index of emitter is out of bounds : " << index);
		return emitters[index];
	}

	inline size_t Group::getNbEmitters() const
	{
		return emitters.size();
	}

	inline Modifier* Group::getModifier(size_t index) const
	{
		SPK_ASSERT(index < getNbModifiers(),"Group::geModifier(size_t) - Index of modifier is out of bounds : " << index);
		return modifiers[index].obj;
	}

	inline size_t Group::getNbModifiers() const
	{
		return modifiers.size();
	}

	inline Renderer* Group::getRenderer() const
	{
		return renderer.obj;
	}

	inline void Group::enableDistanceComputation(bool distanceComputation)
	{
		distanceComputationEnabled = distanceComputation;
	}

	inline bool Group::isDistanceComputationEnabled() const
	{
		return distanceComputationEnabled;
	}

	inline void Group::enableSorting(bool sorting)
	{
		sortingEnabled = distanceComputationEnabled = sorting;
	}

	inline bool Group::isSortingEnabled() const
	{
		return sortingEnabled;
	}

	inline const void* Group::getColorAddress() const
	{
		return particleData.colors;
	}

	inline const void* Group::getPositionAddress() const
	{
		return particleData.positions;
	}

	inline const void* Group::getVelocityAddress() const
	{
		return particleData.velocities;
	}

	inline const void* Group::getParamAddress(Param param) const
	{
		return particleData.parameters[param];
	}

	inline const Vector3D& Group::getAABBMin() const
	{
		return AABBMin;
	}

	inline const Vector3D& Group::getAABBMax() const
	{
		return AABBMax;
	}

	inline void Group::setRadius(float radius)
	{
		setGraphicalRadius(radius);
		setPhysicalRadius(radius);
	}

	inline float Group::getGraphicalRadius() const
	{
		return graphicalRadius;
	}

	inline float Group::getPhysicalRadius() const
	{
		return physicalRadius;
	}

	inline void Group::addParticles(unsigned int nb,const Vector3D& position,const Vector3D& velocity)
	{
		addParticles(nb,position,velocity,NULL,NULL);
	}

	inline System& Group::getSystem() const
	{
		return system;
	}

	inline Action* Group::getBirthAction() const
	{
		return birthAction;
	}

	inline Action* Group::getDeathAction() const
	{
		return deathAction;
	}

	inline DataSet* Group::getParamInterpolatorDataSet(Param param)
	{
		return paramInterpolators[param].dataSet;
	}

	inline DataSet* Group::getColorInterpolatorDataSet()
	{
		return colorInterpolator.dataSet;
	}

	inline DataSet* Group::getRendererDataSet()
	{
		return renderer.dataSet;
	}

	inline RenderBuffer* Group::getRenderBuffer()
	{
		return renderer.renderBuffer;
	}
}

#endif
