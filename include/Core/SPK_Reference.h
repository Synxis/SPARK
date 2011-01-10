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

#ifndef H_SPK_REFERENCE
#define H_SPK_REFERENCE

#define SPK_NULL_REF SPK::NullReferenceValue()

namespace SPK
{
	// Hack to allow easy null reference initialization
	class NullReferenceValue {};

	template<typename T> class WeakRef;

	/**
	* @brief A reference on a Referenceable
	*
	*/
	template<typename T>
	class Ref
	{
	public :

		/////////////////////////////
		// Constructors/Destructor //
		/////////////////////////////

		inline Ref() : ptr(NULL) {}
		inline Ref(NullReferenceValue) : 
			ptr(NULL) {}
		inline Ref(const Ref& ref) : 
			ptr(ref.get()) { increment(); }
		template<typename U> inline Ref(U* ptr) : 
			ptr(ptr) { increment(); }
		template<typename U> inline Ref(const Ref<U>& ref) : 
			ptr(ref.get()) { increment(); }
		template<typename U> inline Ref(const WeakRef<U>& ref) : 
			ptr(ref.get()) { increment(); }
		
		inline ~Ref() { 
			decrement(); }

		//////////////////////////
		// Operator overloading //
		//////////////////////////

		Ref& operator=(NullReferenceValue) 
		{ 
			reset(); 
			return *this; 
		}

		Ref& operator=(T* ptr)
		{
			if (*this != ptr)
			{
				decrement();
				this->ptr = ptr;
				increment();
			}

			return *this;
		}

		Ref& operator=(const Ref& ref)
		{
			if (*this != ref)
			{
				decrement();
				ptr = ref.get();
				increment();
			}

			return *this;
		}

		template<typename U> Ref& operator=(const Ref<U>& ref)
		{
			if (*this != ref)
			{
				decrement();
				ptr = ref.get();
				increment();
			}

			return *this;
		}

		template<typename U> Ref& operator=(const WeakRef<U>& ref)
		{
			if (*this != ref)
			{
				decrement();
				ptr = ref.get();
				increment();
			}

			return *this;
		}

		inline T& operator*() const { return *ptr; }
		inline T* operator->() const { return ptr; }
		inline T* get() const { return ptr; }

		operator bool() const { return ptr != 0; }

		void reset() { decrement(); ptr = NULL; }

		template<typename U> Ref<U> cast() const { return Ref<U>(dynamic_cast<U*>(ptr)); }

	private :

		inline void increment() { if (ptr != NULL) ++(ptr->nbReferences); }
		inline void decrement() { if (ptr != NULL && --(ptr->nbReferences) == 0) SPK_DELETE(ptr); }

		T* ptr;
	};

	/**
	* @brief A weak on a Referenceable
	*
	*/
	template<typename T>
	class WeakRef
	{
	public :

		/////////////////////////////
		// Constructors/Destructor //
		/////////////////////////////

		inline WeakRef() : ptr(NULL) {}
		inline WeakRef(NullReferenceValue) : ptr(NULL) {}
		inline WeakRef(const WeakRef& ref) : ptr(ref.get()) {}
		template<typename U> inline WeakRef(U* ptr) : ptr(ptr) {}
		template<typename U> inline WeakRef(const WeakRef<U>& ref) : ptr(ref.get()) {}
		template<typename U> inline WeakRef(const Ref<U>& ref) : ptr(ref.get()) {}

		//////////////////////////
		// Operator overloading //
		//////////////////////////

		WeakRef& operator=(T* ptr)
		{
			this->ptr = ptr;
			return *this;
		}

		WeakRef& operator=(const WeakRef& ref)
		{
			ptr = ref.get();
			return *this;
		}

		template<typename U> WeakRef& operator=(const WeakRef<U>& ref)
		{
			ptr = ref.get();
			return *this;
		}

		template<typename U> WeakRef& operator=(const Ref<U>& ref)
		{
			ptr = ref.get();
			return *this;
		}

		inline T& operator*() const { return *ptr; }
		inline T* operator->() const { return ptr; }
		inline T* get() const { return ptr; }

		operator bool() const { return ptr != 0; }

		void reset() { ptr = NULL; }

		template<typename U> WeakRef<U> cast() const { return WeakRef<U>(dynamic_cast<U*>(ptr)); }

	private :

		T* ptr;
	};

	template<typename T,typename U> bool operator==(const Ref<T>& ref0,const Ref<U>& ref1) { return ref0.get() == ref1.get(); }
	template<typename T,typename U> bool operator==(const Ref<T>& ref,U* ptr) { return ref.get() == ptr; }
	template<typename T,typename U> bool operator==(T* ptr,const Ref<U>& ref) { return ref.get() == ptr; }
	template<typename T,typename U> bool operator==(const WeakRef<T>& ref0,const WeakRef<U>& ref1) { return ref0.get() == ref1.get(); }
	template<typename T,typename U> bool operator==(const WeakRef<T>& ref,U* ptr) { return ref.get() == ptr; }
	template<typename T,typename U> bool operator==(T* ptr,const WeakRef<U>& ref) { return ref.get() == ptr; }
	template<typename T,typename U> bool operator<(const Ref<T>& ref0,const Ref<U>& ref1) { return ref0.get() < ref1.get(); }

	template<typename T,typename U> bool operator!=(const Ref<T>& ref0,const Ref<U>& ref1) { return ref0.get() != ref1.get(); }
	template<typename T,typename U> bool operator!=(const Ref<T>& ref,U* ptr) { return ref.get() != ptr; }
	template<typename T,typename U> bool operator!=(T* ptr,const Ref<U>& ref) { return ref.get() != ptr; }
	template<typename T,typename U> bool operator!=(const WeakRef<T>& ref0,const WeakRef<U>& ref1) { return ref0.get() != ref1.get(); }
	template<typename T,typename U> bool operator!=(const WeakRef<T>& ref,U* ptr) { return ref.get() != ptr; }
	template<typename T,typename U> bool operator!=(T* ptr,const WeakRef<U>& ref) { return ref.get() != ptr; }
	template<typename T,typename U> bool operator<(const WeakRef<T>& ref0,const WeakRef<U>& ref1) { return ref0.get() < ref1.get(); }
}

#endif

