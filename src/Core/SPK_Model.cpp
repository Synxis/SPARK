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


#include "Core/SPK_Model.h"


namespace SPK
{
	Model defaultModel;

	const float Model::DEFAULT_VALUES[NB_PARAMS] =
	{
		1.0f,	// RED
		1.0f,	// GREEN
		1.0f,	// BLUE
		1.0f,	// ALPHA
		1.0f,	// SIZE
		1.0f,	// WEIGHT
		0.0f,	// ANGLE
		0.0f,	// TEXTURE_INDEX
		0.0f,	// CUSTOM_0
		0.0f,	// CUSTOM_1
		0.0f,	// CUSTOM_2
	};

	Model::Model(int enableFlag,int mutableFlag,int randomFlag) :
		Registerable(),
		lifeTimeMin(1.0f),
		lifeTimeMax(1.0f),
		immortal(false),
		paramsSize(0),
		enableParamsSize(0),
		mutableParamsSize(0)
	{
		// Adds the color parameters to the enable flag
		enableFlag |= FLAG_RED | FLAG_GREEN | FLAG_BLUE;
		// masks the enable flag with the existing parameters
		this->enableFlag = enableFlag & ((1 << (NB_PARAMS + 1)) - 1);
		// masks the mutable flag with the enable flag
		this->mutableFlag = mutableFlag & this->enableFlag;
		// masks the random flag with the enable flag
		this->randomFlag = randomFlag & this->enableFlag;

		int particleEnableParamsSize = 0;
		int particleMutableParamsSize = 0;
		for (size_t i = 0; i < NB_PARAMS; ++i)
		{
			// computes the size of the array of params stored by the particle template
			// 1 if the param is enable
			int paramSize = isEnabled(static_cast<ModelParam>(i)) >> i;
			enableParamsSize += paramSize;
			// *2 if the param is mutable along the time
			paramSize <<= isMutable(static_cast<ModelParam>(i)) >> i;
			mutableParamsSize += paramSize >> 1;
			// *2 if the param is randomly generated
			paramSize <<= isRandom(static_cast<ModelParam>(i)) >> i;

			// computes the size of the array of params stored by a single particle
			particleEnableIndices[i] = particleEnableParamsSize;
			particleMutableIndices[i] = particleMutableParamsSize;
			// 1 if the param is enable
			particleEnableParamsSize += isEnabled(static_cast<ModelParam>(i)) >> i;
			// 1 if the param is mutable
			particleMutableParamsSize += isMutable(static_cast<ModelParam>(i)) >> i;

			indices[i] = paramsSize;
			paramsSize += paramSize;
		}

		// creates the array of params for this ParticleTemplate
		if (paramsSize > 0)
		{
			params = new float[paramsSize];
			unsigned int currentParamIndex = 0;
			unsigned int currentIndex = 0;
			while (currentIndex < paramsSize)
			{
				unsigned int nbValues = getNbValues(static_cast<ModelParam>(currentParamIndex));
				for (size_t i = 0; i < nbValues; ++i)
					params[currentIndex + i] = DEFAULT_VALUES[currentParamIndex];
				++currentParamIndex;
				currentIndex += nbValues;
			}
		}
		else
			params = NULL;

		if (enableParamsSize > 0)
		{
			enableParams = new int[enableParamsSize];;
			size_t index = 0;
			for (size_t i = 0; i < NB_PARAMS; ++i)
				if (isEnabled(static_cast<ModelParam>(i)))
					enableParams[index++] = i;
		}
		else
			enableParams = NULL;

		if (mutableParamsSize > 0)
		{
			mutableParams = new int[mutableParamsSize];
			size_t index = 0;
			for (size_t i = 0; i < NB_PARAMS; ++i)
				if (isMutable(static_cast<ModelParam>(i)))
					mutableParams[index++] = i;
		}
		else
			mutableParams = NULL;

		// DEBUG
		/*std::cout << "enable : " << enableFlag << std::endl;
		std::cout << "mutable : " << mutableFlag << std::endl;
		std::cout << "random : " << randomFlag << std::endl;

		std::cout << "params : " << params << std::endl;
		for (int i = 0; i < paramsSize; ++i)
			std::cout << params[i] << " ";
		std::endl;

		std::cout << "enableParams : " << enableParams << std::endl;
		for (int i = 0; i < enableParamsSize; ++i)
			std::cout << enableParams[i] << " ";
		std::endl;

		std::cout << "mutableParams : " << mutableParams << std::endl;
		for (int i = 0; i < mutableParamsSize; ++i)
			std::cout << mutableParams[i] << " ";
		std::endl;

		std::cout << "indices"
		for (int i = 0; i < NB_PARAMS; ++i)
			std::cout << indices[i] << " ";
		std::endl;

		std::cout << "particleEnableIndices"
		for (int i = 0; i < NB_PARAMS; ++i)
			std::cout << particleEnableIndices[i] << " ";
		std::endl;

		std::cout << "particleMutableIndices"
		for (int i = 0; i < NB_PARAMS; ++i)
			std::cout << particleMutableIndices[i] << " ";
		std::endl;*/
	}

