/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define IDX_SCRIPT_CLASS_MAIN							0
#define NEW_OBJECT_VAR_BY_CLASS_IDX(data,idx)			((data->script_class_factory)->instanceScriptObjectByClassIdx(idx))
#define GET_SCRIPT_CLASS(data,idx_or_name)				((data->script_class_factory)->getScriptClass(idx_or_name))
#define GET_SCRIPT_CLASS_NAME(data,idx) 				((data->script_class_factory)->getScriptClassName(idx))
#define SCRIPT_CLASS_MAIN(data)							((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_MAIN))    // 0 is the main class

#define SCRIPT_OBJECT_STRING(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING))
#define SCRIPT_OBJECT_STRING_ITERATOR(data)				((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR))
//#define SCRIPT_CLASS_DICTIONARY(data)					((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_DICTIONARY))
#define SCRIPT_OBJECT_VECTOR(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR))
#define SCRIPT_OBJECT_VECTOR_ITERATOR(data)				((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR))

#define SCRIPT_OBJECT_OBJECT(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT))
#define SCRIPT_OBJECT_OBJECT_ITERATOR(data)				((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR))

//#define SCRIPT_CLASS_FUNCTOR(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_FUNCTION))
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(data,s)		(data->script_class_factory)->getScriptClassByNativeClassPtr(s))    // 0 is the main class
#define GET_IDX_2_CLASS_C_STR(data,idx) 				((data->script_class_factory)->getScriptClass(idx)->str_class_ptr_type)

#define ZS_STATIC_CONSTRUCTOR_DESTRUCTOR(obj_type) \
 obj_type * obj_type##_new(){ return new obj_type();} \
 void obj_type##_delete(obj_type *ptr){ delete  (obj_type *)(ptr);}



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
		ScriptClass * 					registerClass(const std::string & class_name, const std::string & base_class_name="",const char * file="", short line=-1);
		ScriptClass * 					getScriptClass(short idx);
		ScriptClass * 					getScriptClass(const std::string & name);
		int								getBuiltinTypeOrClass(const std::string & name);
		ScriptClass * 					getScriptClassByNativeClassPtr(const std::string & class_type);
		const char 	* 					getScriptClassName(short idx);
		void							classInheritsFrom(const std::string & the_class,const std::string & the_base_class);
		bool							isClassInheritsFrom(short idx_class,short idx_base_class);
		short							getIdxClassFromItsNativeType(const std::string & s);
		short		 					getIdxNativeRegisteredClass(const std::string & str_classPtr);
		std::vector<ScriptClass *>	*	getScriptClasses();

		bool 							isClassRegistered(const std::string & v);

		zs_int							doCast(zs_int obj, short idx_class_src, short idx_class_dst/*, std::string & error*/);
		inline ScriptClass * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptObject 			* 			instanceScriptObjectByClassName(const std::string & class_name);
		ScriptObject 			* 			instanceScriptObjectByClassIdx(short  idx_class, void * value_object = NULL);

		void 							registerNativeBaseSymbols(bool _register);

		/**
		 * Register C variable
		 */
		template <typename V>
		void registerNativeGlobalVariable(
			 const std::string & var_name
			 ,V var_ptr
			 , const char *registered_file=""
			 ,short registered_line=-1
		);

		void registerConstantVariable(const std::string & var_name, int value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const std::string & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const std::string & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const std::string & var_name, const std::string & v, const char *registered_file="", short registered_line=-1);
		void registerConstantVariable(const std::string & var_name, const char * v, const char *registered_file="", short registered_line=-1);

		/**
		 * Register C function
		 */
		 template <typename F>
		 void registerNativeGlobalFunction(
			const std::string &  function_name
			 ,F function_ptr
			 , const char *registered_file=""
			,short registered_line=-1
		);

		/**
		 * Register C Class. Return index registered class
		 */
		 template<typename T>
		 ScriptClass *  registerNativeSingletonClass(
				 const std::string & class_name
				 , const char *registered_file=""
				,short registered_line=-1
		);


		/**
		 * User Register C Class
		 */
		template<typename C>
		void registerNativeClass(
			const std::string & class_name
			, const char *registered_file=""
			,short registered_line=-1
		);


		template<typename C>
		ScriptClass * registerNativeClassStatic(
			const std::string & class_name
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
				const std::string & var_name
				, const R var_pointer
				 , const char *registered_file=""
				,short registered_line=-1
		);

		template <typename C,typename F>
		void registerNativeMemberAttributeSetter(
				const std::string & attr_name
				,F ptr_function_setter
				, const char *registered_file=""
				,short registered_line=-1
		);

		/*
		 * register attribute getter
		 */
		template <typename C,typename F>
		void registerNativeMemberAttributeGetter(
				const std::string & attr_name
				,F ptr_function_getter
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register attribute post_inc
		 */
		template <typename C,typename F>
		void registerNativeMemberAttributePostIncrement(
				const std::string & attr_name
				,F ptr_function_getter
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register attribute post_dec
		 */
		template <typename C,typename F>
		void registerNativeMemberAttributePostDecrement(
				const std::string & attr_name
				,F ptr_function_getter
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register attribute pre_inc
		 */
		template <typename C,typename F>
		void registerNativeMemberAttributePreIncrement(
				const std::string & attr_name
				,F ptr_function_getter
				, const char *registered_file=NULL
				,short registered_line=-1
		);

		/*
		 * register attribute pre_dec
		 */
		template <typename C,typename F>
		void registerNativeMemberAttributePreDecrement(
				const std::string & attr_name
				,F ptr_function_getter
				, const char *registered_file=NULL
				,short registered_line=-1
		);



		/**
		 * Register C Member function static Class
		 */
		template <typename C, typename F>
		void registerNativeMemberFunctionStatic(
				const std::string & function_name
				,F ptr_function
				, const char *registered_file=""
				,short registered_line=-1);

		/**
		 * Register C function as function member
		 */
		template <typename C,typename F>
		void	registerNativeMemberFunction(
				const std::string & function_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		);

		void 	clear(short _idx_start=ZS_IDX_UNDEFINED);
		void 	saveState();


		~ScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			BuiltinType  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 						*return_type;
			std::vector<ScriptFunctionArg *>	params;
		}RegisterFunction;

		std::vector<ScriptClass *> 		*   script_classes;
		ZetScript 						*	zs;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		ScriptClass 					* 	main_object;
		ScriptFunction 					* 	main_function;
		std::map<short,std::map<short,ConversionType>> 	conversion_types;

		/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
		bool register_c_base_symbols;
		int 			idx_clear_checkpoint;

		void							checkClassName(const std::string & class_name);
		PrimitiveType *					getPrimitiveTypeFromStr(const std::string & str);

		std::map<short, std::map<short, ConversionType>>
					*  					getConversionTypes();

		void 							registerPrimitiveTypes();

		int								getIdxScriptClassInternal(const std::string & class_name);
		int								getIdxScriptInternalFrom_C_Type(const std::string & str_native_type);

		void 							setup();
		void 							internalPrintError(const char *s);


	};

}
#include "ScriptClassFactory.tcc"

