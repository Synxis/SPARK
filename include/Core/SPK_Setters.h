//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 :                                                    //
//  - Julien Fryer - julienfryer@gmail.com				                        //
//  - Thibault Lescoat - info-tibo@orange.fr                                    //
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

#ifndef H_SPK_SETTERS
#define H_SPK_SETTERS

namespace SPK
{
	/**
	* @brief Defines the type of a setter based on its class and its parameter type
	*
	* A setter is a member function setting a value in an object. It have no return value.
	* For pair, a setter takes 2 parameters; for triplet, 3 parameters, and for quadruplet,
	* 4 parameters. Parameters are passed in order: myPair.value1 is the first parameter,
	* myPair.value2 is the second parameter, and so on.
	*
	* Setters taking an array have only one parameter, the array itself.
	*/
	template<typename T, class obj>
	struct Setter
	{
		/**
		* @brief Parameter type of the setter
		*/
		typedef typename Arg<T>::type paramType;

		/**
		* @brief Type of the setter
		*/
		typedef void (obj::* type)(paramType);

		/**
		* @brief Type of a free-function setter
		*/
		typedef void (* freefunc)(obj*, paramType);

		/**
		* @brief Extra parameter
		*/
		template<typename P>
		struct extra
		{
			/**
			* @brief Type of the setter, with one extra parameter
			*/
			typedef void (obj::* type)(P,paramType);

			/**
			* @brief Calls a setter
			*/
			template<type func>
			static void call(obj* o, P e, paramType v)
			{
				(o->*func)(e,v);
			}
		};

		/**
		* @brief Calls a setter
		*
		* @tparam func The setter function, which is &obj::someMemberFunction
		*
		* @param o The object on which the function is called
		* @param v The value to set
		*/
		template<type func>
		static void call(obj* o, paramType v)
		{
			(o->*func)(v);
		}
	};

	/** @brief 2 parameters specialization of SPK::Setter<T,obj> */
	template<typename T, class obj>
	struct Setter<Pair<T>,obj>
	{
		typedef typename Arg<T>::type paramType;

		typedef void (obj::* type)(paramType,paramType);

		typedef void (* freefunc)(obj*, const Pair<paramType>&);

		template<typename P>
		struct extra
		{
			typedef void (obj::* type)(P,paramType,paramType);

			template<type func>
			static void call(obj* o, P e, const Pair<T>& v)
			{
				(o->*func)(e,v.value1,v.value2);
			}
		};

		template<type func>
		static void call(obj* o, const Pair<T>& v)
		{
			(o->*func)(v.value1,v.value2);
		}
	};

	/** @brief 3 parameters specialization of SPK::Setter<T,obj> */
	template<typename T, class obj>
	struct Setter<Triplet<T>,obj>
	{
		typedef typename Arg<T>::type paramType;

		typedef void (obj::* type)(paramType,paramType,paramType);

		typedef void (* freefunc)(obj*, const Triplet<paramType>&);

		template<typename P>
		struct extra
		{
			typedef void (obj::* type)(P,paramType,paramType,paramType);

			template<type func>
			static void call(obj* o, P e, const Triplet<T>& v)
			{
				(o->*func)(e,v.value1,v.value2,v.value3);
			}
		};

		template<type func>
		static void call(obj* o, const Triplet<T>& v)
		{
			(o->*func)(v.value1,v.value2,v.value3);
		}
	};

	/** @brief 4 parameters specialization of SPK::Setter<T,obj> */
	template<typename T, class obj>
	struct Setter<Quadruplet<T>,obj>
	{
		typedef typename Arg<T>::type paramType;

		typedef void (obj::* type)(paramType,paramType,paramType,paramType);

		typedef void (* freefunc)(obj*, const Quadruplet<paramType>&);

		template<typename P>
		struct extra
		{
			typedef void (obj::* type)(P,paramType,paramType,paramType,paramType);

			template<type func>
			static void call(obj* o, P e, const Quadruplet<T>& v)
			{
				(o->*func)(e,v.value1,v.value2,v.value3,v.value4);
			}
		};

		template<type func>
		static void call(obj* o, const Quadruplet<T>& v)
		{
			(o->*func)(v.value1,v.value2,v.value3,v.value4);
		}
	};

	/**
	* @brief A setter bound to an object, ie Delegate
	*/
	template<typename T>
	struct BoundSetter
	{
		/**
		* @brief Function call operator, allows easy use of the delegate.
		*/
		virtual void operator()(typename Arg<T>::type value) const = 0;
	};

	/**
	* @internal
	* @brief Implementation of the BoundSetter interface
	*
	* @tparam T Type of the value, ex: float, Pair<int>, etc...
	* @tparam obj Class in which the setter is declared
	* @tparam func The setter itself
	*/
	template<typename T, class obj, typename Setter<T,obj>::type func>
	class SetterDelegate : public BoundSetter<typename meta::EraseEnum<T>::type>
	{
	public:
		typedef typename meta::EraseEnum<T>::type realType;

		/**
		* @brief Constructor
		*/
		SetterDelegate(obj* o) :
			object(o)
		{
		}

		/**
		* @brief Sets the value by calling the setter
		*/
		void operator()(typename Arg<realType>::type value) const
		{
			// Cast useful for int -> enum conversion (for other cases it is a no-op)
			Setter<T,obj>::template call<func>(const_cast<obj*>(object),*(T*)(&value));
		}

	private:
		obj* object;
	};

	/**
	* @internal
	* @brief Implementation of the BoundSetter interface
	*
	* @tparam T Type of the value, ex: float, Pair<int>, etc...
	* @tparam obj Class to which the free function is applied
	* @tparam func The setter itself (a free function)
	*/
	template<typename T, class obj, typename Setter<T, obj>::freefunc func>
	class FreeSetterDelegate : public BoundSetter<typename meta::EraseEnum<T>::type>
	{
	public:
		typedef typename meta::EraseEnum<T>::type realType;

		/**
		* @brief Constructor
		*/
		FreeSetterDelegate(obj* o) :
			object(o)
		{
		}

		/**
		* @brief Sets the value by calling the setter
		*/
		void operator()(typename Arg<realType>::type value) const
		{
			// Cast useful for int -> enum conversion (for other cases it is a no-op)
			func(object, *(T*)(&value));
		}

	private:
		obj* object;
	};

	/**
	* @internal
	* @brief Implementation of the BoundSetter interface
	*
	* @tparam T Type of the value, ex: float, Pair<int>, etc...
	* @tparam obj Class in which the setter is declared
	* @tparam func The setter itself
	*/
	template<typename T, class obj, typename Setter<T,obj>::template extra<unsigned int>::type func>
	class IndexedSetterDelegate : public BoundSetter<T>
	{
	public:
		/**
		* @brief Constructor
		*/
		IndexedSetterDelegate(obj* o, unsigned int i) :
			object(o),
			index(i)
		{
		}

		/**
		* @brief Sets the value by calling the setter
		*/
		void operator()(typename Arg<T>::type value) const
		{
			Setter<T,obj>::template extra<unsigned int>::template call<func>(const_cast<obj*>(object),index,value);
		}

	private:
		obj* object;
		unsigned int index;
	};
}

#endif
