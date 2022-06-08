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

#ifndef H_SPK_GRAPHINTERPOLATOR
#define H_SPK_GRAPHINTERPOLATOR

#include <cmath> // for std::abs
#include <vector>

namespace SPK
{
	/**
	* @brief An entry in the interpolator graph
	*
	* See the Interpolator description for more information
	*/
	template<typename T>
	struct InterpolatorEntry
	{
		unsigned int id;

		float x;	/**< x value of this entry */
		T y0;		/**< y first value of this entry */
		T y1;		/**< y second value of this entry */

		/** @brief Default constructor of interpolator entry */
		InterpolatorEntry() : x(0),y0(),y1() {}

		/**
		* @brief Constructs an interpolator entry with y0 and y1 having the same value
		* @param x : the x value
		* @param y : the y value (value of y0 and y1)
		*/
		InterpolatorEntry(float x, T y) : x(x),y0(y),y1(y) {}

		/**
		* @brief Constructs and interpolator entry
		* @param x : the x value
		* @param y0 : the y0 value
		* @param y1 : the y1 value
		*/
		InterpolatorEntry(float x, T y0, T y1) : x(x),y0(y0),y1(y1) {}
	};

	/**
	* @class Interpolator
	* @brief An interpolator that offers flexible control over particle parameters
	*
	* An interpolator is created for each parameter of a model which is set as interpolated.<br>
	* The user can get the interpolator of a parameter for a given model by calling Model::getInterpolator(Param).<br>
	* <br>
	* An interpolator can use several types of value to interpolate a given parameter :
	* <ul>
	* <li>the lifetime of a particle : it is defined in a range between 0 and 1, 0 being the birth of the particle and 1 being its death</li>
	* <li>the age of a particle</li>
	* <li>the value of another parameter of a particle (which can be any of the parameters)</li>
	* <li>the square norm of the velocity of a particle</li>
	* </ul>
	* Here is a description of how an interpolator works :<br>
	* <br>
	* Internally an interpolator holds a list of entries which defines a 2D graph. The entries are sorted internally along the x axis.<br>
	* Each entry have a unique x value and 2 y values (although both y can have the same value).<br>
	* <br>
	* The x defines the value that will be used to interpolate the parameter value. This value depends on the type set of the interpolator.<br>
	* For instance, if the type is INTERPOLATOR_AGE, the current x value will be the age of the particle.<br>
	* <br>
	* Knowing the current x value, the interpolator interpolates the y value in function of the entries y values.<br>
	* An interpolator holds 2 curves : the y0 one and the y1 one.<br>
	* Each particle is given a random value between 0 and 1 which defines where between the y0 and the y1 curve the interpolated y value will be.<br>
	* The final interpolated y value will be the value of the interpolated particle parameter for this frame.<br>
	* <br>
	* Moreover the graph can loop or not :
	* <ul>
	* <li>If the graph does not loop, the current x value is clamped between the minimum x and the maximum x of the graph.</li>
	* <li>If the graph loops, the current x is recomputed to fit in the range between the minimum x and the maximum x of the graph.</li>
	* </ul>
	* Finally, it is possible to set a variation in the offset and the scale of the current x computed :<br>
	* Each particle is given an offset and a scale to compute its current x depending on the variations set. The formula to compute the final current x is the following :<br>
	* <i>final current x = (current x + offset) * scale</i><br>
	* offset being randomly generated per particle in <i>[-offsetXVariation,+offsetXVariation]</i><br>
	* scale being randomly generated per particle in <i>1.0 + [-scaleXVariation,+scaleXVariation]</i><br>
	* <br>
	* The default values of the interpolator are the following :
	* <ul>
	* <li>type : INTERPOLATOR_LIFETIME</li>
	* <li>offset x variation : 0.0</li>
	* <li>scale x variation : 0.0</li>
	* </ul>
	*/
	template<typename T>
	class GraphInterpolator : public Interpolator<T>
	{
	public :
		typedef typename Arg<T>::type argType;

