#pragma once

#include        <stdlib.h>
#include        <stdio.h>

#if defined(__GNUC__)
	#include <cxxabi.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <memory.h>
	#include <fcntl.h>

	#ifdef _WIN32
		#include <windows.h>
		#include <stdio.h>
		#include <conio.h>
		#include <tchar.h>
	#else
		#include <dlfcn.h>
		#include <sys/ipc.h>
		#include <sys/shm.h>
		#include <sys/ioctl.h>
		#include <termios.h>
	#endif

#endif

#include        <memory.h>
#include        <math.h>
#include        <vector>
#include        <stack>
#include		<regex>
#include        <new>
#include        <iostream>
#include 		<functional>
#include 		<sstream>
#include 		<string>
#include 		<cstring>
#include 		<list>
#include 		<utility>
#include 		<float.h>
#include 		<cstdarg>
#include 		<stdexcept>
#include 		<chrono>

#include 		<typeinfo>
#include 		<string.h>
#include 		<map>
#include 		<cstddef>

#include 		<type_traits>

// Prototypes & structs
#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif


// utils
#include "util/zs_strutils.h"
#include "util/zs_file.h"
#include "util/zs_log.h"
#include "util/zs_rtti.h"
#include "util/zs_path.h"
#include "util/zs_vector.h"
#include "util/zs_map.h"

#include "common.h"
#include "ByteCode.h"
#include "Instruction.h"
#include "StackElement.h"
#include "Symbol.h"
#include "exception.h"

#include "function_proxy/FunctionTraits.h"
#include "function_proxy/FunctionProxyFactory.h"

#include "scope/Scope.h"
#include "scope/ScopeFactory.h"

#include "built_in/MathBuiltIn.h"
#include "built_in/IoBuiltIn.h"

#include "script/ScriptVar.h"
#include "script/ScriptVarString.h"
#include "script/ScriptVarVector.h"
#include "script/ScriptVarFunction.h"
#include "script/ScriptVarDictionary.h"
//#include "script/ScriptContext.h"
#include "script/ScriptClass.h"
#include "script/ScriptFunction.h"
#include "script/ScriptFunctionFactory.h"
#include "script/ScriptClassFactory.h"

#include "eval/eval.h"
#include "zetscript.h"


#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0


#define ZS_CLASS_BASE_OF(zs,a,b)								(zs)->nativeClassBaseOf<a,b>()
#define ZS_REGISTER_GLOBAL_FUNCTION(zs,text,s) 					(zs)->registerNativeGlobalFunction(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_GLOBAL_VARIABLE(zs,text,s) 					(zs)->registerNativeGlobalVariable(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_CLASS(zs,class_type,s) 						(zs)->registerNativeClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_SINGLETON_CLASS(zs,class_type,s)			(zs)->registerNativeSingletonClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_VARIABLE_MEMBER(zs,class_type,s,v)			(zs)->registerNativeVariableMember<class_type>(s,v)
#define ZS_REGISTER_FUNCTION_MEMBER_STATIC(zs,class_type,s,f)	(zs)->registerNativeMemberFunctionStatic<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_FUNCTION_MEMBER(zs,class_type,s,f)			(zs)->registerNativeMemberFunction<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_CONSTANT_INT(zs,constant_name,v)			(zs)->registerConstantIntValue(constant_name,v)


