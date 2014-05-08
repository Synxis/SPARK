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

#ifndef H_SPK_FUNCTION
#define H_SPK_FUNCTION

namespace SPK
{
	/** @brief Helper for Getters class */
	template<typename T, class obj>
	struct GetterBase
	{
		/** @brief Type returned by each getter */
		typedef typename Arg<T>::type returnType;

		/** @brief Type of a getter */
		typedef returnType (obj::* type)() const;

		/** @brief Used to specify getters with an extra parameter */
		template<typename P>
		struct extra_
		{
			/** @brief Type of a getter with a parameter */
			typedef returnType (obj::* type)(P) const;
		};
	};

	/**
	* @brief Define some getters based on the return type and the object type
	*
	* @note Used mainly as a wrapper around several getters
	*/
	template<typename T, class obj>
	struct Getters : public GetterBase<T,obj>
	{
	    typedef typename GetterBase<T,obj>::type type;
	    typedef typename GetterBase<T,obj>::returnType returnType;

		/**
		* @brief Stores getters address for later call
		*/
		template<type g1>
		struct store
		{
			/**
			* @brief Aggregate the values from all getters and return the result
			* @see Other specializations of Getters
			*/
			static inline returnType call(const obj* o)
			{
				return (o->*g1)();
			}
		};

		/**
		* @brief For getters with an extra parameter
		*/
		template<typename P>
		struct extra
		{
			/** @brief Convenient typedef */
			typedef GetterBase<T,obj> Base;
			typedef typename Base::template extra_<P>::type gtype;

			/**
			* @brief Stores getters address for later call
			*/
			template<gtype g1>
			struct store
			{
				/**
				* @brief Aggregate the values from all getters and return the result
				* @see Other specializations of Getters
				*/
				static inline returnType call(const obj* o, P v)
				{
					return (o->*g1)(v);
				}
			};
		};
	};

	/** @brief Pair specialization for Getters */
	template<typename T, class obj>
	struct Getters<Pair<T>,obj> : public GetterBase<T,obj>
	{
	    typedef typename GetterBase<T,obj>::type type;
	    typedef typename GetterBase<T,obj>::returnType returnType;

		template<type g1, type g2>
		struct store
		{
			static inline Pair<T> call(const obj* o)
			{
				return Pair<T>((o->*g1)(),(o->*g2)());
			}
		};

		template<typename P>
		struct extra
		{
			typedef typename GetterBase<T,obj>::template extra_<P>::type gtype;

			template<gtype g1, gtype g2>
			struct store
			{
				static inline returnType call(const obj* o, P v)
				{
					return Pair<T>((o->*g1)(v),(o->*g2)(v));
				}
			};
		};
	};

	/** @brief Triplet specialization for Getters */
	template<typename T, class obj>
	struct Getters<Triplet<T>,obj> : public GetterBase<T,obj>
	{
	    typedef typename GetterBase<T,obj>::type type;
	    typedef typename GetterBase<T,obj>::returnType returnType;

		template<type g1, type g2, type g3>
		struct store
		{
			static inline Triplet<T> call(const obj* o)
			{
				return Triplet<T>((o->*g1)(),(o->*g2)(),(o->*g3)());
			}
		};

		template<typename P>
		struct extra
		{
			typedef typename GetterBase<T,obj>::template extra_<P>::type gtype;

			template<gtype g1, gtype g2, gtype g3>
			struct store
			{
				static inline returnType call(const obj* o, P v)
				{
					return Triplet<T>((o->*g1)(v),(o->*g2)(v),(o->*g3)(v));
				}
			};
		};
	};

	/** @brief Quadruplet specialization for Getters */
	template<typename T, class obj>
	struct Getters<Quadruplet<T>,obj> : public GetterBase<T,obj>
	{
	    typedef typename GetterBase<T,obj>::type type;
	    typedef typename GetterBase<T,obj>::returnType returnType;

		template<type g1, type g2, type g3, type g4>
		struct store
		{
			static inline Quadruplet<T> call(const obj* o)
			{
				return Quadruplet<T>((o->*g1)(),(o->*g2)(),(o->*g3)(),(o->*g4)());
			}
		};

		template<typename P>
		struct extra
		{
			typedef typename GetterBase<T,obj>::template extra_<P>::type gtype;

			template<gtype g1, gtype g2, gtype g3, gtype g4>
			struct store
			{
				static inline returnType call(const obj* o, P v)
				{
					return Quadruplet<T>((o->*g1)(v),(o->*g2)(v),(o->*g3)(v),(o->*g4)(v));
				}
			};
		};
	};
}

#endif
