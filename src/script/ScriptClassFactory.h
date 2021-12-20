/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define NEW_OBJECT_VAR_BY_CLASS_IDX(data,idx)			((data->script_class_factory)->instanceScriptObjectByClassIdx(idx))
#define GET_SCRIPT_CLASS(data,idx_or_name)				((data->script_class_factory)->getScriptClass(idx_or_name))
#define GET_SCRIPT_CLASS_NAME(data,idx) 				((data->script_class_factory)->getScriptClassName(idx))
#define SCRIPT_CLASS_MAIN(data)							((data->script_class_factory)->getScriptClass(IDX_TYPE_CLASS_MAIN))    // 0 is the main class

#define SCRIPT_OBJECT_STRING(data)						((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_STRING))
#define SCRIPT_OBJECT_ITERATOR_STRING(data)				((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING))
//#define SCRIPT_CLASS_DICTIONARY(data)					((data->script_class_factory)->getScriptClass(IDX_TYPE_DICTIONARY))
#define SCRIPT_OBJECT_VECTOR(data)						((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_VECTOR))
#define SCRIPT_OBJECT_ITERATOR_VECTOR(data)				((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR))

#define SCRIPT_OBJECT_OBJECT(data)						((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_ITERATOR_OBJECT(data)				((data->script_class_factory)->getScriptClass(IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))

//#define SCRIPT_CLASS_FUNCTOR(data)						((data->script_class_factory)->getScriptClass(IDX_TYPE_FUNCTION))
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(data,s)		(data->script_class_factory)->getScriptClassByNativeClassPtr(s))    // 0 is the main class
#define GET_IDX_2_CLASS_C_STR(data,idx) 				((data->script_class_factory)->getScriptClass(idx)->str_class_ptr_type)


namespace zetscript{

	class ScriptObjectObject;
	class ScriptClass;
	class ZetScript;

	typedef zs_int (*ConversionType)(zs_int);


	class ScriptClassFactory{

	public:

		ScriptClassFactory(ZetScript *_zs);
		void init();
		void registerSystem();

		 /**
		  * Class management region
		  */
		ScriptClass * 					registerClass(const zs_string & class_name, const zs_string & base_class_name="",const char * file="", short line=-1);
		ScriptClass * 					getScriptClass(short idx);

		ScriptClass * 					getScriptClass(const zs_string & name);
		int								getBuiltinTypeOrClass(const zs_string & name);
		ScriptClass * 					getScriptClassByNativeClassPtr(const zs_string & class_type);
		const char 	* 					getScriptClassName(short idx);
		void							classInheritsFrom(const zs_string & the_class,const zs_string & the_base_class);
		bool							isClassInheritsFrom(short idx_type_class,short idx_base_class);
		bool							isClassInstanceable(short idx_type_class);
		short							getIdxClassFromItsNativeType(const char * s);
		short		 					getIdxNativeRegisteredClass(const zs_string & str_classPtr);
		zs_vector	* 					getScriptClasses();

		bool 							isClassRegistered(const zs_string & v);

		inline ScriptClass * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByClassName(const zs_string & class_name);
		ScriptObject 			* 			instanceScriptObjectByClassIdx(short  idx_type_class, void * value_object = NULL);

		void 							registerNativeBaseSymbols(bool _register);

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
			const zs_string &  function_name
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Register C Class. Return index registered class
		 */
		 template<typename T>
		 ScriptClass *  registerNativeSingletonClass(
				 const zs_string & class_name
				 , const char *registered_file=""
				,short registered_line=-1
		);

		/**
		 * User Register C Class
		 */
		template<typename C>
		ScriptClass * registerNativeClass(
			const zs_string & class_name
			, C * (*_constructor)()
			, void (*_destructor)(C *)
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
				const zs_string & function_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		);

		void 	clear(short _idx_start=IDX_ZS_UNDEFINED);
		void 	saveState();


		~ScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			Type  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			zs_vector					params;
		}RegisterFunction;

		zs_vector 						*   script_classes;
		ZetScript 						*	zs;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		ScriptClass 					* 	main_object;
		ScriptFunction 					* 	main_function;

		/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
		bool register_c_base_symbols;
		int 			idx_clear_checkpoint;

		void							checkClassName(const zs_string & class_name);
		PrimitiveType *					getPrimitiveTypeFromStr(const zs_string & str);


		void 							registerPrimitiveTypes();

		int								getIdxScriptClassInternal(const zs_string & class_name);
		int								getIdxScriptInternalFrom_C_Type(const char * str_native_type);

		void 							setup();
		void 							internalPrintError(const char *s);


	};

}
#include "ScriptClassFactory.tcc"

