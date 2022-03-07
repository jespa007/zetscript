/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define NEW_OBJECT_VAR_BY_CLASS_IDX(data,idx)			((data->script_type_factory)->instanceScriptObjectByTypeIdx(idx))
#define GET_SCRIPT_CLASS(data,idx_or_name)				((data->script_type_factory)->getScriptType(idx_or_name))
#define GET_SCRIPT_CLASS_NAME(data,idx) 				((data->script_type_factory)->getScriptTypeName(idx))
#define SCRIPT_CLASS_MAIN(data)							((data->script_type_factory)->getScriptType(IDX_TYPE_CLASS_MAIN))    // 0 is the main class

#define SCRIPT_OBJECT_STRING(data)						((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_STRING))
#define SCRIPT_OBJECT_ITERATOR_STRING(data)				((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING))
//#define SCRIPT_CLASS_DICTIONARY(data)					((data->script_type_factory)->getScriptType(IDX_TYPE_DICTIONARY))
#define SCRIPT_OBJECT_VECTOR(data)						((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_VECTOR))
#define SCRIPT_OBJECT_ITERATOR_VECTOR(data)				((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR))

#define SCRIPT_OBJECT_OBJECT(data)						((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_ITERATOR_OBJECT(data)				((data->script_type_factory)->getScriptType(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(data)						((data->script_type_factory)->getScriptType(IDX_TYPE_FUNCTION))
#define GET_IDX_2_CLASS_C_STR(data,idx) 				((data->script_type_factory)->getScriptType(idx)->script_type_name_ptr)


namespace zetscript{

	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_zs_int_type_ptr;			//	typeid(zs_int *).name()
	extern const char * k_str_const_zs_int_type_ptr;	//	typeid(zs_int *).name()
	extern const char * k_str_zs_float_type_ptr;			//	typeid(zs_float *).name()
	extern const char * k_str_const_zs_float_type_ptr;		//	typeid(zs_float *).name()
	extern const char * k_str_zs_string_type_ptr;			//	typeid(zs_string *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(zs_string *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(zs_string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char *	k_str_zs_int_type;				//	typeid(zs_int).name()

	extern const char * k_str_zs_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()

	class ScriptObjectObject;
	class ScriptType;
	class ZetScript;

	typedef zs_int (*ConversionType)(zs_int);


	class ScriptTypeFactory{

	public:

		ScriptTypeFactory(ZetScript *_zs);
		void init();
		void registerSystem();

		 /**
		  * Class management region
		  */
		ScriptType * 					registerClass(const zs_string & script_type_name, const zs_string & base_class_name="",const char * file="", short line=-1);
		ScriptType * 					registerStaticClass(const zs_string & _script_type_name,const char * file="", short line=-1);
		ScriptType * 					getScriptType(short _idx_script_type);
		ScriptType * 					getScriptType(const zs_string & _type_name);
		ScriptType * 					getScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr);

		short							getIdxScriptType(const zs_string & _type_name);
		short		 					getIdxScriptTypeFromTypeNamePtr(const zs_string & _type_name_ptr);

		int								getBuiltinTypeOrClass(const zs_string & name);

		const char 	* 					getScriptTypeName(short _idx_script_type);
		void							scriptClassTypeInheritsFrom(const zs_string & _type_class_name,const zs_string & _type_name_inherits_from);
		bool							isScriptClassTypeInheritsFrom(short _idx_class_type,short _idx_class_type_inherits_from);
		bool							isScriptTypeInstanceable(short _idx_script_type);

		zs_vector	* 					getScriptTypes();

		bool 							isScriptTypeRegistered(const zs_string & _type_name);

		inline ScriptType * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByTypeName(const zs_string & script_type_name);
		ScriptObject 			* 			instanceScriptObjectByTypeIdx(short  idx_script_type, void * value_object = NULL);

		/**
		 * Register C variable
		 */
		template <typename V>
		void registerNativeGlobalVariable(
			 const zs_string & var_name
			 ,V var_ptr
			 , const char *registered_file=""
			 ,short registered_line=-1
		);

