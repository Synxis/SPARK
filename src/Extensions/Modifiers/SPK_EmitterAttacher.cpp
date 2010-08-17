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

#include "Extensions/Modifiers/SPK_EmitterAttacher.h"
#include "Core/SPK_Emitter.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_System.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Zone.h"
#include "Core/SPK_Iterator.h"

namespace SPK
{
	EmitterAttacher::EmitterAttacher(size_t groupIndex,Emitter* emitter,bool orientate,bool rotate) :
		Modifier(MODIFIER_PRIORITY_CHECK,true,true),
		groupIndex(groupIndex),
		orientationEnabled(orientate),
		rotationEnabled(rotate)
	{
		setEmitter(emitter);
	}

	EmitterAttacher::EmitterAttacher(const EmitterAttacher& emitterAttacher) :
		Modifier(emitterAttacher),
		groupIndex(emitterAttacher.groupIndex),
		orientationEnabled(emitterAttacher.orientationEnabled),
		rotationEnabled(emitterAttacher.rotationEnabled)
	{
		baseEmitter = dynamic_cast<Emitter*>(copyChild(emitterAttacher.baseEmitter));
	}

	EmitterAttacher::~EmitterAttacher()
	{
		destroyChild(baseEmitter);
	}

	EmitterAttacher::EmitterData::EmitterData(size_t nbParticles,Group* emittingGroup) :
		data(SPK_NEW_ARRAY(Emitter*,nbParticles)),
		dataSize(nbParticles),
		group(emittingGroup)
	{
		for (size_t i = 0; i < dataSize; ++i)
			data[i] = NULL;
	}

	EmitterAttacher::EmitterData::~EmitterData()
	{
		for (size_t i = 0; i < dataSize; ++i)
			destroyEmitter(i);
		SPK_DELETE_ARRAY(data);
	}

	void EmitterAttacher::setEmitter(Emitter* emitter)
	{
		decrementChild(baseEmitter);
		baseEmitter = emitter;
		incrementChild(baseEmitter);
	}

	void EmitterAttacher::createData(DataSet& dataSet,const Group& group) const
	{
		dataSet.init(NB_DATA);
		dataSet.setData(EMITTER_INDEX,SPK_NEW(EmitterData,group.getCapacity(),group.getSystem().getGroup(groupIndex)));
	}

	void EmitterAttacher::checkData(DataSet& dataSet,const Group& group) const
	{
		// If the group has changed, flush the potentially used emitters
		EmitterData& data = SPK_GET_DATA(EmitterData,&dataSet,EMITTER_INDEX);
		Group* currentDataGroup = data.getGroup();
		Group* emittingGroup = group.getSystem().getGroup(groupIndex);
		if (currentDataGroup != emittingGroup)
		{
			data.flushUsedEmitters();
			data.setGroup(emittingGroup);
		}
	}

	bool EmitterAttacher::checkEmitterValidity() const
	{
		if (baseEmitter == NULL)
		{
			SPK_LOG_WARNING("SpawnParticlesAction::checkEmitterValidity() - The base emitter is NULL and cannot be used");
			return false;
		}
		else if (baseEmitter->getZone()->isShared())
		{
			SPK_LOG_WARNING("SpawnParticlesAction::checkEmitterValidity() - The base emitter is invalid (its zone is shared) and cannot be used");
			return false;
		}

		return true;
	}

	void EmitterAttacher::init(Particle& particle,DataSet* dataSet) const
	{
		// Only destroy the old emitter here as we need to ensure the emitters validity
		SPK_GET_DATA(EmitterData,dataSet,EMITTER_INDEX).destroyEmitter(particle.getIndex());
	}

	void EmitterAttacher::modify(Group& group,DataSet* dataSet,float deltaTime) const
	{
		if (!checkEmitterValidity())
			return;

		EmitterData& data = SPK_GET_DATA(EmitterData,dataSet,EMITTER_INDEX);
		Emitter** emitterIt = data.getEmitters();
		Group* emittingGroup = data.getGroup();

		bool rotationEnabled = this->rotationEnabled && group.isEnabled(PARAM_ANGLE);

		for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
		{
			const Particle& particle = *particleIt;

			if (*emitterIt == NULL)
				data.createEmitter(particle.getIndex(),baseEmitter);

			Emitter* emitter = *emitterIt;

			emitter->setTransformPosition(particle.position());
			if (orientationEnabled)
			{
				Vector3D look(-particle.velocity());
				Vector3D up(0.0f,1.0f,0.0f);

				if (look.x == 0.0f && look.z == 0.0f) // Handles special cases
				{
					float tmp = (look.y >= 0.0f ? 1.0f : -1.0f);
					look.set(0.0f,tmp,0.0f);
					up.set(0.0f,0.0f,-tmp);
				}
				if (rotationEnabled)
				{
					float angle = -particle.getParamNC(PARAM_ANGLE); // minus as look is inverted
					look.normalize();

					float c = std::cos(angle);
					float s = std::sin(angle);
					float a = 1 - c;

					up.x = look.x * look.y * a - look.z * s;
					up.y = look.y * look.y + (1.0f - look.y * look.y) * c;
					up.z = look.y * look.z * a + look.x * s;
				}
				
				emitter->setTransformOrientationRH(look,up); // TODO What about LH ?
			}

			emitter->updateTransform();
			emittingGroup->addParticles(emitter,deltaTime);

			++emitterIt;
		}
	}

	void EmitterAttacher::EmitterData::destroyEmitter(size_t index)
	{
		if (data[index] != NULL)
		{
			if (data[index]->getNbReferences() > 1)
				group->flushBufferedParticles();

			data[index]->decrement();
			SPK_DELETE(data[index]);
			data[index] = NULL;
		}
	}

	void EmitterAttacher::EmitterData::createEmitter(size_t index,const Emitter* emitter)
	{
		destroyEmitter(index);
		data[index] = dynamic_cast<Emitter*>(Registerable::copyRegisterable(emitter));
		data[index]->resetTransform();
	}

	void EmitterAttacher::EmitterData::flushUsedEmitters()
	{
		for (size_t i = 0; i < dataSize; ++i)
			if (data[i]->getNbReferences() > 1) // Ensures group is valid
			{
				group->flushBufferedParticles();
				break;
			}
	}
}
