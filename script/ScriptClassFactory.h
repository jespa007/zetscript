/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once



#define NEW_CLASS_VAR_BY_IDX(data,idx)					((data->script_class_factory)->instanceScriptVariableByIdx(idx))
#define GET_SCRIPT_CLASS(data,idx_or_name)				((data->script_class_factory)->getScriptClass(idx_or_name))
#define GET_SCRIPT_CLASS_NAME(data,idx) 				((data->script_class_factory)->getScriptClassName(idx))
#define SCRIPT_CLASS_MAIN(data)							((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_MAIN))    // 0 is the main class
#define SCRIPT_CLASS_STRING(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_STRING))
#define SCRIPT_CLASS_DICTIONARY(data)					((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_DICTIONARY))
#define SCRIPT_CLASS_VECTOR(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_VECTOR))
#define SCRIPT_CLASS_FUNCTOR(data)						((data->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_FUNCTION))
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(data,s)		(data->script_class_factory)->getScriptClassByNativeClassPtr(s))    // 0 is the main class
#define GET_IDX_2_CLASS_C_STR(data,idx) 				((data->script_class_factory)->getScriptClass(idx)->str_class_ptr_type)

namespace zetscript{

	class ScriptVar;
	class ScriptClass;
	class ZetScript;


	class ScriptClassFactory{

	public:

		ScriptClassFactory(ZetScript *_zs);
		void init();

		 /**
		  * Class management region
		  */
		ScriptClass * 					registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");
		ScriptClass * 					getScriptClass(unsigned char idx);
		ScriptClass * 					getScriptClass(const std::string & name);
		ScriptClass * 					getScriptClassByNativeClassPtr(const std::string & class_type);
		const char 	* 					getScriptClassName(unsigned char idx);
		bool							nativeClassBaseOf(unsigned char  theClass,unsigned char  class_idx);
		unsigned char					getIdxClassFromItsNativeType(const std::string & s);
		unsigned char 					getIdx_C_RegisteredClass(const std::string & str_classPtr);
		zs_vector	* 					getScriptClasses();

		bool 							isClassRegistered(const std::string & v);

		intptr_t						doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);
		void 							clear();
		inline ScriptClass * 			getMainObject() { return main_object;}
		inline ScriptFunction * 		getMainFunction() { return main_function;}

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		ScriptVar 			* 			instanceScriptVariableByClassName(const std::string & class_name);
		ScriptVar 			* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);

		void 							registerNativeBaseSymbols(bool _register);

		/**
		 * Register C variable
		 */
		template <typename V>
		void registerNativeGlobalVariable(
			 const std::string & var_name
			 ,V var_ptr
			 , const char *registered_file=""
			 ,int registered_line=-1
		);

		/**
		 * Register C function
		 */
		 template <typename F>
		 void registerNativeGlobalFunction(
			 const char * function_name
			 ,F function_ptr
			 , const char *registered_file=""
			,int registered_line=-1
		);

		/**
		 * Register C Class. Return index registered class
		 */
		 template<typename T>
		 ScriptClass *  registerNativeSingletonClass(
				 const std::string & class_name
				 , const char *registered_file=""
				,int registered_line=-1
		);

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename T>
		void registerNativeClass(
				const std::string & class_name
				 , const char *registered_file=""
				,int registered_line=-1
		);


		template<typename T>
		void registerNativeClassBuiltIn(
				const std::string & class_name
				 , const char *registered_file=""
				,int registered_line=-1
		);


		template<class T, class B>
		void nativeClassBaseOf();

		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		void registerNativeVariableMember(
				const char *var_name
				, R T::*var_pointer
				 , const char *registered_file=""
				,int registered_line=-1
		);

		template <typename C, typename R>
		void registerNativeStaticConstMember(
				const char *var_name
				, const R var_pointer
				 , const char *registered_file=""
				,int registered_line=-1
		);

		/**
		 * Register C Member function Class
		 */
		template < typename C, typename R, class T, typename..._A>
		void registerNativeMemberFunction(
				const char *function_name
				,R (T:: *function_type)(_A...)
				 , const char *registered_file=""
				,int registered_line=-1
		);

		/**
		 * Register C Member function static Class
		 */
		template <typename C, typename F>
		void registerNativeMemberFunctionStatic(
				const char *function_name
				,F ptr_function
				, const char *registered_file=""
				,int registered_line=-1);

		/**
		 * Register C function as function member
		 */
		template <typename F>
		void	registerNativeMemberFunction(
				const char *function_name
				,F function_type
				 , const char *registered_file=""
				,int registered_line=-1
		);



		~ScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			IdxBuiltInType  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			zs_vector					params;
		}RegisterFunction;

		zs_vector 						*   script_classes;
		ZetScript 						*	zs;
		ScopeFactory 					*	scope_factory;
		ScriptFunctionFactory 			*	script_function_factory;
		FunctionProxyFactory 			*	function_proxy_factory;
		ScriptClass 					* 	main_object;
		ScriptFunction 					* 	main_function;
		std::map<unsigned char,std::map<unsigned char,ConversionType>> 	conversion_types;

		/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
		bool register_c_base_symbols;
		MathBuiltIn math_built_in;
		IoBuiltIn 	io_built_in;


		PrimitiveType *					getPrimitiveTypeFromStr(const std::string & str);

		std::map<unsigned char, std::map<unsigned char, ConversionType>>
					*  					getConversionTypes();

		void 							registerPrimitiveTypes();

		unsigned char					getIdxScriptClassInternal(const std::string & class_name);
		unsigned char					getIdxScriptInternalFrom_C_Type(const std::string & str_native_type);

		void 							setup();
		void 							internalPrintError(const char *s);


	};

}
#include "ScriptClassFactory.tcc"

