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
#include "Instruction.h"
#include "StackElement.h"
#include "SymbolInfo.h"
#include "Instruction.h"

#include "var/ScriptVarString.h"
#include "var/ScriptVarVector.h"
#include "var/ScriptVarFunctor.h"
#include "var/ScriptVarDictionary.h"

#include "zetscript_exception.h"
#include "FunctionMemberPointer.h"


#include "Instruction.h"
#include "StackElement.h"
#include "Scope.h"
#include "ScopeFactory.h"
#include "ScriptClassBase.h"
#include "ScriptFunction.h"
#include "ScriptFunctionFactory.h"
#include "ScriptClass.h"
#include "ScriptClassFactory.h"
#include "ZetScript.h"



#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0


#define ZS_CLASS_C_BASEOF(zs)										(zs)->class_C_BaseOf();
#define ZS_REGISTER_C_FUNCTION(zs,text,s) 							(zs)->Register_C_Function(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE(zs,text,s) 							(zs)->Register_C_Variable(text,&s,typeid(decltype(&s)).name(),__FILE__, __LINE__)
#define ZS_REGISTER_C_CLASS(zs,class_type,s) 						(zs)->Register_C_Class<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_SINGLETON_CLASS(zs,class_type,s)				(zs)->register_C_SingletonClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_C_VARIABLE_MEMBER(zs,class_type,s,v)			(zs)->Register_C_VariableMember<class_type>(s,v)
#define ZS_REGISTER_C_STATIC_FUNCTION_MEMBER(zs,class_type,s,f)		(zs)->Register_C_FunctionMemberStatic<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_FUNCTION_MEMBER(zs,class_type,s,f)			(zs)->register_C_FunctionMember<class_type>(s,f,__FILE__, __LINE__)
#define ZS_REGISTER_C_CONSTANT_INT(zs,constant_name,v)				(zs)->registerConstantIntValue(constant_name,v)


namespace zetscript{

	class VirtualMachine;
	class ScriptEval;
	class ZetScript{



	public:

		std::string  str_void_type;				// 	typeid(void).name()
		std::string  str_int_type_ptr;			//	typeid(int *).name()
		std::string  str_float_type_ptr;		//	typeid(float *).name()
		std::string  str_string_type_ptr;		//	typeid(std::string *).name()
		std::string  str_const_char_type_ptr;	//	typeid(std::string *).name()
		std::string  str_bool_type_ptr;			//	typeid(bool *).name()
		std::string  str_int_type;				//	typeid(int).name()
		std::string  str_unsigned_int_type;		//	typeid(unsigned int).name()
		std::string  str_intptr_t_type;			//	typeid(intptr_t).name()

		std::string  str_float_type;			//	typeid(int).name()
		std::string  str_bool_type;				//	typeid(bool).name()
		std::string  str_stack_element_type;	//	typeid(bool).name()

		//===================================================================================================
		//
		// PRINT ASM INFO
		void PrintGeneratedCode();

		// PRINT ASM INFO
		//---------------------------------------------------------------------------------------------------------------------------------------



		ZetScript();

		inline VirtualMachine * GetVirtualMachine() { return virtual_machine;}
		inline NativeFunctionFactory * GetNativeFunctionFactory() { return native_function_factory;}
		inline ScopeFactory * GetScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *GetScriptFunctionFactory() { return script_function_factory;}
		inline ScriptClassFactory *GetScriptClassFactory() { return script_class_factory;}


		void	SetCallbackOnError(PrintFunctionCallback _fun);

		int * EvalIntValue(const std::string & str_to_eval);
		bool * EvalBoolValue(const std::string & str_to_eval);
		float * EvalFloatValue(const std::string & str_to_eval);
		std::string * EvalStringValue(const std::string & str_to_eval);

		bool Eval(const std::string & expresion, bool Execute=true,bool show_bytecode=false, const char * filename="");
		bool EvalFile(const std::string & filename,bool Execute=true,bool show_bytecode=false);

		ConstantValueInfo 	* 		registerConstantIntValue(const std::string & const_name, int value);

		// CONSTANT TOOLS
		ConstantValueInfo * getRegisteredConstantValue(const std::string & const_name);
		ConstantValueInfo * registerConstantValue(const std::string & const_name, void *obj, unsigned short properties);

		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		bool IsFilenameAlreadyParsed(const std::string & filename);
		const char * GetParsedFilenameFromIdx(unsigned idx);

