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


#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_Modifier.h"
#include "Core/SPK_System.h"
#include "Core/SPK_Buffer.h"


namespace SPK
{
	Particle::Particle(Group* group,size_t index) :
		group(group),
		index(index),
		data(group->particleData + index),
		enableParams(group->particleEnableParams + index * group->model->getNbValuesInParticleEnableArray()),
		mutableParams(group->particleMutableParams + index * group->model->getNbValuesInParticleMutableArray())
	{
		init();
	}

	void Particle::init()
	{
		const Model* model = group->getModel();
		data->age = 0.0f;
		data->life = random(model->lifeTimeMin,model->lifeTimeMax);

		// creates pseudo-iterators to parse arrays
		float* particleEnableIt = enableParams;
		float* particleMutableIt = mutableParams;
		const int* paramIt = model->enableParams;

		// initializes params
		for (size_t i = model->enableParamsSize; i != 0; --i)
		{
			const float* templateIt = &model->params[model->indices[*paramIt]];

			if (!model->isRandom(static_cast<ModelParam>(*paramIt)))
			{
				*particleEnableIt++ = *templateIt;
				if (model->isMutable(static_cast<ModelParam>(*paramIt)))
					*particleMutableIt++ = *(templateIt + 1);
			}
			else
			{
				*particleEnableIt++ = random(*templateIt,*(templateIt + 1));
				if (model->isMutable(static_cast<ModelParam>(*paramIt)))
					*particleMutableIt++ = random(*(templateIt + 2),*(templateIt + 3));
			}
			++paramIt;
		}
	}

	bool Particle::update(float deltaTime)
	{
		const Model* model = group->getModel();
		data->age += deltaTime;

		if (!model->immortal)
		{
			// computes the ratio between the life of the particle and its lifetime
			float ratio = std::min(1.0f,deltaTime / data->life);
			data->life -= deltaTime;
			
			// update mutable parameters
			for (size_t i = 0; i < model->mutableParamsSize; ++i)
			{
				size_t index = model->mutableParams[i];
				size_t enableIndex = model->particleEnableIndices[index];
				enableParams[enableIndex] += (mutableParams[model->particleMutableIndices[index]] - enableParams[enableIndex]) * ratio;
			}
		}

		// updates position
		oldPosition() = position();
		position() += velocity() * deltaTime;

		// updates velocity
		velocity() += group->getGravity() * deltaTime;

		std::vector<Modifier*>::const_iterator end = group->activeModifiers.end();
		for (std::vector<Modifier*>::const_iterator it = group->activeModifiers.begin(); it != end; ++it)
			(*it)->process(*this,deltaTime);

		if (group->getFriction() != 0.0f)
			velocity() *= 1.0f - std::min(1.0f,group->getFriction() * deltaTime / getParamCurrentValue(PARAM_MASS));

		return data->life <= 0.0f;
	}

	bool Particle::setParamCurrentValue(ModelParam type,float value)
	{
		const Model* const model = group->getModel();
		if (model->isEnabled(type))
		{
			enableParams[model->particleEnableIndices[type]] = value;
			return true;
		}

		return false;
	}

	bool Particle::setParamFinalValue(ModelParam type,float value)
	{
		const Model* const model = group->getModel();
		if (model->isMutable(type))
		{
			mutableParams[model->particleMutableIndices[type]] = value;
			return true;
		}

		return false;
	}

	bool Particle::changeParamCurrentValue(ModelParam type,float delta)
	{
		const Model* const model = group->getModel();
		if (model->isEnabled(type))
		{
			enableParams[model->particleEnableIndices[type]] += delta;
			return true;
		}

		return false;
	}

	bool Particle::changeParamFinalValue(ModelParam type,float delta)
	{
		const Model* const model = group->getModel();
		if (model->isMutable(type))
		{
			mutableParams[model->particleMutableIndices[type]] += delta;
			return true;
		}

		return false;
	}

	float Particle::getParamCurrentValue(ModelParam type) const
	{
		const Model* const model = group->getModel();
		if (model->isEnabled(type))
			return enableParams[model->particleEnableIndices[type]];

		return Model::DEFAULT_VALUES[type];
	}

	float Particle::getParamFinalValue(ModelParam type) const
	{
		const Model* const model = group->getModel();
		if (model->isEnabled(type))
		{
			if (model->isMutable(type))
				return mutableParams[model->particleMutableIndices[type] + 1];
			return enableParams[model->particleEnableIndices[type]];
		}

		return Model::DEFAULT_VALUES[type];
	}

	Model* Particle::getModel() const
	{
		return group->getModel();
	}

	void Particle::computeSqrDist()
	{
		data->sqrDist = getSqrDist(position(),System::getCameraPosition());
	}

	extern void swapParticles(Particle& a,Particle& b)
	{
		//std::swap(a.index,b.index);
		std::swap(*a.data,*b.data);
		for (size_t i = 0; i < a.getModel()->getNbValuesInParticleEnableArray(); ++i)
			std::swap(a.enableParams[i],b.enableParams[i]);
		for (size_t i = 0; i < a.getModel()->getNbValuesInParticleMutableArray(); ++i)
			std::swap(a.mutableParams[i],b.mutableParams[i]);
		
		// swap additional data (groups are assumed to be the same)
		for (std::map<std::string,Buffer*>::iterator it = a.group->additionalBuffers.begin(); it != a.group->additionalBuffers.end(); ++it)
		{
			Buffer* buffer = it->second;
			if (buffer->isSwapEnabled())
				buffer->swap(a.index,b.index);
		}
	}
}
