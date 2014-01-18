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

#ifndef H_SPK_DESCRIPTIONDEFINES
#define H_SPK_DESCRIPTIONDEFINES

namespace SPK
{
namespace meta
{
	/**
	* @brief Compile-time check that attribute name is unique in the inheritance
	*
	* If your compiler outputs an error about this struct, it means one of your attribute has a name
	* that is already taken by one of its ancestor(s). Example:
	* @code
	* class A
	* {
	*     spark_description(A, void)
	*     {
	*         spk_generic(int, value, setValue, getValue);
	*     };
	* };
	*
	* class B : public A
	* {
	*     spark_description(B, void)
	*     {
	*         // Error: the name 'value' is already used by A
	*         spk_generic(float, value, setValue, getValue);
	*     };
	* };
	* @endcode
	*/
	template<bool>
	struct attr_name_already_taken;
	template<>
	struct attr_name_already_taken<false>
	{
		static const bool value = true;
	};

	
}
}

/// Macros

#define SPK_EXPAND_( ... ) __VA_ARGS__
#define SPK_COUNT_ARGS_0_( _1 , _2 , _3 , _4 , _5 , _N , ... ) _N
#define SPK_COUNT_ARGS_( ... ) SPK_EXPAND_(SPK_COUNT_ARGS_0_(__VA_ARGS__,5,4,3,2,1))
#define SPK_CONCAT_0_( x , y ) x ## y
#define SPK_CONCAT_( x , y ) SPK_CONCAT_0_(x,y)
#define SPK_UNPACK_GETTERS_0_1( _obj_ , _g1_ )						&_obj_::_g1_
#define SPK_UNPACK_GETTERS_0_2( _obj_ , _g1_ , _g2_ )				&_obj_::_g1_, &_obj_::_g2_
#define SPK_UNPACK_GETTERS_0_3( _obj_ , _g1_ , _g2_ , _g3_ )		&_obj_::_g1_, &_obj_::_g2_, &_obj_::_g3_
#define SPK_UNPACK_GETTERS_0_4( _obj_ , _g1_ , _g2_ , _g3_ , _g4_ )	&_obj_::_g1_, &_obj_::_g2_, &_obj_::_g3_, &_obj_::_g4_
#define SPK_UNPACK_GETTERS_( _obj_ , ... ) SPK_EXPAND_(SPK_CONCAT_(SPK_UNPACK_GETTERS_0_ , SPK_COUNT_ARGS_(__VA_ARGS__))(_obj_,__VA_ARGS__))


#define SPK_CHECK_UNIQUENESS( _t_ )															\
	template<typename, typename = void>														\
	struct _spk_check ## _t_																\
	{																						\
		static const bool value = false;													\
	};																						\
	template<typename _spk_>																\
	struct _spk_check ## _t_ <_spk_, typename SPK::meta::ToVoid<typename _spk_::_t_>::type>	\
	{																						\
		static const bool value = true;														\
	};																						\
	static const bool unique ## _t_ = SPK::meta::attr_name_already_taken<					\
		_spk_check ## _t_<parentDescription>::value>::value

#define SPK_REGISTER_DESC_TYPE( _tag_ , _type_ )											\
		template<bool b>																	\
		struct spk ## _tag_<GET_COUNT(_tag_), b>											\
		{																					\
			static const bool isVoid = false;												\
			typedef _type_ type;															\
		};																					\
		INC_COUNT(_tag_)

