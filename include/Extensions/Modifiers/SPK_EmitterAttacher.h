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

#ifndef H_SPK_EMITTERATTACHER
#define H_SPK_EMITTERATTACHER

#include "Core/SPK_Modifier.h"

namespace SPK
{
	class Emitter;

	class EmitterAttacher : public Modifier
	{
	SPK_IMPLEMENT_REGISTERABLE(EmitterAttacher)

	public :

		static inline EmitterAttacher* create(size_t groupIndex = 0,Emitter* emitter = NULL,bool orientate = false,bool rotate = false);
		~EmitterAttacher();

		void setEmitter(Emitter* emitter);
		inline Emitter* getEmitter() const;

		inline void setGroupIndex(size_t index);
		inline size_t getGroupIndex() const;

		inline void enableEmitterOrientation(bool orientate,bool rotate = false);
		inline bool isEmitterOrientationEnabled() const;
		inline bool isEmitterRotationEnabled() const;

	protected :

		virtual void createData(DataSet& dataSet,const Group& group) const;
		virtual void checkData(DataSet& dataSet,const Group& group) const;

	private :

		// Data indices
		static const size_t NB_DATA = 1;
		static const size_t EMITTER_INDEX = 0;

		class EmitterData : public Data
		{
		public :

			inline EmitterData(size_t nbParticles,Group* emittingGroup);
			
			inline Emitter** getEmitters() const;
			inline void setGroup(Group* group);
			inline Group* getGroup() const;

			void destroyEmitter(size_t index);
			void createEmitter(size_t index,const Emitter* emitter);

			void flushUsedEmitters();

		private :

			Emitter** data;
			size_t dataSize;

			Group* group;

			~EmitterData();

			virtual inline void swap(size_t index0,size_t index1);
		};

		Emitter* baseEmitter;

		bool orientationEnabled;
		bool rotationEnabled;

		size_t groupIndex;

		EmitterAttacher(size_t groupIndex = 0,Emitter* emitter = NULL,bool orientate = false,bool rotate = false);
		EmitterAttacher(const EmitterAttacher& emitterAttacher);

		bool checkEmitterValidity() const;

		virtual void init(Particle& particle,DataSet* dataSet) const;
		virtual void modify(Group& group,DataSet* dataSet,float deltaTime) const;
	};

	inline EmitterAttacher* EmitterAttacher::create(size_t groupIndex,Emitter* emitter,bool orientate,bool rotate)
	{
		return SPK_NEW(EmitterAttacher,groupIndex,emitter,orientate,rotate);
	}

	inline Emitter* EmitterAttacher::getEmitter() const
	{
		return baseEmitter;
	}

	inline void EmitterAttacher::setGroupIndex(size_t index)
	{
		groupIndex = index;
	}

	inline size_t EmitterAttacher::getGroupIndex() const
	{
		return groupIndex;
	}

	inline void EmitterAttacher::enableEmitterOrientation(bool orientate,bool rotate)
	{
		orientationEnabled = orientate;
		rotationEnabled = rotate;
	}

	inline bool EmitterAttacher::isEmitterOrientationEnabled() const
	{
		return orientationEnabled;
	}

	inline bool EmitterAttacher::isEmitterRotationEnabled() const
	{
		return rotationEnabled;
	}

	inline Emitter** EmitterAttacher::EmitterData::getEmitters() const
	{
		return data;
	}

	inline void EmitterAttacher::EmitterData::setGroup(Group* group)
	{
		this->group = group;
	}

	inline Group* EmitterAttacher::EmitterData::getGroup() const
	{
		return group;
	}

	inline void EmitterAttacher::EmitterData::swap(size_t index0,size_t index1)
	{
		std::swap(data[index0],data[index1]);
	}
}

#endif
