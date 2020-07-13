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
#include "util/zs_io.h"
#include "util/zs_log.h"
#include "util/zs_rtti.h"
#include "util/zs_path.h"

#include "common.h"
#include "ByteCode.h"
#include "Instruction.h"
#include "StackElement.h"
#include "SymbolInfo.h"
#include "exception.h"

#include "function/FunctionSymbol.h"
#include "function/FunctionTraits.h"
#include "function/FunctionMemberPointer.h"
#include "function/FunctionProxyFactory.h"

#include "scope/Scope.h"
#include "scope/ScopeFactory.h"

#include "script/ScriptVar.h"
#include "script/ScriptVarString.h"
#include "script/ScriptVarVector.h"
#include "script/ScriptVarFunctor.h"
#include "script/ScriptVarDictionary.h"
#include "script/ScriptClassBase.h"
#include "script/ScriptClass.h"
#include "script/ScriptFunction.h"
#include "script/ScriptFunctionFactory.h"
#include "script/ScriptClassFactory.h"



#include "eval/eval.h"
#include "zetscript.h"


#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0


#define ZS_CLASS_C_BASEOF(zs)										(zs)->class_C_BaseOf();
#define ZS_REGISTER_C_FUNCTION(zs,text,s) 							(zs)->register_C_Function(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE(zs,text,s) 							(zs)->register_C_Variable(text,&s,typeid(decltype(&s)).name(),__FILE__, __LINE__)
#define ZS_REGISTER_C_CLASS(zs,class_type,s) 						(zs)->register_C_Class<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_SINGLETON_CLASS(zs,class_type,s)				(zs)->register_C_SingletonClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE_MEMBER(zs,class_type,s,v)			(zs)->register_C_VariableMember<class_type>(s,v)
#define ZS_REGISTER_C_STATIC_FUNCTION_MEMBER(zs,class_type,s,f)		(zs)->register_C_FunctionMemberStatic<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_FUNCTION_MEMBER(zs,class_type,s,f)			(zs)->register_C_FunctionMember<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_CONSTANT_INT(zs,constant_name,v)				(zs)->registerConstantIntValue(constant_name,v)


namespace zetscript{


	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_int_type_ptr;			//	typeid(int *).name()
	extern const char * k_str_float_type_ptr;			//	typeid(float *).name()
	extern const char * k_str_string_type_ptr;		//	typeid(std::string *).name()
	extern const char * k_str_const_char_type_ptr;	//	typeid(std::string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_int_type;				//	typeid(int).name()
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
		inline FunctionProxyFactory * GetProxyFunctionFactory() { return proxy_function_factory;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline ScriptClassFactory *getScriptClassFactory() { return script_class_factory;}


		void	setCallbackOnError(PrintFunctionCallback _fun);

		int * evalIntValue(const std::string & str_to_eval);
		bool * evalBoolValue(const std::string & str_to_eval);
		float * evalFloatValue(const std::string & str_to_eval);
		std::string * evalStringValue(const std::string & str_to_eval);

		bool eval(const std::string & expresion, bool callFunction=true,bool show_bytecode=false, const char * filename="");
		bool evalFile(const std::string & filename,bool callFunction=true,bool show_bytecode=false);

		ConstantValue 	* 		registerConstantIntValue(const std::string & const_name, int value);

		// CONSTANT TOOLS
		ConstantValue * getRegisteredConstantValue(const std::string & const_name);
		ConstantValue * registerConstantValue(const std::string & const_name, void *obj, unsigned short properties);
		ConstantValue * registerConstantValue(const std::string & const_name, int  value);

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

		int getIdxClassFromIts_C_Type(const std::string & str_type){
			return script_class_factory->getIdxClassFromIts_C_Type(str_type);
		}


		void 												register_C_BaseSymbols(bool r){
			script_class_factory->register_C_BaseSymbols(r);
		}

		/**
		 * Register C function
		 */
		template <typename F>
		bool register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_Function( function_name,function_ptr, registered_file,registered_line);
		}

		/**
		 * Register C variable
		 */
		template <typename V>
		 SymbolInfo * register_C_Variable(const std::string & var_str,V var_ptr, const char *registered_file="",int registered_line=-1){
			 return script_class_factory->register_C_Variable(var_str,var_ptr, typeid(V).name(), registered_file, registered_line);
		 }

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		 bool register_C_SingletonClass(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_SingletonClass<C>(class_name, registered_file, registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		bool register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_Class<C>(class_name, registered_file,registered_line);
		}


		template<typename C>
		bool register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1){
			return script_class_factory->register_C_ClassBuiltIn<C>(class_name, registered_file, registered_line);
		}


		template<class C, class B>
		bool class_C_BaseOf(){
			return script_class_factory->class_C_BaseOf<C,B>();
		}

		/**
		 * Register Function Member Class
		 */
		template < typename C, typename R, class T, typename..._A>
		bool register_C_FunctionMember(const char *function_name,R (T:: *function_type)(_A...), const char *registered_file="",int registered_line=-1 ){
			return script_class_factory->register_C_FunctionMember<C>(function_name,function_type, registered_file,registered_line );
		}


		/**
		 * Register Static Function Member Class
		 */
		template <typename C,typename F>
		bool register_C_FunctionMemberStatic(const char *function_name,F fun, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_FunctionMemberStatic<C>(function_name,fun, registered_file, registered_line);

		}

		/**
		 * Register C function as function member
		 */
		template <typename F>
		bool register_C_FunctionAsFunctionMember( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_FunctionAsFunctionMember( function_name,function_ptr, registered_file,registered_line);
		}


		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1){
			return script_class_factory->register_C_VariableMember<C>(var_name,var_pointer,registered_file,registered_line);
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
				*stk = zs_instance->convertVarToStackElement((intptr_t)(v[i]),zs_instance->getIdxClassFromIts_C_Type(typeid(T).name()));
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



		bool getScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj );


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
		FunctionProxyFactory * proxy_function_factory;
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
		bool evalInternal(const char * code, bool exec_vm, bool show_bytecode, const char * filename);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

	};

}

#include	"vm/VirtualMachine.h"
#include 	"zetscript.inc"