/**
* @internal
*/
#define _spk_description_body( ... )														\
		{																					\
		private:																			\
			static description_base* _spk_getDescription(_spk_obj* obj)						\
			{																				\
				return &obj->_spk_description_obj;											\
			}																				\
			template<typename,typename,typename> friend class SPK::DescriptionBase;			\
			template<int index, bool unused = true>											\
			struct spkAttr																	\
			{																				\
				static const bool isVoid = true;											\
				typedef SPK::DefaultAttribute<_spk_obj> type;								\
			};																				\
			MAKE_COUNTER(Attr);																\
																							\
			template<int index, bool unused = true>											\
			struct spkCtrl																	\
			{																				\
				static const bool isVoid = true;											\
				typedef SPK::DefaultControl<_spk_obj> type;									\
			};																				\
			MAKE_COUNTER(Ctrl);																\
																							\
			/* Workaround for VS2003-2012 : the following structure (Instantiate)         */\
			/* triggers infinite inheritance if used in class template and defined        */\
			/* outside it. See the following page:                                        */\
			/*http://stackoverflow.com/questions/15620686/templated-recurrent-type-error  */\
			template<template<int,bool=true> class A, int n = 0, bool = A<n>::isVoid>		\
			struct Instantiate : public Instantiate<A, n+1>									\
			{																				\
				typedef typename A<n>::type type;											\
				type object;																\
				using Instantiate<A, n+1>::getObject;										\
				Instantiate() : object() {}													\
				type* getObject(SPK::meta::NumOverload<n>)									\
				{																			\
					return &object;															\
				}																			\
			};																				\
			template<template<int,bool> class A, int n>										\
			struct Instantiate<A, n, true>													\
			{																				\
				void getObject();															\
			};																				\
		public:																				\
			__VA_ARGS__																		\
		private:																			\
			Instantiate<spkAttr> _spk_attributes;											\
			template<int n>																	\
			typename spkAttr<n>::type* getAttributePtr()									\
			{																				\
				return _spk_attributes.getObject(SPK::meta::NumOverload<n>());				\
			}																				\
		};																					\
	public:																					\
		class description : public description_base											\
		{																					\
		public:																				\
			static const char* getClassName()												\
			{																				\
				return _spk_inner_name::get();												\
			}																				\
		};																					\
	private:																				\
		description_base _spk_description_obj

/**
* @brief Starts the definition of a type description.
*/
#define spark_description( _classname_ , _parentobject_ )									\
	public:																					\
		template<typename, bool> friend struct SPK::meta::Cloner;							\
		virtual SPK::ClassDescription getDescription()										\
		{																					\
			return description::getClassDescription(this);									\
		}																					\
		virtual std::string getClassName() const											\
		{																					\
			return description::getClassName();												\
		}																					\
		virtual SPK::Ref<SPK::SPKObject> clone() const										\
		{																					\
			return SPK::meta::Cloner<_classname_ >::clone(this);							\
		}																					\
		class description;																	\
	private:																				\
		class description_base;																\
		friend class description_base;														\
		friend class description;															\
	private:																				\
		virtual void propagate()															\
		{																					\
			description::propagate(this);													\
		}																					\
		virtual SPK::ConnectableDescription* getConnectableDescription()					\
		{																					\
			return &_spk_description_obj;													\
		}																					\
		SPK_STRING(_spk_inner_name, _classname_);											\
		typedef _classname_ _spk_obj;														\
		class description_base : public														\
			SPK::DescriptionBase<description_base,											\
								 _spk_obj,													\
								 _parentobject_>											\
		_spk_description_body

/**
* @brief Adjust the description for typedef of template classes.
*/
#define spark_description_specialization( _name_ )											\
	template<>																				\
	class _name_::description : public _name_::description_base								\
	{																						\
	public:																					\
		static const char* getClassName()													\
		{																					\
			return #_name_;																	\
		}																					\
	}