namespace zetscript{


	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_int_type_ptr;				//	typeid(int *).name()
	extern const char * k_str_const_int_type_ptr;		//	typeid(int *).name()
	extern const char * k_str_float_type_ptr;			//	typeid(float *).name()
	extern const char * k_str_const_float_type_ptr;		//	typeid(float *).name()
	extern const char * k_str_string_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(std::string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char * k_str_int_type;					//	typeid(int).name()
	extern const char * k_str_unsigned_int_type;		//	typeid(unsigned int).name()
	extern const char *	k_str_intptr_t_type;			//	typeid(intptr_t).name()

	extern const char * k_str_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()



	class VirtualMachine;
	class ScriptEval;
	class ZetScript{

	public:


		//===================================================================================================
		//
		// PRINT ASM INFO
		void printGeneratedCode();

		// PRINT ASM INFO
		//---------------------------------------------------------------------------------------------------------------------------------------



		ZetScript();

		inline VirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline FunctionProxyFactory * getFunctionProxyFactory() { return function_proxy_factory;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline ScriptClassFactory *getScriptClassFactory() { return script_class_factory;}

		void	setCallbackOnError(PrintFunctionCallback _fun);

		int * evalIntValue(const std::string & str_to_eval);
		bool * evalBoolValue(const std::string & str_to_eval);
		float * evalFloatValue(const std::string & str_to_eval);
		std::string * evalStringValue(const std::string & str_to_eval);

		void eval(const std::string & expresion, bool execute=true,bool show_bytecode=false, const char * filename="");
		void evalFile(const std::string & filename,bool execute=true,bool show_bytecode=false);

		ConstantValue 	* 		registerConstantIntValue(const std::string & const_name, int value);

		// CONSTANT TOOLS
		ConstantValue * getRegisteredConstantValue(const std::string & const_name);
		ConstantValue *	registerConstantValue(const std::string & const_name, ConstantValue constant_value);
		ConstantValue * registerConstantValue(const std::string & const_name, void *obj, unsigned short properties);
		ConstantValue * registerConstantValue(const std::string & const_name, int  value);
		ConstantValue * registerConstantValue(const std::string & const_name, float  value);

		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		bool isFilenameAlreadyParsed(const std::string & filename);
		const char * getParsedFilenameFromIdx(unsigned idx);

		//-----------------------------------------------

		/**
		* clear: clear compile information.
		*/
		void clear();

		void 						setPrintOutCallback(void (*)(const char *));

		void 												registerNativeBaseSymbols(bool r){
			script_class_factory->registerNativeBaseSymbols(r);
		}

		/**
		 * Register C variable
		 */
		template <typename V>
		 void registerNativeGlobalVariable(const std::string & var_str,V var_ptr, const char *registered_file="",int registered_line=-1){
			 script_class_factory->registerNativeGlobalVariable(var_str,var_ptr, registered_file, registered_line);
		 }

		/**
		 * Register C function
		 */
		template <typename F>
		void registerLocalFunction( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			script_class_factory->registerNativeGlobalFunction( function_name,function_ptr, registered_file,registered_line);
		}



		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		 void registerSingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			script_class_factory->registerNativeSingletonClass<C>(class_name, registered_file, registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		void registerClass(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			script_class_factory->registerNativeClass<C>(class_name, registered_file,registered_line);
		}

		template<class C, class B>
		void classBaseOf(){
			script_class_factory->nativeClassBaseOf<C,B>();
		}

		/**
		 * Register Function Member Class
		 */
		template < typename C, typename R, class T, typename..._A>
		void registerNativeMemberFunction(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 ){
			script_class_factory->registerNativeMemberFunction<C>(function_name,function_type, registered_file,registered_line );
		}

		/**
		 * Register Static Function Member Class
		 */
		template <typename C,typename F>
		void registerMemberFunctionStatic(const char *function_name,F fun, const char *registered_file="",int registered_line=-1){
			script_class_factory->registerNativeMemberFunctionStatic<C>(function_name,fun, registered_file, registered_line);

		}

		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		void registerNativeVariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1){
			script_class_factory->registerNativeVariableMember<C>(var_name,var_pointer,registered_file,registered_line);
		}

		//cpp binding
		// Helpers...
		inline StackElement convertVarToStackElement(intptr_t var_ptr, int idx_builtin_type);
		inline bool convertStackElementToVar(StackElement *stack_element, int idx_builtin_type, intptr_t *result, std::string & error);

		template<typename T>
		static ScriptVarVector * convertStdVectorToScriptVarVector(const std::vector<T> & v,ZetScript *zs_instance){
			ScriptVarVector *vsv = new ScriptVarVector(zs_instance);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->newSlot();
				//intptr_t uvar = (intptr_t)(v[i]);
				*stk = zs_instance->convertVarToStackElement((intptr_t)(v[i]),zs_instance->script_class_factory->getIdxClassFromItsNativeType(typeid(T).name()));
			}

			return vsv;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f ,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(void **f,ScriptVar *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;



		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj );

		template <  typename F>
		std::function<F> * bindScriptFunction(const std::string & function_access);


		 ~ZetScript();

	private:

		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedFile;


		//--------
		// VARS
		std::map<std::string,ConstantValue *> 	 	constant_values;
		std::vector<ParsedFile> 			 		parsed_files;

		//ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		FunctionProxyFactory * function_proxy_factory;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory *script_class_factory;

		//ZetScript 		*zs;

		float eval_float;
		int eval_int;
		bool eval_bool;
		std::string eval_string;


		//===================================================================================================
		//
		// PRINT ASM INFO
//		std::string getStrMovVar(Instruction * iao);

		//void printGeneratedCode(ScriptFunction *sfo);

		//----
		void evalInternal(const char * code, bool exec_vm, bool show_bytecode, const char * filename);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

	};

}

#include	"vm/VirtualMachine.h"
#include 	"zetscript.tcc"

