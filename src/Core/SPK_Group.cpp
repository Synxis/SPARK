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

#include <algorithm> // for std::swap and std::sort
#include <limits> // for max float value

#include "Core/SPK_Group.h"
#include "Core/SPK_System.h"
#include "Core/SPK_Emitter.h"
#include "Core/SPK_Renderer.h"
#include "Core/SPK_Action.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_DataSet.h"
#include "Core/SPK_RenderBuffer.h"

namespace SPK
{
	const float Group::DEFAULT_VALUES[NB_PARAMETERS] =
	{
		1.0f,	// PARAM_SIZE
		1.0f,	// PARAM_MASS
		0.0f,	// PARAM_ANGLE
		0.0f,	// PARAM_TEXTURE_INDEX
		0.0f,	// PARAM_ROTATION_SPEED
	};

	Group::Group(System& system,size_t capacity) :
		system(system),
		nbEnabledParameters(0),
		minLifeTime(1.0f),
		maxLifeTime(1.0f),
		immortal(false),
		still(false),
		distanceComputationEnabled(false),
		sortingEnabled(false),
		AABBMin(),
		AABBMax(),
		radius(0.0f),
		nbBufferedParticles(0),
		birthAction(NULL),
		deathAction(NULL)
	{
		reallocate(capacity);
	}

	Group::Group(System& system,const Group& group) :
		system(system),
		nbEnabledParameters(0),
		minLifeTime(group.minLifeTime),
		maxLifeTime(group.maxLifeTime),
		immortal(group.immortal),
		still(group.still),
		distanceComputationEnabled(group.distanceComputationEnabled),
		sortingEnabled(group.sortingEnabled),
		AABBMin(group.AABBMin),
		AABBMax(group.AABBMax),
		radius(group.radius),
		nbBufferedParticles(0)
	{
		reallocate(group.getCapacity());

		renderer.obj = dynamic_cast<Renderer*>(Registerable::copyChild(group.renderer.obj));
		renderer.dataSet = attachDataSet(renderer.obj);

		setColorInterpolator(dynamic_cast<ColorInterpolator*>(Registerable::copyChild(group.colorInterpolator.obj,false)));
		for (size_t i = 0; i < NB_PARAMETERS; ++i)
			setParamInterpolator(static_cast<Param>(i),dynamic_cast<FloatInterpolator*>(Registerable::copyChild(group.paramInterpolators[i].obj,false)));

		for (std::vector<Emitter*>::const_iterator it = group.emitters.begin(); it != group.emitters.end(); ++it)
			emitters.push_back(dynamic_cast<Emitter*>(Registerable::copyChild(*it)));

		for (std::vector<ModifierDef>::const_iterator it = group.modifiers.begin(); it != group.modifiers.end(); ++it)
		{
			Modifier* modifier = dynamic_cast<Modifier*>(Registerable::copyChild(it->obj));
			ModifierDef modifierDef(modifier,attachDataSet(modifier));
			modifiers.push_back(modifierDef);
			if (system.isInitialized())
				sortedModifiers.push_back(modifierDef);
		}
		std::sort(sortedModifiers.begin(),sortedModifiers.end(),CompareModifierPriority());

		birthAction = dynamic_cast<Action*>(Registerable::copyChild(group.birthAction));
		deathAction = dynamic_cast<Action*>(Registerable::copyChild(group.deathAction));
	}

