/*
 *  This _file is distributed under the MIT License.
 *  See LICENSE _file for details.
 */
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
	#ifdef _WIN32
		#include <windows.h>
		#include <stdio.h>
		#include <conio.h>
		#include <tchar.h>
		#include <direct.h>
	#else
		#include <dlfcn.h>
		#include <sys/ipc.h>
		#include <sys/shm.h>
		#include <sys/ioctl.h>
		#include <termios.h>
	#endif
#else
	#if defined(_WIN32)
		#include <windows.h>
		#include <direct.h>
	#endif
#endif



#include        <memory.h>
#include        <memory>
#include        <math.h>
#include        <new>
#include 		<functional>
#include 		<float.h>
#include 		<cstdarg>
#include 		<stdexcept>
#include 		<chrono>

#include 		<typeinfo>
#include 		<cstddef>
#include 		<time.h>       // time_t, struct tm, time, gmtime

#include 		<type_traits>


// Prototypes & structs
#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif

// utils
#include "config.h"
#include "common.h"
#include "util/zs_util.h"

#include "StackElement.h"
#include "ByteCodeHelper.h"
#include "MetamethodHelper.h"
#include "Instruction.h"
#include "Symbol.h"


#include "scope/zs_scope.h"
#include "module/zs_module.h"
#include "script/zs_script.h"
#include "eval/eval.h"
#include "vm/vm.h"


#define ZETSCRIPT_VERSION_MAJOR 2
#define ZETSCRIPT_VERSION_MINOR 0
#define ZETSCRIPT_VERSION_PATCH 0

#define ZS_DECLARE_CONSTRUCTOR_DESTRUCTOR_FUNCTIONS(class_type) \
 class_type * class_type##_New(ZetScript *_zs){ return new class_type(_zs);} \
 void class_type##_Delete(ZetScript *_zs,class_type *ptr){ ZS_UNUSUED_PARAM(_zs);delete  (class_type *)(ptr);}


namespace zetscript{

	enum:uint16_t{
		 ZS_EVAL_OPTION_NO_EXECUTE				=0x1
		,ZS_EVAL_OPTION_PRINT_BYTE_CODE			=0x2
		,ZS_EVAL_OPTION_PRINT_ALL_BYTE_CODE		=0x4

	};

	struct VirtualMachine;
	class ScriptEval;
	class ZetScript{

	public:


		//===================================================================================================
		//
		// PRINT INFO

		static void printAllStructSizes();


		void printGeneratedCode(bool show_system_code=false);

		// PRINT  INFO
		//---------------------------------------------------------------------------------------------------------------------------------------

		ZetScript();

		inline VirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline ScriptTypeFactory *getScriptTypeFactory() { return script_type_factory;}

		StackElement	eval(const zs_string & expresion, const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	eval(const zs_string & expresion,unsigned short _eval_options, const char * _script_file_by_ref="", const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	evalFile(const zs_string & _filename,uint16_t _eval_options=0, EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);


		inline zs_map * getCompiledSymbolName(){
			return compiled_symbol_name;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
		//
		// STACKELEMENT
		//
		// it gives stackelement as string (the result should be deleted)
		void 					setStackElementUndefined();
		StackElement   			intToStackElement(zs_int);
		StackElement    		floatToStackElement(zs_float);
		StackElement    		boolToStackElement(bool);

		template<typename _C>
		_C stackElementTo(StackElement * _stk);

		template<typename _C>
		bool canStackElementCastTo(StackElement * _stack_element);

		template<typename _C>
		bool stackElementInstanceOf(StackElement * _stack_element);


		template<typename _C>
		_C stackElementTo(StackElement   _stk);

		zs_string		stackElementToString(StackElement *_stk,const zs_string & _format="");
		const char		*stackElementToString(char *_str_out,int _str_out_len, StackElement *_stk,const zs_string & _format="");
		StackElement	unwrapStackElement(StackElement _stk);

		zs_string		stackElementToStringTypeOf(StackElement *_stk);
		const char		*stackElementToStringTypeOf(char *_str_out, StackElement *_stk);

		void			stackElementAssign(StackElement *_stk_dst, const StackElement *_stk_src);
		StackElement 	toStackElement(zs_int ptr_var, short idx_builtin_type_var);
		bool			stackElementTo(StackElement * _stack_element, int _idx_builtin_type, zs_int *_ptr_var, zs_string  & _error);
		bool			canStackElementCastTo(StackElement * _stack_element, int _idx_builtin_type, bool _strict = false);

		template<typename _C>
		StackElement	toStackElement( _C _val);

		//
		// STACKELEMENT
		//
		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		//
		bool isFilenameAlreadyParsed(const zs_string & filename);
		//
		ObjectScriptObject * newObjectScriptObject();
		StringScriptObject * newStringScriptObject(const zs_string & _str="");
		ArrayScriptObject * newArrayScriptObject();

		template<typename _C>
		ClassScriptObject * newClassScriptObject(_C  *_instance=NULL);

		//-----------------------------------------------
		/**
		 * Register constants
		 */

		void registerConstant(const zs_string & var_name, int value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const zs_string & var_name, zs_float value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const zs_string & var_name, bool value, const char *registered_file="", short registered_line=-1);
		void registerConstant(const zs_string & var_name, const zs_string & v, const char *registered_file="", short registered_line=-1);
		void registerConstant(const zs_string & var_name, const char * v, const char *registered_file="", short registered_line=-1);

		// register object
		StackElement * registerStkConstantStringObject(const zs_string & _key,const zs_string & _value);
		StackElement * getStkConstantStringObject(const zs_string & _key);

		/**
		 * Register T Class. Return index registered class
		 */
		template<typename T>
		ScriptType * registerType(
				const zs_string & name
				, T  * (*_new_native_instance)(ZetScript *_zs)=NULL
				, void (*_delete_native_instance)(ZetScript *_zs,T *)=NULL
				, const char *_registered_file="",short _registered_line=-1
		){
			try{
				return script_type_factory->registerType<T>(name, _new_native_instance, _delete_native_instance, _registered_file,_registered_line);
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\")': %s",__func__,zs_rtti::demangle(typeid(T).name()).c_str(),name.c_str(),_ex.what());
				return NULL;
			}
		}

		/**
		 * Register T function
		 */
		template <typename F>
		void registerFunction( const zs_string & _name_script_function,F ptr_function, const char *_registered_file="",short _registered_line=-1){
			try{
				script_type_factory->registerFunction( _name_script_function,ptr_function, _registered_file,_registered_line);
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR("Exception in '%s(\"%s\")': %s",__func__,_name_script_function.c_str(),_ex.what());
			}
		}

		template<class T, class B>
		void extends(){
			script_type_factory->extends<T,B>();
		}

		template<typename T,typename F>
		void registerConstructor(
				 F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->registerConstructor<T>(function_type, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(...)': %s"
					,__func__
					,zs_rtti::demangle(typeid(T).name()).c_str()
					,_ex.what()
				);
			}
		}

		template <typename T,typename F>
		void	registerMemberFunction(
				const zs_string & _name_script_function
				,F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->registerMemberFunction<T>(_name_script_function,function_type, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,zs_rtti::demangle(typeid(T).name()).c_str()
					,_name_script_function.c_str()
					,_ex.what()
				);
			}
		}