		static  Ref<GraphInterpolator<T> > create();

		/////////////////////////////
		// Interpolator parameters //
		/////////////////////////////

		/**
		* @brief Sets the value used to interpolate
		*
		* See the class description for more information.<br>
		* Note that the argument param is only used when the type is INTERPOLATOR_PARAM.
		*
		* @param type : the type of value used to interpolate
		* @param param : the parameter used to interpolate when the type is INTERPOLATOR_PARAM.
		*/
		void setType(InterpolationType type, Param param);
		void setType(InterpolationType type);

		void setParam(Param param);

		/**
		* @brief Gets the type of value used to interpolate
		* @return the type of value used to interpolate
		*/
		InterpolationType getType() const;

		/**
		* @brief Gets the parameter used to interpolate
		*
		* Note that the parameter is only used if the type is INTERPOLATOR_PARAM
		*
		* @return the parameter used to interpolate
		*/
		Param getInterpolatorParam() const;

		/**
		* @brief Enables or disables the looping of the graph
		*
		* The range of the graph is defined between the entry with the minimum x and the entry with the maximum y.<br>
		* If the looping is disabled, the x are clamped to the range.<br>
		* If the looping is enabled, the value of x is reported in the range. It is better that the xmin and xmax have
		* the same y values so that the graph tiles perfectly.
		*
		* @param loop : true to enabled the looping, false to disable it
		*/
		void enableLooping(bool loop);

		/**
		* @brief Tells whether the looping is enabled or not
		* @return true if the looping is enabled, false if not
		*/
		bool isLoopingEnabled() const;

		/**
		* @brief Sets the offset variation in x
		*
		* See the class description for more information
		*
		* @param offsetXVariation : the offset variation in x
		*/
		void setOffsetXVariation(float offsetXVariation);

		/**
		* @brief Gets the offset variation along x
		* @return the offset variation along x
		*/
		float getOffsetXVariation() const;

		/**
		* @brief Sets the scale variation in x
		*
		* See the class description for more information
		*
		* @param scaleXVariation : the scale variation in x
		*/
		void setScaleXVariation(float scaleXVariation);

		/**
		* @brief Gets the scale variation along x
		* @return the scale variation along x
		*/
		float getScaleXVariation() const;

		/////////////////////////
		// Operations on graph //
		/////////////////////////

		/**
		* @brief Adds an entry to the graph
		* @param x : the x of the entry to add
		* @param y : the y of the entry to add (y0 and y1 are set to y)
		* @return true if the entry has been added to the graph, false if not (the graph already contains an entry with the same x)
		*/
		bool addEntry(float x, T y);

		/**
		* @brief Adds an entry to the graph
		* @param x : the x of the entry to add
		* @param y0 : the y0 of the entry to add
		* @param y1 : the y1 of the entry to add
		* @return true if the entry has been added to the graph, false if not (the graph already contains an entry with the same x)
		*/
		bool addEntry(float x, T y0, T y1);

		/** @brief Clears the graph (removes all the entries) */
		void clearGraph();

		/** @brief Returns the number of entries in the graph */
		unsigned int getNbEntries() const;

		/** @brief Removes an entry specified by index */
		void removeEntry(unsigned int id);

	public :
		void createEntry();
		void setX(unsigned id, float x);
		float getX(unsigned int id) const;
		void setY0(unsigned id, argType y);
		argType getY0(unsigned int id) const;
		void setY1(unsigned id, argType y);
		argType getY1(unsigned int id) const;

	public :
		spark_description(GraphInterpolator, Interpolator<T>)
		(
			spk_attribute(InterpolationType, interpolationType, setType, getType);
			spk_attribute(Param, parameter, setParam, getInterpolatorParam);
			spk_attribute(bool, loop, enableLooping, isLoopingEnabled);
			spk_attribute(float, scale, setScaleXVariation, getScaleXVariation);
			spk_attribute(float, offset, setOffsetXVariation, getOffsetXVariation);
			spk_structure(graph, createEntry, removeEntry, clearGraph, getNbEntries)
			(
				spk_field(float, x, setX, getX);
				spk_field(T, y0, setY0, getY0);
				spk_field(T, y1, setY1, getY1);
			);
		);

