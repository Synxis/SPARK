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


#ifndef H_SPK_INTERPOLATOR
#define H_SPK_INTERPOLATOR

#include "Core/SPK_DEF.h"

namespace SPK
{
	class Particle;

	/**
	*
	* @since 1.05.00
	*/
	enum InterpolationType
	{
		INTERPOLATOR_LIFETIME,
		INTERPOLATOR_AGE,
		INTERPOLATOR_PARAM,
		INTERPOLATOR_VELOCITY,
	};

	/**
	*
	* @since 1.05.00
	*/
	class SPK_PREFIX Interpolator
	{
	friend class Particle;

	public :

		/**
		*
		* @since 1.05.00
		*/
		struct Entry
		{
			float x;	/**< x value of this entry */
			float y0;	/**< y first value of this entry */
			float y1;	/**< y second value of this entry */

			Entry() : x(0.0f),y0(0.0f),y1(0.0f) {}
			Entry(float x) : x(x) {}
			Entry(float x,float y) : x(x),y0(y),y1(y) {}
			Entry(float x,float y0,float y1) : x(x),y0(y0),y1(y1) {}
		};

		/////////////////
		// Constructor //
		/////////////////

		Interpolator();

		/////////////
		// Setters //
		/////////////

		inline void setType(InterpolationType type,ModelParam param = static_cast<ModelParam>(0));
		inline void enableLooping(bool loop);
		inline void setScaleXVariation(float scaleXVariation);
		inline void setOffsetXVariation(float offsetXVariation);

		/////////////
		// Getters //
		/////////////

		inline InterpolationType getType() const;
		inline ModelParam getInterpolatorParam() const;
		inline bool isLoopingEnabled() const;
		inline float getScaleXVariation() const;
		inline float getOffsetXVariation() const;

		inline std::set<Entry>& getGraph();
		inline const std::set<Entry>& getGraph() const;

		///////////////
		// Interface //
		///////////////

		inline bool addEntry(const Entry& entry);
		inline bool addEntry(float x,float y);
		inline bool addEntry(float x,float y0,float y1);
		inline void clearGraph();

		void generateSinCurve(float period,float amplitudeMin,float amplitudeMax,float offsetX,float offsetY,float startX,unsigned int length,unsigned int nbSamples);
		void generatePolyCurve(float constant,float linear,float quadratic,float cubic,float startX,float endX,unsigned int nbSamples);

	private :

		std::set<Entry> graph;

		InterpolationType type;
		ModelParam param;
		bool loopingEnabled;

		float scaleXVariation;
		float offsetXVariation;

		float interpolate(const Particle& particle,ModelParam interpolatedParam,float ratioY,float offsetX,float scaleX);
		inline float interpolateY(const Entry& entry,float ratio); 

		// methods to compute X
		typedef float (Interpolator::*computeXFn)(const Particle&) const;
		static computeXFn COMPUTE_X_FN[4];

		float computeXLifeTime(const Particle& particle) const;
		float computeXAge(const Particle& particle) const;
		float computeXParam(const Particle& particle) const;
		float computeXVelocity(const Particle& particle) const;
	};

	
	inline void Interpolator::setType(InterpolationType type,ModelParam param)
	{
		this->type = type;
		this->param = param;
	}

	inline void Interpolator::enableLooping(bool loop)
	{
		loopingEnabled = loop;
	}

	inline void Interpolator::setScaleXVariation(float scaleXVariation)
	{
		this->scaleXVariation = scaleXVariation;
	}

	inline void Interpolator::setOffsetXVariation(float offsetXVariation)
	{
		this->offsetXVariation = offsetXVariation;
	}

	inline InterpolationType Interpolator::getType() const
	{
		return type;
	}

	inline ModelParam Interpolator::getInterpolatorParam() const
	{
		return param;
	}

	inline bool Interpolator::isLoopingEnabled() const
	{
		return loopingEnabled;
	}

	inline float Interpolator::getScaleXVariation() const
	{
		return scaleXVariation;
	}

	inline float Interpolator::getOffsetXVariation() const
	{
		return offsetXVariation;
	}

	inline std::set<Interpolator::Entry>& Interpolator::getGraph()
	{
		return graph;
	}

	inline const std::set<Interpolator::Entry>& Interpolator::getGraph() const
	{
		return graph;
	}

	inline bool Interpolator::addEntry(const Entry& entry)
	{
		return graph.insert(entry).second;
	}

	inline bool Interpolator::addEntry(float x,float y)
	{
		return addEntry(Entry(x,y));
	}

	inline bool Interpolator::addEntry(float x,float y0,float y1)
	{
		return addEntry(Entry(x,y0,y1));
	}

	inline void Interpolator::clearGraph()
	{
		graph.clear();
	}

	inline float Interpolator::interpolateY(const Entry& entry,float ratio)
	{
		return entry.y0 + (entry.y1 - entry.y0) * ratio;
	}

	/////////////////////////////////////////////////////////////
	// Functions to sort the entries on the interpolator graph //
	/////////////////////////////////////////////////////////////

	inline bool operator<(const Interpolator::Entry& entry0,const Interpolator::Entry& entry1)
	{
		return entry0.x < entry1.x;
	}

	inline bool operator==(const Interpolator::Entry& entry0,const Interpolator::Entry& entry1)
	{
		return entry0.x == entry1.x;
	}
}

#endif