/**
* @brief Defines a control
*/
#define spk_control( _type_ , _name_ )														\
	private:																				\
		SPK_STRING(name ## _name_, _name_);													\
		static void* _spk_get_ctrl_ ## _name_(_spk_obj* obj)								\
		{																					\
			description_base* desc = description_base::_spk_getDescription(obj);			\
			return &(desc->_spk_control_ ## _name_);										\
		}																					\
	public:																					\
		typedef SPK::ObjectValueControl<_spk_obj,											\
										_type_,												\
										name ## _name_,										\
										&description_base::_spk_get_ctrl_ ## _name_> _name_;\
	private:																				\
		_name_ _spk_control_ ## _name_;														\
		SPK_REGISTER_DESC_TYPE(Ctrl, _name_);												\
		SPK_CHECK_UNIQUENESS(_name_)

/**
* @brief Defines an attribute (not structured)
*/
#define spk_attribute( _type_ , _name_ , _setter_ , ... )									\
	private:																				\
		SPK_STRING(name ## _name_, _name_);													\
		static const int index ## _name_ = GET_COUNT(Attr);									\
		static void* _spk_get_attr_ ## _name_(_spk_obj* obj)								\
		{																					\
			description_base* desc = description_base::_spk_getDescription(obj);			\
			return desc->getAttributePtr<index ## _name_>();								\
		}																					\
	public:																					\
		typedef SPK::Attribute<_type_,														\
							   _spk_obj,													\
							   &_spk_obj::_setter_,											\
							   typename SPK::Getters<_type_,_spk_obj>						\
								::template store<SPK_UNPACK_GETTERS_(_spk_obj,__VA_ARGS__)>,\
							   name ## _name_,												\
							   &description_base::_spk_get_attr_ ## _name_> _name_;			\
	private:																				\
		SPK_REGISTER_DESC_TYPE(Attr, _name_);												\
		SPK_CHECK_UNIQUENESS(_name_)

/**
* @brief Defines an array
*/
#define spk_array( _type_ , _name_ , _add_ , _remove_ , _clear_ , _getat_ , _getsize_ )		\
	private:																				\
		SPK_STRING(name ## _name_, _name_);													\
		static const int index ## _name_ = GET_COUNT(Attr);									\
		static void* _spk_get_attr_ ## _name_(_spk_obj* obj)								\
		{																					\
			description_base* desc = description_base::_spk_getDescription(obj);			\
			return desc->getAttributePtr<index ## _name_>();								\
		}																					\
	public:																					\
		typedef SPK::ArrayAttribute<_type_,													\
									_spk_obj,												\
									&_spk_obj::_add_,										\
									&_spk_obj::_remove_,									\
									&_spk_obj::_clear_,										\
									&_spk_obj::_getat_,										\
									&_spk_obj::_getsize_,									\
									name ##_name_,											\
									&description_base::_spk_get_attr_ ## _name_> _name_;	\
	private:																				\
		SPK_REGISTER_DESC_TYPE(Attr, _name_);												\
		SPK_CHECK_UNIQUENESS(_name_)

/**
* @internal
*/
#define _spk_structured_attr_body( ... )													\
	{																						\
		private:																			\
			template<typename,typename,typename o,void (o::*)(),							\
				void (o::*)(unsigned int),void (o::*)(),unsigned int (o::*)() const,		\
				unsigned int>																\
			friend class SPK::StructuredAttributeBase;										\
																							\
			template<int index, bool unused = true>											\
			struct spkField																	\
			{																				\
				static const bool isVoid = true;											\
				typedef SPK::DefaultField<_spk_obj> type;									\
			};																				\
			MAKE_COUNTER(Field);															\
																							\
		__VA_ARGS__																			\
	}

/**
* @brief Defines a structured attribute
*/
#define spk_structure( _name_ , _create_ , _remove_ , _clear_ , _getsize_ )					\
	public:																					\
		class _name_;																		\
	private:																				\
		SPK_STRING(name ## _name_, _name_);													\
		SPK_REGISTER_DESC_TYPE(Attr, _name_);												\
		SPK_CHECK_UNIQUENESS(_name_);														\
	public:																					\
		class _name_																		\
			: public SPK::StructuredAttributeBase<											\
				_name_,																		\
				name ## _name_,																\
				_spk_obj,																	\
				&_spk_obj::_create_,														\
				&_spk_obj::_remove_,														\
				&_spk_obj::_clear_,															\
				&_spk_obj::_getsize_,														\
				GET_COUNT(Attr)>															\
		_spk_structured_attr_body

/**
* @brief Defines a shield in a structured attribute (cannot be used elsewhere)
*/
#define spk_field( _type_ , _name_ , _setter_ , ... )										\
	private:																				\
		SPK_STRING(name ## _name_, _name_);													\
		static void* _spk_get_field_ ## _name_(_spk_obj* obj)								\
		{																					\
			description_base* desc = description_base::_spk_getDescription(obj);			\
			void* attr = desc->getAttributePtr<instantiation::index>();						\
			return &(((instantiation::type*)attr)->_spk_field_ ## _name_);					\
		}																					\
	public:																					\
		typedef SPK::FieldAttribute<_type_,													\
									_spk_obj,												\
									&_spk_obj::_setter_,									\
									typename SPK::Getters<_type_,_spk_obj>					\
									::template extra<unsigned int>							\
									::template store<SPK_UNPACK_GETTERS_(_spk_obj,__VA_ARGS__)>,\
									name ## _name_,											\
									attrName,												\
									&_spk_get_field_ ## _name_> _name_;						\
	private:																				\
		_name_ _spk_field_ ## _name_;														\
		SPK_REGISTER_DESC_TYPE(Field, _name_)

#endif
