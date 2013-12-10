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

#ifndef H_SPK_META
#define H_SPK_META

// Note: '__is_enum' intrinsic supported by at least VC++ >= 2005 and GCC >= 4.3
#define IS_ENUM( X ) __is_enum(X)

namespace SPK
{
namespace meta
{
	/**
	* @brief Select a type according to a condition
	*/
	template<typename A, typename B, bool useA>
	struct SelectType
	{
		typedef A type;
	};

	/** @brief Specialization of SelectType when B is selected */
	template<typename A, typename B>
	struct SelectType<A, B, false>
	{
		typedef B type;
	};

	/**
	* @brief Advanced operations on a type array
	*
	* A type array is a structure templated by an int, with the following
	* structure:
	* @code
	*   template<int index>
	*   struct Array
	*   {
	*      static const bool isVoid;
	*      typedef something type;
	*   };
	* @endcode
	*
	* With Array\<index\>, the @c isVoid constant indicates whether
	* the element is valid or not. Valid elements are element considered to
	* be in the array. These elements have to be contiguous.
	*
	* A function trait is passed by the parameter F. Its first template parameter
	* is the type of an element in the array (array\<index\>::type), the second is
	* the index of the element.
	* The function trait can contain several functions; each of these is associated
	* with a tag.
	* @code
	*    template<typename type, int index>
	*    struct Traits
	*    {
	*        static void call(tag*
	*                      //, optional parameters
	*                        )
	*        {
	*            // ...
	*        }
	*
	*        static int get(another_tag*
	*                      //, optional parameters
	*                        )
	*        {
	*            // ...
	*            return 42;
	*        }
	*    };
	* @endcode
	*
	* Tags are just structures (you don't even need to define them, except if
	* you want to specify a return type):
	* @code
	*        struct tag; // Usable only with call()
	*        struct another_tag { typedef int R; } // Usable also with get(), since the return type is specified
	* @endcode
	*
	* First step is to typedef a StaticQuery type, for easier access:
	* @code
	*   //            starting index
	*   //                  v
	*   typedef StaticQuery<0, Array, Traits> query;
	* @endcode
	*
	* Now, there are 2 operations you can do on the query: select a particular element,
	* or call a function on all elements. See the documentation for select() and/or process()
	* for more information.
	*/
	template<int n, template<int> class T, template<typename,int> class F, bool = T<n>::isVoid>
	struct StaticQuery
	{
		typedef typename T<n>::type item;
		typedef StaticQuery<n + 1, T, F> next;
		static const int size = 1 + next::size;

		/**
		* @brief Compile-time search for a specific item.
		* The tag is used to use the right predicate in the traits.
		* The parameter is passed to the predicate.
		*/
		template<typename tag, typename param = void>
		struct search
		{
			/**
			* @brief This value is true if the predicate matched for the index
			* @c n, else false.
			*/
			typedef typename F<item,n>::template predicate<tag, param> traitsPredicate;
			static const bool match = traitsPredicate::value;

			/**
			* @internal
			* @brief This value is true if the predicate matched for the index
			* <tt> n + 1 </tt>, else false.
			*/
			static const bool next_match = next::template search<tag, param>::match;

			/**
			* @brief This value is true if an element matched by the predicate
			* was found for the indices <tt> n..last-index </tt>
			*/
			static const bool found = match || next::template search<tag, param>::found;

			/**
			* @internal
			* @brief Type of the result for the search with the same tag but
			* starting at index <tt> n + 1 </tt>.
			*/
			typedef typename next::template search<tag, param>::result next_result;

			/**
			* @brief Type of the result of this search, starting from index @c n.
			* If nothing is found, it will be a typedef for void.
			*/
			typedef typename SelectType<item, next_result, match && !next_match>::type result;
		};
   