		template <typename T,typename F>
		void	registerConstMemberProperty(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->registerConstMemberProperty<T>(
					_property_name
					,ptr_function
					, _registered_file
					,_registered_line
				);
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,zs_rtti::demangle(typeid(T).name()).c_str()
					,_property_name.c_str()
					,_ex.what()
				);
			}
		}


		template <typename T,typename F>
		void	registerMemberPropertyMetamethod(
				const zs_string & _property_name
				,const zs_string & _metamethod_name
				,F _ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->registerMemberPropertyMetamethod<T>(
					_property_name
					,_metamethod_name
					,_ptr_function
					,_registered_file
					,_registered_line
				);
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,zs_rtti::demangle(typeid(T).name()).c_str()
					,_property_name.c_str()
					,_ex.what()
				);
			}
		}



		/**
		 * Register Static Function Member Class
		 */
		template <typename T,typename F>
		void registerStaticMemberFunction(const zs_string & _name_script_function,F _fun, const char *_registered_file="",short _registered_line=-1){
			try{
				script_type_factory->registerStaticMemberFunction<T>(_name_script_function,_fun, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				ZS_THROW_RUNTIME_ERROR(
					"Exception in '%s<%s>(\"%s\",...)': %s"
					,__func__
					,zs_rtti::demangle(typeid(T).name()).c_str()
					,_name_script_function.c_str()
					,_ex.what()
				);
			}
		}

		//cpp binding
		void unrefLifetimeObject(ScriptObject *so);
		void makeScriptObjectShared(ScriptObject *so);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 8)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 8)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 9)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 9)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 10)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *_file,int _line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 10)>::type;




		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *_file,int _line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getScriptObject(const zs_string &function_access,ScriptObject **calling_obj,ScriptFunction **fun_obj );

		template <  typename F>
		std::function<F> bindScriptFunction(const zs_string & function_access, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(MemberFunctionScriptObject *_sf, const char *_file="", int _line=-1);


		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *_sf, ScriptObject *_calling_object, const char *_file="", int _line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *_sf, const char *_file="", int _line=-1);



		//-----------------------------------------------------------------------------------------------------------------------
		/**
		* clear: clear compile information.
		*/
		void clear();
		void saveState();

		void addUnresolvedSymbol(ScriptFunction *_sf, zs_int _idx_instruction);
		void link();

		 ~ZetScript();

	private:

		typedef struct {
			zs_string filename;
			//unsigned char *data;
		} ParsedFile;


		//--------
		// VARS
		zs_map 	 								*script_filenames_by_ref;
		zs_map 	 							   	*stk_constants;//,*stk_objects;
		zs_vector<ParsedFile *>					parsed_files;
		zs_vector<ScriptFunction *>				functions_with_unresolved_symbols;

		//ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptTypeFactory *script_type_factory;

		zs_float eval_float;
		zs_int eval_int;
		bool eval_bool;
		zs_string eval_string;
		int idx_current_global_variable_checkpoint;
		int idx_current_global_function_checkpoint;
		int idx_current_script_types_checkpoint;

		zs_map	*compiled_symbol_name;

		//===================================================================================================
		//
		// PRINT ASM INFO
		//----
		void clearMainFunction();
		StackElement evalInternal(const char * _code, unsigned short _options=0, const char  *_filename="",EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

		//void setClearGlobalVariablesCheckpoint();
		void resetParsedFiles();
		const char *getFilenameByRef(const char * _filename_by_ref);
		void clearGlobalVariables(int _idx_start_variable=ZS_UNDEFINED_IDX, int _idx_start_function=ZS_UNDEFINED_IDX);
		bool getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf);

		template<typename R>
		R checkAndConvertStackElement(StackElement *_stk, int _idx_return);


	};

}

#include 	"zetscript.tcc"