	private :

		static const size_t NB_DATA = 3;
		static const size_t OFFSET_X_DATA_INDEX = 0;
		static const size_t SCALE_X_DATA_INDEX = 1;
		static const size_t RATIO_Y_DATA_INDEX = 2;

		std::vector<InterpolatorEntry<T> > graph;
		std::vector<unsigned int> sortedGraph;

		InterpolationType type;
		Param param;
		bool loopingEnabled;

		float scaleXVariation;
		float offsetXVariation;

		// methods to compute X
		typedef float (GraphInterpolator<T>::*computeXFn)(const Particle&) const;
		static computeXFn COMPUTE_X_FN[4];

		float computeXLifeTime(const Particle& particle) const;
		float computeXAge(const Particle& particle) const;
		float computeXParam(const Particle& particle) const;
		float computeXVelocity(const Particle& particle) const;

		GraphInterpolator<T>();
		GraphInterpolator<T>(const GraphInterpolator<T>& interpolator);

		virtual void createData(DataSet& dataSet,const Group& group) const;

		virtual void interpolate(T* data, Group& group, DataSet* dataSet) const;
		virtual void init(T& data, Particle& particle, DataSet* dataSet) const;

		void sortGraph(unsigned int start);
		void swapEntries(unsigned int id1, unsigned int id2);
		void swapSortedEntries(unsigned int id1, unsigned int id2);

		// Interpolates between the two ys of an entry function of the ratioY
		void interpolateEntry(T& result,const InterpolatorEntry<T>& entry,float ratio) const;
		// Interpolates the data of a single particle function of the graph
		void interpolateParticle(T& data,const Particle& particle,float offsetX,float scaleX,float ratioY) const;
	};

	typedef GraphInterpolator<Color> ColorGraphInterpolator;
	typedef GraphInterpolator<float> FloatGraphInterpolator;

	spark_description_specialization( ColorGraphInterpolator );
	spark_description_specialization( FloatGraphInterpolator );

	template<typename T>
	typename GraphInterpolator<T>::computeXFn GraphInterpolator<T>::COMPUTE_X_FN[4] =
	{
		&GraphInterpolator<T>::computeXLifeTime,
		&GraphInterpolator<T>::computeXAge,
		&GraphInterpolator<T>::computeXParam,
		&GraphInterpolator<T>::computeXVelocity,
	};

	template<typename T>
	inline Ref<GraphInterpolator<T> > GraphInterpolator<T>::create()
	{
		return SPK_NEW(GraphInterpolator<T>);
	}

	template<typename T>
	GraphInterpolator<T>::GraphInterpolator() :
		Interpolator<T>(true),
		type(INTERPOLATOR_LIFETIME),
		param(PARAM_SCALE),
		scaleXVariation(0.0f),
		offsetXVariation(0.0f),
		loopingEnabled(false)
	{}

	template<typename T>
	GraphInterpolator<T>::GraphInterpolator(const GraphInterpolator<T>& interpolator) :
		Interpolator<T>(interpolator),
		graph(interpolator.graph),
		sortedGraph(interpolator.sortedGraph),
		type(interpolator.type),
		param(interpolator.param),
		scaleXVariation(interpolator.scaleXVariation),
		offsetXVariation(interpolator.scaleXVariation),
		loopingEnabled(interpolator.loopingEnabled)
	{}

	template<typename T>
	inline void GraphInterpolator<T>::setType(InterpolationType type,Param param)
	{
		this->type = type;
		this->param = param;
	}

	template<typename T>
	inline void GraphInterpolator<T>::setType(InterpolationType t)
	{
		type = t;
	}

	template<typename T>
	inline void GraphInterpolator<T>::setParam(Param p)
	{
		param = p;
	}

