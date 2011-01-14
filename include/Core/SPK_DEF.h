//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2011 - Julien Fryer - julienfryer@gmail.com				//
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

#ifndef H_SPK_DEF
#define H_SPK_DEF

#include <cstdlib>

// for windows platform only
#if defined(WIN32) || defined(_WIN32)

#ifdef SPK_CORE_EXPORT
#define SPK_PREFIX __declspec(dllexport)
#elif defined(SPK_IMPORT) || defined(SPK_CORE_IMPORT)
#define SPK_PREFIX __declspec(dllimport)
#else
#define SPK_PREFIX
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4251) // disables the warning about exporting STL classes in DLLs
#pragma warning(disable : 4996) // disables the deprecation of some functions
#pragma warning(disable : 4275) // disables the warning about exporting DLL classes children of non DLL classes
#endif

// other platforms than windows
#else
#define SPK_PREFIX
#endif

#include "Core/SPK_MemoryTracer.h"
#include "Core/SPK_Reference.h"

/**
* @brief A macro returning a random value within [min,max[
* This is a shortcut syntax to <i>SPK::SPKContext::getInstance().generateRandom(min,max)</i>
* @param min : the minimum bound of the interval (inclusive)
* @param max : the maximum bound of the interval (exclusive)
* @return a random number within [min,max[
*/
#define SPK_RANDOM(min,max) SPK::SPKContext::getInstance().generateRandom(min,max)

/**
* @brief A macro returning the zone by default of SPARK
* When an object needs a zone that must not be NULL, this zone can be used by default.<br>
* The default zone is a shared point located at position (0.0f,0.0f,0.0f).<br>
* <br>
* This is a shortcut syntax to <i>SPK::SPKContext::getInstance().getDefaultZone()</i>
* @return the default zone of SPARK
*/
#define SPK_DEFAULT_ZONE	SPK::SPKContext::getInstance().getDefaultZone()

/**
* @namespace SPK
* @brief the namespace for core SPARK code
*/
namespace SPK
{
	class Zone;

	/** @brief Constants defining parameters of a particle */
	enum Param
	{
		PARAM_SCALE = 0,				/**< @brief The scale of a particle */
		PARAM_MASS = 1,					/**< @brief The mass of a particle */
		PARAM_ANGLE = 2,				/**< @brief The rotation angle of a particle */
		PARAM_TEXTURE_INDEX = 3,		/**< @brief The index of texture coordinates of a particle */
		PARAM_ROTATION_SPEED = 4,		/**< @brief The rotation speed of a particle */
	};

	/** @brief Constants defining the way a factor is applied*/
	enum Factor
	{
		FACTOR_CONSTANT = 0,	/**< @brief Defines a constant factor (C) */
		FACTOR_LINEAR = 1,		/**< @brief Defines a linear factor (x) */
		FACTOR_QUADRATIC = 2,	/**< @brief Defines a quadratic factor (x^2) */
		FACTOR_CUBIC = 3,		/**< @brief Defines a cubic factor (x^3) */		
	};

	/** A singleton class that holds some static objects needed by SPARK */
	class SPK_PREFIX SPKContext
	{
	public :

		/**
		* @brief Gets the singleton instance
		* @return the instance of the context
		*/
		static inline SPKContext& getInstance();

		/** @brief Releases all dynamic data held by the context */
		void release();

		/**
		* @brief Gets the default zone
		* The default zone is a shared point located at position (0.0f,0.0f,0.0f).
		* @return the default zone
		*/
		const Ref<Zone>& getDefaultZone();

		/**
		* @brief Gets a random value within the interval [min,max[
		* @param min : the minimum bound of the interval (inclusive)
		* @param max : the maximum bound of the interval (exclusive)
		* @return a random number within [min,max[
		*/
		template<typename T>
		inline T generateRandom(const T& min,const T& max);

	private :

		static SPKContext instance;

		Ref<Zone> defaultZone;
		unsigned int randomSeed;

		SPKContext();
		~SPKContext();

		SPKContext(const SPKContext&); // Not used
		SPKContext& operator=(const SPKContext&); // Not used
	};

	inline SPKContext& SPKContext::getInstance()
	{
		return instance;
	}

	template<typename T>
	inline T SPKContext::generateRandom(const T& min,const T& max)
    {
		// optimized standard minimal
		long tmp0 = 16807L * (randomSeed & 0xFFFFL);
        long tmp1 = 16807L * (randomSeed >> 16);
        long tmp2 = (tmp0 >> 16) + tmp1;
        tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

		// correction of the error
        if ((tmp0 & 0x80000000L) != 0)
			tmp0 = (tmp0 + 1) & 0x7FFFFFFFL;

		randomSeed = tmp0;

		// find a random number in the interval
        return static_cast<T>(min + ((randomSeed - 1) / 2147483646.0) * (max - min));
    }

	extern SPK_PREFIX void registerCoreForLoading();
}

#endif