		//-----------------------------------------------

		/**
		* Clear: Clear compile information.
		*/
		void Clear();
		void Execute();

		void 						SetPrintOutCallback(void (*)(const char *));


		StackElement 					C_REF_InfoVariable_2_StackElement(SymbolInfo *ir_var, void *ptr_variable);

		int GetIdxClassFromIts_C_Type(const std::string & str_type){
			return script_class_factory->GetIdxClassFromIts_C_Type(str_type);
		}


		const char * getMetamethod(ByteCodeMetamethod op);
		void 												register_C_BaseSymbols(bool r){
			script_class_factory->register_C_BaseSymbols(r);
		}

		/**
		 * Register C function
		 */
		template <typename F>
		bool Register_C_Function( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_Function( function_name,function_ptr, registered_file,registered_line);
		}

		/**
		 * Register C variable
		 */
		template <typename V>
		 SymbolInfo * Register_C_Variable(const std::string & var_str,V var_ptr, const char *registered_file="",int registered_line=-1){
			 return script_class_factory->Register_C_Variable(var_str,var_ptr, typeid(V).name(), registered_file, registered_line);
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
		bool Register_C_Class(const std::string & class_name, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_Class<C>(class_name, registered_file,registered_line);
		}


		template<typename C>
		bool Register_C_ClassBuiltIn(const std::string & class_name, const char *registered_file=NULL,int registered_line=-1){
			return script_class_factory->Register_C_ClassBuiltIn<C>(class_name, registered_file, registered_line);
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
		template <typename _F>
		bool Register_C_FunctionMemberStatic(const char *function_name,_F fun, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_FunctionMemberStatic(function_name,fun, registered_file, registered_line);

		}

		/**
		 * Register C function as function member
		 */
		template <typename F>
		bool Register_C_FunctionAsFunctionMember( const char * function_name,F function_ptr, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_FunctionAsFunctionMember( function_name,function_ptr, registered_file,registered_line);
		}


		/**
		 * Register C Member var
		 */
		template <typename C, typename R,typename T>
		bool Register_C_VariableMember(const char *var_name, R T::*var_pointer, const char *registered_file="",int registered_line=-1){
			return script_class_factory->Register_C_VariableMember<C>(var_name,var_pointer,registered_file,registered_line);
		}

		//cpp binding
		// Helpers...
		inline StackElement varToStk(intptr_t var_trans, int idx_type);
		inline bool stkToVar(StackElement *stk_src, int idx_dst_type, intptr_t *result, std::string & error);


		template<typename T>
		static ScriptVarVector * stdVectorToScriptVarVector(const std::vector<T> & v,ZetScript *zs_instance){
			ScriptVarVector *vsv = new ScriptVarVector(zs_instance);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->push();
				//intptr_t uvar = (intptr_t)(v[i]);
				*stk = zs_instance->varToStk((intptr_t)(v[i]),zs_instance->GetIdxClassFromIts_C_Type(typeid(T).name()));
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
		 auto BindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto BindScriptFunctionBuilderBase(void **f, ScriptVar *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;



		bool GetScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj );


		template <  typename F>
		std::function<F> * BindScriptFunction(const std::string & function_access);


		 ~ZetScript();

	private:

		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedSourceInfo;


		//--------
		// VARS
		std::map<std::string,ConstantValueInfo *> 	 m_contantPool;
		std::vector<ParsedSourceInfo> 			 m_parsedSource;
		std::map<std::string,ConstantValueInfo *> 	 constant_pool;
		ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		NativeFunctionFactory * native_function_factory;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory *script_class_factory;


		ZetScript 		*zs;

		float eval_float;
		int eval_int;
		bool eval_bool;
		std::string eval_string;


		//===================================================================================================
		//
		// PRINT ASM INFO
		std::string getStrMovVar(Instruction * iao);
		std::string getStrTypeLoadValue(ScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction);
		void PrintGeneratedCode(ScriptFunction *sfo);

		//----

		// FUNCTIONS
		static 									void  print(const char *s);
		static 									void (* print_out_callback)(const char *);

	};

}

#include "VirtualMachine.h"
#include "ZetScript.inc"
#include "FunctionTraits.h"