	template<typename T>
	inline InterpolationType GraphInterpolator<T>::getType() const
	{
		return type;
	}

	template<typename T>
	inline Param GraphInterpolator<T>::getInterpolatorParam() const
	{
		return param;
	}

	template<typename T>
	inline void GraphInterpolator<T>::enableLooping(bool loop)
	{
		loopingEnabled = loop;
	}

	template<typename T>
	inline bool GraphInterpolator<T>::isLoopingEnabled() const
	{
		return loopingEnabled;
	}

	template<typename T>
	inline void GraphInterpolator<T>::setOffsetXVariation(float offsetXVariation)
	{
		this->offsetXVariation = offsetXVariation;
	}

	template<typename T>
	inline float GraphInterpolator<T>::getOffsetXVariation() const
	{
		return offsetXVariation;
	}

	template<typename T>
	void GraphInterpolator<T>::setScaleXVariation(float scaleXVariation)
	{
		if (std::abs(scaleXVariation) >= 1.0)
			SPK_LOG_WARNING("GraphInterpolator<T>::setScaleXVariation(float) - A scale variation with an absolute value >= 1.0 may reverse the interpolation");
		this->scaleXVariation = scaleXVariation;
	}

	template<typename T>
	inline float GraphInterpolator<T>::getScaleXVariation() const
	{
		return scaleXVariation;
	}

	template<typename T>
	inline void GraphInterpolator<T>::swapEntries(unsigned int id1, unsigned int id2)
	{
		if(id1 == id2)
			return;

		unsigned int sId1 = graph[id1].id;
		unsigned int sId2 = graph[id2].id;
		InterpolatorEntry<T> tmp = graph[id1];
		graph[id1] = graph[id2];
		graph[id2] = tmp;
		graph[id1].id = sId2;
		graph[id2].id = sId1;
		sortedGraph[sId1] = id2;
		sortedGraph[sId2] = id1;
	}

	template<typename T>
	inline void GraphInterpolator<T>::swapSortedEntries(unsigned int sId1, unsigned int sId2)
	{
		if(sId1 == sId2)
			return;

		unsigned int id1 = sortedGraph[sId1];
		unsigned int id2 = sortedGraph[sId2];
		sortedGraph[sId1] = id2;
		sortedGraph[sId2] = id1;
		graph[id1].id = sId2;
		graph[id2].id = sId1;
	}

	template<typename T>
	inline void GraphInterpolator<T>::sortGraph(unsigned int start)
	{
		for(unsigned int i = start; i < sortedGraph.size(); i++)
			for(unsigned int k = i; k >= 1; k--)
			{
				unsigned int a = sortedGraph[k];
				unsigned int b = sortedGraph[k-1];
				if(graph[a].x < graph[b].x)
					swapSortedEntries(k, k-1);
				else
					break;
			}
	}

	template<typename T>
	inline unsigned int GraphInterpolator<T>::getNbEntries() const
	{
		return graph.size();
	}

	template<typename T>
	inline void GraphInterpolator<T>::removeEntry(unsigned int id)
	{
		if(id >= graph.size())
			return;

		description::graph::elementRemoved(this, id);
		swapEntries(id, graph.size() - 1);
		unsigned int sortedId = graph[graph.size() - 1].id;
		graph.erase(graph.begin() + (graph.size() - 1));
		sortedGraph.erase(sortedGraph.begin() + sortedId);
		for(unsigned int i = sortedId; i < sortedGraph.size(); i++)
			graph[sortedGraph[i]].id--;
	}

	template<typename T>
	inline void GraphInterpolator<T>::createEntry()
	{
		InterpolatorEntry<T> entry(0, 0);
		if(graph.size() > 0)
		{
			unsigned int last = sortedGraph[sortedGraph.size() - 1];
			entry.x = graph[last].x + 1;
		}
		entry.id = sortedGraph.size();
		graph.push_back(entry);
		sortedGraph.push_back(graph.size() - 1);
	}