		void registerConstantVariable(const zs_string & var_name, int value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const zs_string & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const zs_string & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const zs_string & var_name, const zs_string & v, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const zs_string & var_name, const char * v, const char *registered_file="", short registered_line=-1);

		/**
		 * Register C function
		 */
		 template <typename F>
		 void registerNativeGlobalFunction(
			const zs_string &  name_script_function
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Register C Class. Return index registered class
		 */
		 template<typename T>
		 ScriptType *  registerNativeStaticClass(
				 const zs_string & script_type_name
				 , const char *registered_file=""
				,short registered_line=-1
		);

		/**
		 * User Register C Class
		 */
		template<typename C>
		ScriptType * registerNativeClass(
			const zs_string & script_type_name
			, C * (*_constructor)(ZetScript *_zs)
			, void (*_destructor)(ZetScript *_zs,C *)
			, const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Instenceable Register C type
		 */
		template<typename C>
		ScriptType * registerNativeType(
			const zs_string & script_type_name
			, C * (*_constructor)(ZetScript *_zs)
			, void (*_destructor)(ZetScript *_zs,C *)
			, const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Static C type
		 */
		 template<typename T>
		 ScriptType *  registerNativeTypeStatic(
				 const zs_string & script_type_name
				 , const char *registered_file=""
				,short registered_line=-1
		);


		/**
		 * Built in register C Class, like ScriptObject,ScriptObjectString...
		 */
		template<class C, class B>
		void nativeClassInheritsFrom();

		void registerNativeMemberSymbols();

		/**
		 * Register C Member constructor
		 */
		template <typename C,typename F>
		void registerNativeConstructor(
				F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		template <typename C, typename R>
		void registerNativeStaticConstMember(
				const zs_string & var_name
				, const R var_pointer
				 , const char *registered_file=""
				,short registered_line=-1
		);

		template <typename C,typename F>
		void registerNativeMemberPropertySetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register property getter
		 */
		template <typename C,typename F>
		void registerNativeMemberPropertyGetter(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_inc
		 */
		template <typename C,typename F>
		void registerNativeMemberPropertyPostIncrement(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property post_dec
		 */
		template <typename C,typename F>
		void registerNativeMemberPropertyPostDecrement(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_inc
		 */
		template <typename C,typename F>
		void registerNativeMemberPropertyPreIncrement(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register member property pre_dec
		 */
		template <typename C,typename F>
		void registerNativeMemberPropertyPreDecrement(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property add set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyAddSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register member property sub set operation
		template <typename C,typename F>
		void registerNativeMemberPropertySubSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mul set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyMulSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register div set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyDivSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register mod set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyModSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register and set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyAndSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register or set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyOrSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register xor set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyXorSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shl set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyShlSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		// register shr set operation
		template <typename C,typename F>
		void registerNativeMemberPropertyShrSet(
				const zs_string & _property_name
				,F _ptr_function
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/**
		 * Register C Member function static Class
		 */
		template <typename C, typename F>
		void registerNativeMemberFunctionStatic(
				const zs_string & _function_name
				,F _ptr_function
				, const char *_registered_file=""
				,short _registered_line=-1);



		/**
		 * Register C function as function member
		 */
		template <typename C,typename F>
		void	registerNativeMemberFunction(
				const zs_string & name_script_function
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		);

		void 	clear(short _idx_start=ZS_IDX_UNDEFINED);
		void 	saveState();


		~ScriptTypeFactory();

	private:

		typedef struct{
			const char *   type_str;
			Type  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			zs_vector					params;
		}RegisterFunction;

		zs_vector 						*   script_types;
		ZetScript 						*	zs;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		ScriptType 						* 	main_object;
		ScriptFunction 					* 	main_function;

		int 							idx_clear_checkpoint;

		void							checkClassName(const zs_string & script_type_name);
		PrimitiveType *					getPrimitiveTypeFromStr(const zs_string & str);


		void 							registerPrimitiveTypes();

		short							getIdxTypeFromTypeNameInternal(const zs_string & _type_name);

		void 							setup();
		void 							internalPrintError(const char *s);


	};

}
#include "ScriptTypeFactory.tcc"

