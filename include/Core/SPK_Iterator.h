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

#ifndef H_SPK_ITERATOR
#define H_SPK_ITERATOR

namespace SPK
{
	/** @brief A generic class to iterate over a collection of particles */
	template<typename T>
	class Iterator
	{
	public :

		/**
		* @brief Constructor of iterator
		* The iterator points at the beginning of the collection
		* @param t : the collection over which to iterate
		*/
		Iterator(T& t);

		/**
		* @brief Gets the particle on which points by the iterator
		* @return the particle on which points by the iterator
		*/
		inline Particle& operator*() const;

		/**
		* @brief Allows access to the interface of the particle on which points by the iterator
		* @return the particle on which points by the iterator
		*/
		inline Particle* operator->() const;

		/**
		* @brief pre-increments the position of the iterator
		* @return the incremented iterator
		*/
		inline Iterator& operator++();

		/**
		* @brief post-increments the position of the iterator
		* @return the incremented iterator
		*/
		inline Iterator operator++(int);

		/**
		* @brief Checks whether the iterator has reached the end of the collection
		* @return true if the iterator has reached the end of the collection, false if not
		*/
		inline bool end() const;

	private :

		mutable Particle particle;
	};

	/** @brief A generic class to iterate over a constant collection of particles */
	template<typename T>
	class ConstIterator
	{
	public :

		/**
		* @brief Constructor of iterator
		* The iterator points at the beginning of the collection
		* @param t : the collection over which to iterate
		*/
		ConstIterator(const T& t);

		/**
		* @brief Gets the particle on which points by the iterator
		* @return the particle on which points by the iterator
		*/
		inline const Particle& operator*() const;

		/**
		* @brief Allows access to the interface of the particle on which points by the iterator
		* @return the particle on which points by the iterator
		*/
		inline const Particle* operator->() const;

		/**
		* @brief pre-increments the position of the iterator
		* @return the incremented iterator
		*/
		inline ConstIterator& operator++();

		/**
		* @brief post-increments the position of the iterator
		* @return the incremented iterator
		*/
		inline ConstIterator operator++(int);

		/**
		* @brief Checks whether the iterator has reached the end of the collection
		* @return true if the iterator has reached the end of the collection, false if not
		*/
		inline bool end() const;

	private :

		const Particle particle;
	};

	typedef Iterator<Group> GroupIterator;				/**< @brief Iterator of a Group */
	typedef ConstIterator<Group> ConstGroupIterator;	/**< @brief Constant Iterator of a Group */

	template<typename T>
	inline bool operator!=(const Iterator<T>& it0,const Iterator<T>& it1) { return it0->getIndex() != it1->getIndex(); }
	template<typename T>
	inline bool operator!=(const ConstIterator<T>& it0,const ConstIterator<T>& it1) { return it0->getIndex() != it1->getIndex(); }
	template<typename T>
	inline bool operator!=(const ConstIterator<T>& it0,const Iterator<T>& it1) { return it0->getIndex() != it1->getIndex(); }
	template<typename T>
	inline bool operator!=(const Iterator<T>& it0,const ConstIterator<T>& it1) { return it0->getIndex() != it1->getIndex(); }

	inline Iterator<Group>::Iterator(Group& group) :
		particle(group,0)
	{
		SPK_ASSERT(group.getSystem().isInitialized(),"Iterator::Iterator(Group&) - An iterator from an uninitialized group cannot be retrieved");
	}

	inline Particle& Iterator<Group>::operator*() const
	{ 
		return particle;
	}
		
	inline Particle* Iterator<Group>::operator->() const 
	{
		return &particle;
	}

	inline Iterator<Group>& Iterator<Group>::operator++()
	{
		++particle.index;
		return *this;
	}

	inline Iterator<Group> Iterator<Group>::operator++(int)
	{
		Iterator tmp(*this);
		return ++tmp;
	}

	inline bool Iterator<Group>::end() const
	{ 
		return particle.index >= particle.group.getNbParticles();
	}

	inline ConstIterator<Group>::ConstIterator(const Group& group) :
		particle(const_cast<Group&>(group),0)
	{
		SPK_ASSERT(group.getSystem().isInitialized(),"ConstIterator::ConstIterator(Group&) - An const iterator from a uninitialized group cannot be retrieved");	
	}

	inline const Particle& ConstIterator<Group>::operator*() const
	{ 
		return particle;
	}
		
	inline const Particle* ConstIterator<Group>::operator->() const 
	{
		return &particle;
	}

	inline ConstIterator<Group>& ConstIterator<Group>::operator++()
	{
		++particle.index;
		return *this;
	}

	inline ConstIterator<Group> ConstIterator<Group>::operator++(int)
	{
		ConstIterator tmp(*this);
		return ++tmp;
	}

	inline bool ConstIterator<Group>::end() const
	{ 
		return particle.index >= particle.group.getNbParticles();
	}
}

#endif