		/**
		* @brief Select a type and call a function on it
		* The index is the index of the type in the array.
		* The function get()  with the specified tag will be called.
		*/
		template<typename tag>
		static typename tag::R select(unsigned int i)
		{
			if(n == (int)i)
				return F<item,n>::get((tag*)0);
			else
				return next::template select<tag>(i);
		}

		/**
		* @brief Convenience specialization with 1 parameter
		* @see select()
		*/
		template<typename tag, typename P>
		static typename tag::R select(unsigned int i, P param)
		{
			if(n == (int)i)
				return F<item,n>::get((tag*)0, param);
			else
				return next::template select<tag, P>(i, param);
		}

		/**
		* @brief Convenience specialization with 2 parameters
		* @see select()
		*/
		template<typename tag, typename P1, typename P2>
		static typename tag::R select(unsigned int i, P1 param1, P2 param2)
		{
			if(n == (int)i)
				return F<item,n>::get((tag*)0, param1, param2);
			else
				return next::template select<tag, P1, P2>(i, param1, param2);
		}

		/**
		* @brief Calls a function on each type of the array.
		* The parameter is passed to the function call(). The tag
		* is used to call the right function in the traits.
		*/
		template<typename tag, typename P1>
		static void process(P1 p1)
		{
			F<item,n>::call((tag*)0,p1);
			next::template process<tag,P1>(p1);
		}

		/**
		* @brief Convenience specialization with 2 parameters
		* @see process()
		*/
		template<typename tag, typename P1, typename P2>
		static void process(P1 p1, P2 p2)
		{
			F<item,n>::call((tag*)0,p1,p2);
			next::template process<tag,P1,P2>(p1, p2);
		}

		/**
		* @brief Convenience specialization with 3 parameters
		* @see process()
		*/
		template<typename tag, typename P1, typename P2, typename P3>
		static void process(P1 p1, P2 p2, P3 p3)
		{
			F<item,n>::call((tag*)0,p1,p2,p3);
			next::template process<tag,P1,P2,P3>(p1, p2, p3);
		}
	};

	template<int n, template<int> class T, template<typename,int> class F>
	struct StaticQuery<n,T,F,true>
	{
		typedef typename T<n>::type item;
		static const int size = 0;

		template<typename tag, typename param = void>
		struct search
		{
			static const bool match = false;
			static const bool found = false;
			typedef void result;
		};

		template<typename tag>
		static typename tag::R select(unsigned int)
		{
			return F<item,n>::get((tag*)0);
		}

		template<typename tag, typename P>
		static typename tag::R select(unsigned int, P param)
		{
			return F<item,n>::get((tag*)0, param);
		}

		template<typename tag, typename P1, typename P2>
		static typename tag::R select(unsigned int, P1 param1, P2 param2)
		{
			return F<item,n>::get((tag*)0, param1, param2);
		}

		template<typename tag, typename P1>
		static void process(P1 p1)
		{
		}

		template<typename tag, typename P1, typename P2>
		static void process(P1 p1, P2 p2)
		{
		}

		template<typename tag, typename P1, typename P2, typename P3>
		static void process(P1 p1, P2 p2, P3 p3)
		{
		}
	};

	/**
	* @internal
	* @brief Used to force overloads of functions
	*/
	template<int>
	struct NumOverload
	{
	};

	/**
	* @internal
	* @brief Template metaprogramming convenience struct
	*/
	template<typename>
	struct ToVoid
	{
		typedef void type;
	};

	/**
	* @internal
	* @brief Store whether to pass a value by ref (here, pass by value)
	* @see PassByRef
	*/
	struct PassByValue
	{
		static const bool passByValue = true;

		template<typename T>
		struct transform
		{
			typedef T type;
		};
	};

	/**
	* @internal
	* @brief Store whether to pass a value by ref (here, pass by ref)
	* @see PassByValue
	*/
	struct PassByRef
	{
		static const bool passByValue = false;
			
		template<typename T>
		struct transform
		{
			typedef const T& type;
		};
	};