	Group::~Group()
	{
		delete[] particleData.positions;
		delete[] particleData.velocities;
		delete[] particleData.oldPositions;
		delete[] particleData.ages;
		delete[] particleData.lifeTimes;
		delete[] particleData.energies;
		delete[] particleData.sqrDists;
		delete[] particleData.colors;

		for (size_t i = 0; i < NB_PARAMETERS; ++i)
			delete[] particleData.parameters[i];

		Registerable::destroyChild(colorInterpolator.obj);
		Registerable::destroyChild(renderer.obj);

		destroyRenderBuffer();

		for (size_t i = 0; i < NB_PARAMETERS; ++i)
			Registerable::destroyChild(paramInterpolators[i].obj);

		for (std::vector<Emitter*>::const_iterator it = emitters.begin(); it != emitters.end(); ++it)
			Registerable::destroyChild(*it);

		for (std::vector<ModifierDef>::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			Registerable::destroyChild(it->obj);

		Registerable::destroyChild(birthAction);
		Registerable::destroyChild(deathAction);

		for (std::deque<CreationData>::const_iterator it = creationBuffer.begin(); it != creationBuffer.end(); ++it)
		{
			Registerable::decrementChild(it->emitter);
			Registerable::decrementChild(it->zone);
		}
	}

	Particle Group::getParticle(size_t index)
	{
		SPK_ASSERT(index < particleData.nbParticles,"Group::getParticle(size_t) - Particle index is out of bounds : " << index);
		return Particle(*this,index);
	}

	const Particle Group::getParticle(size_t index) const
	{
		SPK_ASSERT(index < particleData.nbParticles,"Group::getParticle(size_t) - Particle index is out of bounds : " << index);
		return Particle(const_cast<Group&>(*this),index);
	}

	void Group::setLifeTime(float minLifeTime,float maxLifeTime)
	{
		SPK_ASSERT(minLifeTime > 0.0f && maxLifeTime > 0.0f,"Group::setLifeTime(float,float) - Life times must not be set to negative values");
		if (minLifeTime <= maxLifeTime)
		{
			this->minLifeTime = minLifeTime;
			this->maxLifeTime = maxLifeTime;
		}
		else
		{
			SPK_LOG_WARNING("Group::setEnergy(float,float) - minEnergy is higher than maxEnergy - Values are swapped");
			this->minLifeTime = maxLifeTime;
			this->maxLifeTime = minLifeTime;
		}
	}

	bool Group::updateParticles(float deltaTime)
	{
		// Prepares the additionnal data
		prepareAdditionnalData();

		size_t nbAutoBorn = 0;
		size_t nbManualBorn = nbBufferedParticles;

		// Checks the number of born particles
		bool hasAliveEmitters = false;
		activeEmitters.clear();

		for (std::vector<Emitter*>::const_iterator it = emitters.begin(); it != emitters.end(); ++it)
			if ((*it)->isActive())
			{
				int nb = (*it)->updateTankFromTime(deltaTime);
				if (nb > 0)
				{
					activeEmitters.push_back(EmitterPair(*it,nb));
					nbAutoBorn += nb;
				}

				hasAliveEmitters |= ((*it)->getTank() != 0); // An emitter with some particles in its tank is still potentially alive
			}

		size_t emitterIndex = 0;
		size_t nbBorn = nbAutoBorn + nbManualBorn;

		// Updates the age of the particles function of the delta time
		for (size_t i = 0; i < particleData.nbParticles; ++i)
			particleData.ages[i] += deltaTime;

		// Computes the energy of the particles (if they are not immortal)
		if (!immortal)
			for (size_t i = 0; i < particleData.nbParticles; ++i)
				particleData.energies[i] = 1.0f - particleData.ages[i] / particleData.lifeTimes[i];

		// Updates the position of particles function of their velocity
		if (!still)
			for (size_t i = 0; i < particleData.nbParticles; ++i)
			{
				particleData.oldPositions[i] = particleData.positions[i];
				particleData.positions[i] += particleData.velocities[i] * deltaTime;
			}

		// Interpolates the parameters
		if (colorInterpolator.obj != NULL)
			colorInterpolator.obj->interpolate(particleData.colors,*this,colorInterpolator.dataSet);
		for (size_t i = 0; i < nbEnabledParameters; ++i)
		{
			FloatInterpolatorDef& interpolator = paramInterpolators[enabledParamIndices[i]];
			interpolator.obj->interpolate(particleData.parameters[enabledParamIndices[i]],*this,interpolator.dataSet);
		}

		// Modifies the particles with specific active modifiers behavior
		for (std::vector<ModifierDef>::const_iterator it = activeModifiers.begin(); it != activeModifiers.end(); ++it)
			it->obj->modify(*this,it->dataSet,deltaTime);

		// Updates the renderer data
		if (renderer.obj != NULL)
			renderer.obj->update(*this,renderer.dataSet);

		// Checks dead particles and reinits or swaps
		for (size_t i = 0; i < particleData.nbParticles; ++i)
			if (particleData.energies[i] <= 0.0f)
			{
				// Death action
				if (deathAction	!= NULL && deathAction->isActive())
				{
				    Particle particle = getParticle(i); // fix for gcc
					deathAction->apply(particle);
				}

				if (nbBorn > 0) // If particles are to be born, reinits a dead one with a new one
				{
					initParticle(i,emitterIndex,nbManualBorn);
					--nbBorn;
				}
				else // Else inactivate the dead one
				{
					swapParticles(i,particleData.nbParticles - 1);
					--particleData.nbParticles;
				}
			}

		// Emits new particles if some left
		for (size_t i = std::min(nbBorn,particleData.maxParticles - particleData.nbParticles); i > 0; --i)
			initParticle(particleData.nbParticles++,emitterIndex,nbManualBorn);

		// Computes the distance of particles from the camera
		if (distanceComputationEnabled)
		{
			for (size_t i = 0; i < particleData.nbParticles; ++i)
				particleData.sqrDists[i] = getSqrDist(particleData.positions[i],System::getCameraPosition());
		}

		return hasAliveEmitters || particleData.nbParticles > 0;
	}

	void Group::renderParticles()
	{
		if (renderer.obj != NULL && renderer.obj->isActive())
		{
			renderer.obj->prepareData(*this,renderer.dataSet);
			if (renderer.renderBuffer == NULL)
				renderer.renderBuffer = renderer.obj->attachRenderBuffer(*this);
			renderer.obj->render(*this,renderer.dataSet,renderer.renderBuffer);
		}
	}

	void Group::reallocate(size_t capacity)
	{
		SPK_ASSERT(capacity != 0,"Group::reallocate(size_t) - Group capacity must not be 0");

		if (system.isInitialized() && (!particleData.initialized || capacity != particleData.maxParticles))
		{
			destroyAllAdditionnalData();

			size_t copySize = particleData.nbParticles;
			if (capacity < copySize)
				copySize = capacity;

			reallocateArray(particleData.positions,capacity,copySize);
			reallocateArray(particleData.velocities,capacity,copySize);
			reallocateArray(particleData.oldPositions,capacity,copySize);
			reallocateArray(particleData.ages,capacity,copySize);
			reallocateArray(particleData.lifeTimes,capacity,copySize);
			reallocateArray(particleData.energies,capacity,copySize);
			reallocateArray(particleData.sqrDists,capacity,copySize);
			reallocateArray(particleData.colors,capacity,copySize);

			for (size_t i = 0; i < nbEnabledParameters; ++i)
				reallocateArray(particleData.parameters[enabledParamIndices[i]],capacity,copySize);

			particleData.initialized = true;
		}

		particleData.maxParticles = capacity;
	}

	void Group::setColorInterpolator(ColorInterpolator* interpolator)
	{
		if (colorInterpolator.obj != interpolator)
		{
			detachDataSet(colorInterpolator.dataSet);
			Registerable::decrementChild(colorInterpolator.obj);

			colorInterpolator.obj = interpolator;
			Registerable::incrementChild(interpolator);
			colorInterpolator.dataSet = attachDataSet(interpolator);
		}
	}

	void Group::setParamInterpolator(Param param,FloatInterpolator* interpolator)
	{
		if (paramInterpolators[param].obj == NULL && interpolator != NULL)
		{
			if (particleData.parameters[param] != NULL)
				SPK_LOG_ERROR("setParamInterpolator(Param,FloatInterpolator*) - Unexpected memory leak happened");

			// Creates the data for the parameter
			if (system.isInitialized())
				particleData.parameters[param] = new float[particleData.maxParticles];
		}
		else if (paramInterpolators[param].obj != NULL && interpolator == NULL)
		{
			if (particleData.parameters[param] == NULL)
			{
				SPK_LOG_ERROR("setParamInterpolator(Param,FloatInterpolator*) - Unexpected error happened");
			}

			// Destroys the data for the parameter
			delete[] particleData.parameters[param];
			particleData.parameters[param] = NULL;
		}

		detachDataSet(paramInterpolators[param].dataSet);
		Registerable::decrementChild(paramInterpolators[param].obj);

		paramInterpolators[param].obj = interpolator;
		Registerable::incrementChild(interpolator);
		paramInterpolators[param].dataSet = attachDataSet(interpolator);
		recomputeEnabledParamIndices();
	}

	void Group::addEmitter(Emitter* emitter)
	{
		if (emitter == NULL)
		{
			SPK_LOG_WARNING("Group::addEmitter(Emitter*) - A NULL emitter cannot must not be added to a group");
			return;
		}

		std::vector<Emitter*>::const_iterator it = std::find(emitters.begin(),emitters.end(),emitter);
		if (it != emitters.end())
		{
			SPK_LOG_WARNING("Group::addEmitter(Emitter*) - The emitter is already in the group and cannot be added");
			return;
		}

		Registerable::incrementChild(emitter);
		emitters.push_back(emitter);
	}

	void Group::removeEmitter(Emitter* emitter)
	{
		std::vector<Emitter*>::iterator it = emitters.begin();
		if (it != emitters.end())
		{
			Registerable::decrementChild(*it);
			emitters.erase(it);
		}
		else
		{
			SPK_LOG_WARNING("Group::removeEmitter(Emitter*) - The emitter was not found in the group and cannot be removed");
		}
	}

	void Group::addModifier(Modifier* modifier)
	{
		if (modifier == NULL)
		{
			SPK_LOG_WARNING("Group::addModifier(Modifier*) - A NULL modifier cannot be added to a group");
			return;
		}

		for (std::vector<ModifierDef>::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			if (it->obj == modifier)
			{
				SPK_LOG_WARNING("Group::addModifier(Modifier*) - The modifier is already in the group and cannot be added");
				return;
			}

		Registerable::incrementChild(modifier);
		ModifierDef modifierDef(modifier,attachDataSet(modifier));
		modifiers.push_back(modifierDef);
		if (system.isInitialized())
		{
			sortedModifiers.push_back(modifierDef);
			std::sort(sortedModifiers.begin(),sortedModifiers.end(),CompareModifierPriority());
			SPK_ASSERT(modifiers.size() == sortedModifiers.size(),"Group::addModifier(Modifier*) - Internal Error - Inconsistent storage of modifiers");
		}
	}

	void Group::removeModifier(Modifier* modifier)
	{
		for (std::vector<ModifierDef>::iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			if (it->obj == modifier)
			{
				detachDataSet(it->dataSet);
				Registerable::decrementChild(it->obj);
				if (system.isInitialized())
					for (std::vector<ModifierDef>::iterator it2 = sortedModifiers.begin(); it2 != sortedModifiers.end(); ++it2)
						if (it2->obj == modifier)
						{
							sortedModifiers.erase(it2);
							break;
						}
				modifiers.erase(it);
				return;
			}

		if (system.isInitialized())
			SPK_ASSERT(modifiers.size() == sortedModifiers.size(),"Group::addModifier(Modifier*) - Internal Error - Inconsistent storage of modifiers");
		SPK_LOG_WARNING("The modifier was not found in the group and cannot be removed");
	}

	void Group::setRenderer(Renderer* renderer)
	{
		if (this->renderer.obj != renderer)
		{
			destroyRenderBuffer();

			detachDataSet(this->renderer.dataSet);
			Registerable::decrementChild(this->renderer.obj);

			this->renderer.obj = renderer;
			Registerable::incrementChild(renderer);
			this->renderer.dataSet = attachDataSet(renderer);
		}
	}

	void Group::recomputeEnabledParamIndices()
	{
		nbEnabledParameters = 0;
		for (size_t i = 0; i < NB_PARAMETERS; ++i)
			if (paramInterpolators[i].obj != NULL)
				enabledParamIndices[nbEnabledParameters++] = i;
	}

	void Group::initParticle(size_t index,size_t& emitterIndex,size_t& nbManualBorn)
	{
		Particle particle(getParticle(index));

		particleData.ages[index] = 0.0f;
		particleData.energies[index] = 1.0f;
		particleData.lifeTimes[index] = SPK_RANDOM(minLifeTime,maxLifeTime);

		if (colorInterpolator.obj != NULL)
			colorInterpolator.obj->init(particleData.colors[index],particle,colorInterpolator.dataSet);
		else
			particleData.colors[index] = 0xFFFFFFFF;

		for (size_t i = 0; i < nbEnabledParameters; ++i)
		{
			FloatInterpolatorDef& interpolator = paramInterpolators[enabledParamIndices[i]];
			interpolator.obj->init(particleData.parameters[enabledParamIndices[i]][index],particle,interpolator.dataSet);
		}

		if (nbManualBorn == 0)
		{
			activeEmitters[emitterIndex].obj->emit(particle);
			if (--activeEmitters[emitterIndex].nbBorn == 0)
				++emitterIndex;
		}
		else
		{
			CreationData& creationData = creationBuffer.front();

			if (creationData.zone != NULL)
				creationData.zone->generatePosition(particle.position(),creationData.full);
			else
				particle.position() = creationData.position;

			if (creationData.emitter != NULL)
			{
				float speed = SPK_RANDOM(creationData.emitter->getForceMin(),creationData.emitter->getForceMax()) / particle.getParam(PARAM_MASS);
				creationData.emitter->generateVelocity(particle,speed);
			}
			else
				particle.velocity() = creationData.velocity;

			--creationBuffer.front().nb;
			--nbManualBorn;
			--nbBufferedParticles;
			if (creationBuffer.front().nb <= 0)
			{
				Registerable::decrementChild(creationData.emitter);
				Registerable::decrementChild(creationData.zone);
				creationBuffer.pop_front();
			}
		}

		particleData.oldPositions[index] = particleData.positions[index];

		for (std::vector<ModifierDef>::iterator it = initModifiers.begin(); it != initModifiers.end(); ++it)
			it->obj->init(particle,it->dataSet);

		if (renderer.obj != NULL && renderer.obj->isActive())
			renderer.obj->init(particle,renderer.dataSet);

		// birth action
		if (birthAction != NULL && birthAction->isActive())
			birthAction->apply(particle);
	}

	void Group::swapParticles(size_t index0,size_t index1)
	{
		// Swaps particles attributes
		std::swap(particleData.positions[index0],particleData.positions[index1]);
		std::swap(particleData.velocities[index0],particleData.velocities[index1]);
		std::swap(particleData.oldPositions[index0],particleData.oldPositions[index1]);
		std::swap(particleData.ages[index0],particleData.ages[index1]);
		std::swap(particleData.energies[index0],particleData.energies[index1]);
		std::swap(particleData.lifeTimes[index0],particleData.lifeTimes[index1]);
		std::swap(particleData.sqrDists[index0],particleData.sqrDists[index1]);
		std::swap(particleData.colors[index0],particleData.colors[index1]);

		// Swaps particles enabled parameters
		for (size_t i = 0; i < nbEnabledParameters; ++i)
			std::swap(particleData.parameters[enabledParamIndices[i]][index0],particleData.parameters[enabledParamIndices[i]][index1]);

		// Swaps particles additionnal swappable data
		for (std::list<DataSet>::iterator it = dataSets.begin(); it != dataSets.end(); ++it)
			it->swap(index0,index1);
	}

	DataSet* Group::attachDataSet(DataHandler* dataHandler)
	{
		if (!system.isInitialized())
			return NULL;

		if (dataHandler != NULL && dataHandler->needsDataSet())
		{
			dataSets.push_back(DataSet());
			return &dataSets.back();
		}

		return NULL;
	}

	void Group::detachDataSet(DataSet* dataSet)
	{
		if (dataSet != NULL)
			for (std::list<DataSet>::iterator it = dataSets.begin(); it != dataSets.end(); ++it)
				if (&*it == dataSet)
				{
					dataSets.erase(it);
					break;
				}
	}

	void Group::destroyRenderBuffer()
	{
		delete renderer.renderBuffer;
		renderer.renderBuffer = NULL;
	}

	void Group::destroyAllAdditionnalData()
	{
		destroyRenderBuffer();

		for (std::list<DataSet>::iterator it = dataSets.begin(); it != dataSets.end(); ++it)
			it->destroyAllData();
	}

	void Group::sortParticles()
	{
		if (sortingEnabled)
			sortParticles(0,particleData.nbParticles - 1);
	}

	void Group::computeAABB()
	{
		const float maxFloat = std::numeric_limits<float>::max();
		AABBMin.set(maxFloat,maxFloat,maxFloat);
		AABBMax.set(-maxFloat,-maxFloat,-maxFloat);

		if (renderer.obj != NULL && renderer.obj->isActive())
		{
			renderer.obj->prepareData(*this,renderer.dataSet);
			renderer.obj->computeAABB(AABBMin,AABBMax,*this,renderer.dataSet);
		}
		else // Switches to default AABB computation
			for (size_t i = 0; i < particleData.nbParticles; ++i)
			{
				AABBMin.setMin(particleData.positions[i]);
				AABBMax.setMax(particleData.positions[i]);
			}
	}

	void Group::sortParticles(int start,int end)
	{
		// quick sort implementation (can be optimized)
		if (start < end)
		{
			int i = start - 1;
			int j = end + 1;
			float pivot = particleData.sqrDists[(start + end) >> 1];
			while (true)
			{
				do ++i;
				while (particleData.sqrDists[i] > pivot);
				do --j;
				while (particleData.sqrDists[j] < pivot);
				if (i < j)
					swapParticles(i,j);
				else break;
			}

			sortParticles(start,j);
			sortParticles(j + 1,end);
		}
	}

	void Group::propagateUpdateTransform()
	{
		for (std::vector<Emitter*>::const_iterator it = emitters.begin(); it != emitters.end(); ++it)
			if (!(*it)->isShared())
				(*it)->updateTransform(&system);

		for (std::vector<ModifierDef>::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			if (!it->obj->isShared() && it->obj->isLocalToSystem())
				it->obj->updateTransform(&system);
	}

	void Group::addParticles(unsigned int nb,Zone* zone,Emitter* emitter,bool full)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(unsigned int,Zone*,Emitter*,bool) - emitter must not be NULL");
		SPK_ASSERT(zone != NULL,"Group::addParticles(unsigned int,Zone*,Emitter*,bool) - zone must not be NULL");
		addParticles(emitter->updateTankFromNb(nb),Vector3D(),Vector3D(),zone,emitter,full);
	}

	void Group::addParticles(unsigned int nb,Zone* zone,const Vector3D& velocity,bool full)
	{
		SPK_ASSERT(zone != NULL,"Group::addParticles(unsigned int,Zone*,const Vector3D&,bool) - zone must not be NULL");
		addParticles(nb,Vector3D(),velocity,zone,NULL,full);
	}

	void Group::addParticles(unsigned int nb,const Vector3D& position,Emitter* emitter)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(unsigned int,const Vector3D&,Emitter*) - emitter must not be NULL");
		addParticles(emitter->updateTankFromNb(nb),position,Vector3D(),NULL,emitter);
	}

	void Group::addParticles(unsigned int nb,Emitter* emitter)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(unsigned int,Emitter*) - emitter must not be NULL");
		addParticles(emitter->updateTankFromNb(nb),Vector3D(),Vector3D(),emitter->getZone(),emitter,emitter->isFullZone());
	}

	void Group::addParticles(Zone* zone,Emitter* emitter,float deltaTime,bool full)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(Zone*,Emitter*,float,bool) - emitter must not be NULL");
		SPK_ASSERT(zone != NULL,"Group::addParticles(Zone*,Emitter*,float,bool) - zone must not be NULL");
		addParticles(emitter->updateTankFromTime(deltaTime),Vector3D(),Vector3D(),zone,emitter,full);
	}