	template<typename T>
	inline void GraphInterpolator<T>::setX(unsigned id, float x)
	{
		graph[id].x = x;
		sortGraph(graph[id].id);
	}

	template<typename T>
	inline float GraphInterpolator<T>::getX(unsigned int id) const
	{
		return graph[id].x;
	}

	template<typename T>
	inline void GraphInterpolator<T>::setY0(unsigned id, argType y)
	{
		graph[id].y0 = y;
	}

	template<typename T>
	inline typename GraphInterpolator<T>::argType GraphInterpolator<T>::getY0(unsigned int id) const
	{
		return graph[id].y0;
	}

	template<typename T>
	inline void GraphInterpolator<T>::setY1(unsigned id, argType y)
	{
		graph[id].y1 = y;
	}

	template<typename T>
	inline typename GraphInterpolator<T>::argType GraphInterpolator<T>::getY1(unsigned int id) const
	{
		return graph[id].y1;
	}

	template<typename T>
	inline bool GraphInterpolator<T>::addEntry(float x, T y)
	{
		return addEntry(x, y, y);
	}

	template<typename T>
	inline bool GraphInterpolator<T>::addEntry(float x, T y0, T y1)
	{
		createEntry();
		graph[graph.size() - 1].x = x;
		graph[graph.size() - 1].y0 = y0;
		graph[graph.size() - 1].y1 = y1;
		sortGraph(sortedGraph.size() - 1);
		return true;
	}

	template<typename T>
	inline void GraphInterpolator<T>::clearGraph()
	{
		description::graph::elementsCleared(this);
		graph.clear();
		sortedGraph.clear();
	}

	template<typename T>
	inline float GraphInterpolator<T>::computeXLifeTime(const Particle& particle) const
	{
		return 1.0f - particle.getEnergy();
	}

	template<typename T>
	inline float GraphInterpolator<T>::computeXAge(const Particle& particle) const
	{
		return particle.getAge();
	}

	template<typename T>
	inline float GraphInterpolator<T>::computeXParam(const Particle& particle) const
	{
		return particle.getParam(param);
	}

	template<typename T>
	inline float GraphInterpolator<T>::computeXVelocity(const Particle& particle) const
	{
		return particle.velocity().getSqrNorm();
	}

	template<typename T>
	void GraphInterpolator<T>::createData(DataSet& dataSet,const Group& group) const
	{
		dataSet.init(NB_DATA);
		FloatArrayData* offsetXDataPtr = SPK_NEW(FloatArrayData,group.getCapacity(),1);
		FloatArrayData* scaleXDataPtr = SPK_NEW(FloatArrayData,group.getCapacity(),1);
		FloatArrayData* ratioYDataPtr = SPK_NEW(FloatArrayData,group.getCapacity(),1);

		dataSet.setData(OFFSET_X_DATA_INDEX,offsetXDataPtr);
		dataSet.setData(SCALE_X_DATA_INDEX,scaleXDataPtr);
		dataSet.setData(RATIO_Y_DATA_INDEX,ratioYDataPtr);

		// Inits the newly created data
		for (size_t i = 0; i < group.getNbParticles(); ++i)
		{
			(*offsetXDataPtr)[i] = SPK_RANDOM(-offsetXVariation,offsetXVariation);
			(*scaleXDataPtr)[i] = 1.0f + SPK_RANDOM(-scaleXVariation,scaleXVariation);
			(*ratioYDataPtr)[i] = SPK_RANDOM(0.0f,1.0f);
		}
	}

	template<typename T>
	inline void GraphInterpolator<T>::interpolateEntry(T& result,const InterpolatorEntry<T>& entry,float ratio) const
	{
		this->interpolateParam(result,entry.y0,entry.y1,ratio);
	}