	Model::Model(const Model& model) :
		Registerable(model),
		lifeTimeMin(model.lifeTimeMin),
		lifeTimeMax(model.lifeTimeMax),
		immortal(model.immortal),
		paramsSize(model.paramsSize),
		enableParamsSize(model.enableParamsSize),
		mutableParamsSize(model.mutableParamsSize),
		enableFlag(model.enableFlag),
		mutableFlag(model.mutableFlag),
		randomFlag(model.randomFlag),
		params(NULL),
		enableParams(NULL),
		mutableParams(NULL)
	{
		initParamArrays(model);
	}

	Model::~Model()
	{
		delete[] enableParams;
		delete[] mutableParams;
		delete[] params;
	}

	void Model::initParamArrays(const Model& model)
	{
		if (paramsSize > 0)
		{
			params = new float[paramsSize];
			for (size_t i = 0; i < paramsSize; ++i)
				params[i] = model.params[i];
		}

		if (enableParamsSize > 0)
		{
			enableParams = new int[enableParamsSize];
			for (size_t i = 0; i < enableParamsSize; ++i)
				enableParams[i] = model.enableParams[i];
		}

		if (mutableParamsSize > 0)
		{
			mutableParams = new int[mutableParamsSize];
			for (size_t i = 0; i < mutableParamsSize; ++i)
				mutableParams[i] = model.mutableParams[i];
		}

		for (size_t i = 0; i < NB_PARAMS; ++i)
		{
			indices[i] = model.indices[i];
			particleEnableIndices[i] = model.particleEnableIndices[i];
			particleMutableIndices[i] = model.particleMutableIndices[i];	
		}
	}

	bool Model::setParam(ModelParam type,float startMin,float startMax,float endMin,float endMax)
	{
		// if the given param doesnt have 4 values, return
		if (getNbValues(type) != 4)
			return false;

		// Sets the values at the right position in params
		float* ptr = params + indices[type];
		*ptr++ = startMin;
		*ptr++ = startMax;
		*ptr++ = endMin;
		*ptr = endMax;

		return true;
	}

	bool Model::setParam(ModelParam type,float value0,float value1)
	{
		// if the given param doesnt have 2 values, return
		if (getNbValues(type) != 2)
			return false;

		// Sets the values at the right position in params
		float* ptr = params + indices[type];
		*ptr++ = value0;
		*ptr = value1;

		return true;
	}

	bool Model::setParam(ModelParam type,float value)
	{
		// if the given param doesnt have 1 value, return
		if (getNbValues(type) != 1)
			return false;

		// Sets the value at the right position in params
		params[indices[type]] = value;

		return true;
	}

	float Model::getParamValue(ModelParam type,size_t index) const
	{
		unsigned int nbValues = getNbValues(type);

		if (nbValues == 0)
			return DEFAULT_VALUES[type];

		if (nbValues - 1 > index)
			return params[indices[type] + index];

		return DEFAULT_VALUES[type];
	}

	unsigned int Model::getNbValues(ModelParam type) const
	{
		int value = 1 << type;
		if (!(enableFlag & value))
			return 0;

		if (!(mutableFlag & value) && !(randomFlag & value))
			return 1;

		if ((mutableFlag & value) && (randomFlag & value))
			return 4;

		return 2;
	}
}