	void Group::addParticles(const Vector3D& position,Emitter* emitter,float deltaTime)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(const Vector3D&,Emitter*,float) - emitter must not be NULL");
		addParticles(emitter->updateTankFromTime(deltaTime),position,Vector3D(),NULL,emitter);
	}

	void Group::addParticles(Emitter* emitter,float deltaTime)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(Emitter*,float) - emitter must not be NULL");
		addParticles(emitter->updateTankFromTime(deltaTime),Vector3D(),Vector3D(),emitter->getZone(),emitter,emitter->isFullZone());
	}

	float Group::addParticles(const Vector3D& start,const Vector3D& end,Emitter* emitter,float step,float offset)
	{
		SPK_ASSERT(emitter != NULL,"Group::addParticles(const Vector3D&,const Vector3D&,Emitter*,float,float) - emitter must not be NULL");

		if ((step <= 0.0f)||(offset < 0.0f))
			return 0.0f;

		Vector3D displacement = end - start;
		float totalDist = displacement.getNorm();

		while(offset < totalDist)
		{
			Vector3D position = start;
			position += displacement * offset / totalDist;
			addParticles(1,position,Vector3D(),NULL,emitter);
			offset += step;
		}

		return offset - totalDist;
	}

	float Group::addParticles(const Vector3D& start,const Vector3D& end,const Vector3D& velocity,float step,float offset)
	{
		if ((step <= 0.0f)||(offset < 0.0f))
			return 0.0f;

		Vector3D displacement = end - start;
		float totalDist = displacement.getNorm();

		while(offset < totalDist)
		{
			Vector3D position = start;
			position += displacement * (offset / totalDist);
			addParticles(1,position,velocity,NULL,NULL);
			offset += step;
		}

		return offset - totalDist;
	}

	void Group::addParticles(unsigned int nb,const Vector3D& position,const Vector3D& velocity,Zone* zone,Emitter* emitter,bool full)
	{
		if (nb == 0)
			return;

		SPK_ASSERT(system.isInitialized(),"Group::addParticles(unsigned int,const Vector3D&,const Vector3D&,Zone*,Emitter*,bool) - Particles cannot be added to an uninitialized group");

		Registerable::incrementChild(emitter);
		Registerable::incrementChild(zone);

		CreationData data = {nb,position,velocity,zone,emitter,full};
		creationBuffer.push_back(data);
		nbBufferedParticles += nb;
	}

	void Group::flushBufferedParticles()
	{
		prepareAdditionnalData();

		unsigned int nbManualBorn = nbBufferedParticles;
		if (nbManualBorn > getCapacity() - getNbParticles())
			nbManualBorn = getCapacity() - getNbParticles();

		size_t dummy;
		while(nbManualBorn > 0)
			initParticle(particleData.nbParticles++,dummy,nbManualBorn);

		if (nbBufferedParticles > 0)
		{
			for (std::deque<CreationData>::const_iterator it = creationBuffer.begin(); it != creationBuffer.end(); ++it)
			{
				Registerable::decrementChild(it->emitter);
				Registerable::decrementChild(it->zone);
			}
			creationBuffer.clear();
			nbBufferedParticles = 0;
		}
	}

	inline void Group::prepareAdditionnalData()
	{
		if (renderer.obj != NULL)
			renderer.obj->prepareData(*this,renderer.dataSet);

		activeModifiers.clear();
		initModifiers.clear();

		for (std::vector<ModifierDef>::const_iterator it = sortedModifiers.begin(); it != sortedModifiers.end(); ++it)
		{
			it->obj->prepareData(*this,it->dataSet);	// if it has a data set, it is prepared
			if (it->obj->CALL_INIT)
				initModifiers.push_back(*it); // if its init method needs to be called it is added to the init vector
			if (it->obj->isActive())
				activeModifiers.push_back(*it); // if the modifier is active, it is added to the active vector
		}

		if (colorInterpolator.obj != NULL)
		{
			colorInterpolator.obj->prepareData(*this,colorInterpolator.dataSet);
			colorInterpolator.obj->interpolate(particleData.colors,*this,colorInterpolator.dataSet);
		}
		for (size_t i = 0; i < nbEnabledParameters; ++i)
		{
			FloatInterpolatorDef& interpolator = paramInterpolators[enabledParamIndices[i]];
			interpolator.obj->prepareData(*this,interpolator.dataSet);
		}
	}

	void Group::initData()
	{
		if (!particleData.initialized)
		{
			// Creates particle data arrays
			reallocate(particleData.maxParticles);

			// Creates data sets
			renderer.dataSet = attachDataSet(renderer.obj);
			colorInterpolator.dataSet = attachDataSet(colorInterpolator.obj);
			for (size_t i = 0; i < nbEnabledParameters; ++i)
			{
				FloatInterpolatorDef& interpolator = paramInterpolators[enabledParamIndices[i]];
				interpolator.dataSet = attachDataSet(interpolator.obj);
			}

			for (std::vector<ModifierDef>::iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			{
				it->dataSet = attachDataSet(it->obj);
				sortedModifiers.push_back(*it);
			}
			std::sort(sortedModifiers.begin(),sortedModifiers.end(),CompareModifierPriority());
		}
	}

	void Group::setBirthAction(Action* action)
	{
		Registerable::decrementChild(birthAction);
		birthAction = action;
		Registerable::incrementChild(birthAction);
	}

	void Group::setDeathAction(Action* action)
	{
		Registerable::decrementChild(deathAction);
		deathAction = action;
		Registerable::incrementChild(deathAction);
	}

	DataSet* Group::getModifierDataSet(Modifier* modifier)
	{
		for (std::vector<ModifierDef>::const_iterator it = modifiers.begin(); it != modifiers.end(); ++it)
			if (it->obj == modifier)
				return it->dataSet;

		SPK_LOG_WARNING("Group::getModifierDataSet(Modifier*) - The modifier was not found in the group, NULL is returned");
		return NULL;
	}

	void Group::setRadius(float radius)
	{
		if (radius < 0)
		{
			radius = 0.0f;
			SPK_LOG_WARNING("Group::setRadius(float) - The radius cannot be set to a negative value - 0 is used");
		}
		this->radius = radius;
	}
}