	template<typename T>
	void GraphInterpolator<T>::interpolateParticle(T& data, const Particle& particle, float offsetX, float scaleX, float ratioY) const
	{
		float currentX = (this->*GraphInterpolator<T>::COMPUTE_X_FN[type])(particle);
		currentX = (currentX + offsetX) * scaleX;

		// Recompute X if looping
		if(loopingEnabled)
		{
			float firstX = graph[*sortedGraph.begin()].x;
			float lastX = graph[*sortedGraph.rbegin()].x;
			if(lastX - firstX <= 0.000001f)
			{
				interpolateEntry(data, *(graph.begin()), ratioY);
				return;
			}
			float normalizedX = currentX / (lastX - firstX);
			normalizedX -= (int)normalizedX;
			if(normalizedX < 0)
				normalizedX += 1;
			currentX = normalizedX * (lastX - firstX) + firstX;
		}

		// Find whether the key is in the graph
		if(currentX <= graph[*sortedGraph.begin()].x)
		{
			// Before first key
			interpolateEntry(data, graph[*sortedGraph.begin()], ratioY);
		}
		else if(currentX >= graph[*sortedGraph.rbegin()].x)
		{
			// After last key
			interpolateEntry(data, graph[*sortedGraph.rbegin()], ratioY);
		}
		else
		{
			// Inside 2 keys
			unsigned int after = 0;
			while(after < sortedGraph.size() && graph[sortedGraph[after]].x <= currentX)
				after++;

			const InterpolatorEntry<T>& nextEntry = graph[sortedGraph[after]];
			const InterpolatorEntry<T>& previousEntry = graph[sortedGraph[after-1]];
			float ratioX = (currentX - previousEntry.x) / (nextEntry.x - previousEntry.x);
			T y0,y1;

			interpolateEntry(y0, previousEntry, ratioY);
			interpolateEntry(y1, nextEntry, ratioY);
			this->interpolateParam(data, y0, y1, ratioX);
		}
	}

	template<typename T>
	void GraphInterpolator<T>::interpolate(T* data, Group& group, DataSet* dataSet) const
	{
		SPK_ASSERT(!graph.empty(),"GraphInterpolator<T>::interpolate(T*,Group&,DataSet*) const - The graph of the interpolator is empty. Cannot interpolate");

		FloatArrayData& offsetXData = SPK_GET_DATA(FloatArrayData,dataSet,OFFSET_X_DATA_INDEX);
		FloatArrayData& scaleXData = SPK_GET_DATA(FloatArrayData,dataSet,SCALE_X_DATA_INDEX);
		FloatArrayData& ratioYData = SPK_GET_DATA(FloatArrayData,dataSet,RATIO_Y_DATA_INDEX);

		for (GroupIterator particleIt(group); !particleIt.end(); ++particleIt)
		{
			size_t index = particleIt->getIndex();
			interpolateParticle(data[index],*particleIt,offsetXData[index],scaleXData[index],ratioYData[index]);
		}
	}

	template<typename T>
	void GraphInterpolator<T>::init(T& data,Particle& particle,DataSet* dataSet) const
	{
		SPK_ASSERT(!graph.empty(),"GraphInterpolator<T>::init(T&,Particle&,DataSet*) const - The graph of the interpolator is empty. Cannot interpolate");

		size_t index = particle.getIndex();
		FloatArrayData& offsetXData = SPK_GET_DATA(FloatArrayData,dataSet,OFFSET_X_DATA_INDEX);
		FloatArrayData& scaleXData = SPK_GET_DATA(FloatArrayData,dataSet,SCALE_X_DATA_INDEX);
		FloatArrayData& ratioYData = SPK_GET_DATA(FloatArrayData,dataSet,RATIO_Y_DATA_INDEX);

		offsetXData[index] = SPK_RANDOM(-offsetXVariation,offsetXVariation);
		scaleXData[index] = 1.0f + SPK_RANDOM(-scaleXVariation,scaleXVariation);
		ratioYData[index] = SPK_RANDOM(0.0f,1.0f);
		interpolateParticle(data,particle,offsetXData[index],scaleXData[index],ratioYData[index]);
	}
}

#endif