	/**
	* @internal
	* @brief Erase an enum: if the type passed is an enum, the
	* result is an int, else it is the passed type.
	*/
	template<typename T, bool = IS_ENUM(T)>
	struct EraseEnum
	{
		static const bool enumErased = false;
		typedef T type;
	};

	/** @brief Specialization of EraseEnum when the passed type is an enum. */
	template<typename T>
	struct EraseEnum<T,true>
	{
		static const bool enumErased = true;
		typedef int type;
	};

	/**
	* @internal
	* @brief Computes whether given types are the same
	*
	* Defined here to allow compilation with C++03
	*/
	template<typename, typename>
	struct IsSame
	{
		static const bool value = false;
	};

	/** @brief Specialization of IsSame when the given types are the same */
	template<typename T>
	struct IsSame<T,T>
	{
		static const bool value = true;
	};

	/**
	* @internal
	* @brief Meta-function storing whether the type is abstract or not
	*/
	template<typename T>
	struct IsAbstract
	{
		struct yes { char d[5]; };
		struct no { char d[2]; };

		template <typename C> static no test(C (*)[1]);
		template <typename> static yes test(...);

		static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
	};

	/**
	* @internal
	* @brief Structure used to clone a type
	*/
	template<typename T, bool = IsAbstract<T>::value>
	struct Cloner
	{
		static T* clone(const T* source) { return new T(*source); }
		static T* create() { return new T(); }
	};

	/**
	* @brief Specialization for when the type is abstract
	* @note This function should (and normally will) never be called
	* It returns 0, the only aim is genericity of some meta-programming
	* code (it avoids compiler errors).
	*/
	template<typename T>
	struct Cloner<T, true>
	{
		static T* clone(const T*) { return 0; }
		static T* create() { return 0; }
	};

	// Init counter
#define MAX_COUNT 64
	template<unsigned int n> struct Count { bool data[n]; };
	template<int n> struct Counter : public Counter<n-1> {};
	template<> struct Counter<0> {};

	// Create a counter
#define MAKE_COUNTER( _tag_ ) \
		static SPK::meta::Count<1> _spk_count_ ## _tag_ (SPK::meta::Counter<1>)

	// Use counter
#define GET_COUNT( _tag_ ) \
		((sizeof(_spk_count_ ## _tag_ (SPK::meta::Counter<MAX_COUNT + 1>())) / sizeof(bool)) - 1)
#define INC_COUNT( _tag_ ) \
		static SPK::meta::Count<GET_COUNT(_tag_) + 2> _spk_count_ ## _tag_ (SPK::meta::Counter<GET_COUNT(_tag_) + 2>)
}
}

/**
* @brief Store a string litteral for use in templates
*
* Make _str_ available through _name_::get() (as a const char*)
*/
#define SPK_STRING( _name_ , _str_ )														\
	struct _name_																			\
	{																						\
		static const char* get()															\
		{																					\
			return #_str_;																	\
		}																					\
	}

/**
*
*/
#define SPK_TYPE_RELATIONS_SETUP( ... )														\
	template<int index>																		\
	struct typeRelationsArray																\
	{																						\
		static const bool isVoid = true;													\
		typedef void type;																	\
	};																						\
	MAKE_COUNTER(type_relation);															\
	__VA_ARGS__																				\
	typedef StaticQuery<0, typeRelationsArray, TypeRelationTraits> typeRelations

/**
*
*/
#define spk_type_relations																	\
	SPK_TYPE_RELATIONS_SETUP

/**
*
*/
#define spk_basetype( _cpp_ , _base_ , _passtype_ )											\
	template<>																				\
	struct typeRelationsArray<GET_COUNT(type_relation)>										\
	{																						\
		SPK_STRING(spk_name, _base_);														\
		SPK_STRING(cpp_name, _cpp_);														\
		static const bool isVoid = false;													\
		typedef BaseTypeRelation<_cpp_, _base_, cpp_name, spk_name, _passtype_> type;		\
	};																						\
	INC_COUNT(type_relation)

#endif
