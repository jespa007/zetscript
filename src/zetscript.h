/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
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
	#include <fcntl.h>
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
#include        <math.h>
#include		<regex>
#include		<map>
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

#include "Type.h"
#include "StackElement.h"
#include "zs_exception.h"
#include "ByteCode.h"
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
 class_type * class_type##Wrap_New(ZetScript *_zs){ ZS_UNUSUED_PARAM(_zs);return new class_type();} \
 void class_type##Wrap_Delete(ZetScript *_zs,class_type *ptr){ ZS_UNUSUED_PARAM(_zs);delete  (class_type *)(ptr);}


namespace zetscript{

	typedef enum{

		EVAL_OPTION_NO_EXECUTE				=0x1
		,EVAL_OPTION_SHOW_BYTE_CODE			=0x2
		,EVAL_OPTION_SHOW_ALL_BYTE_CODE		=0x4

	}EvalOption;

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

		StackElement	eval(const std::string & expresion, const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	eval(const std::string & expresion,unsigned short _eval_options, const char * _script_file_by_ref="", const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	evalFile(const std::string & _filename,unsigned short _eval_options=0, EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);


		inline std::map<std::string,std::string *> * getCompiledSymbolName(){
			return compiled_symbol_name;
		}

		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		//
		bool isFilenameAlreadyParsed(const std::string & filename);
		//
		//------------------------------------------------------------------------------------------------------------------------------------
		//
		// SHAREABLE OBJECTS
		// This methods allows to create a shareable script objects string, vector and object. The user has to delete when is not used anymore
		//
		ScriptObjectObject * newShareableScriptObjectObject();
		ScriptObjectString * newShareableScriptObjectString();
		ScriptObjectVector * newShareableScriptObjectVector();

		//
		//------------------------------------------------------------------------------------------------------------------------------------

		template<typename _C>
		ScriptObjectClass * newScriptObjectClass();

		//-----------------------------------------------
		/**
		 * Register C variable
		 */
		template <typename V>
		 void bindVariable(const std::string & var_name,V var_ptr, const char *registered_file="",short registered_line=-1){
			 script_type_factory->bindGlobalVariable(var_name,var_ptr, registered_file, registered_line);
		 }

		void bindConstantVariable(const std::string & var_name, int value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,value, registered_file, registered_line);
		}

		void bindConstantVariable(const std::string & var_name, bool value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,value, registered_file, registered_line);
		}

		void bindConstantVariable(const std::string & var_name, float value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,value, registered_file, registered_line);
		}

		void bindConstantVariable(const std::string & var_name, double value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,(float)value, registered_file, registered_line);
		}


		void bindConstantVariable(const std::string & var_name, const std::string & value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,value, registered_file, registered_line);
		}

		void bindConstantVariable(const std::string & var_name, const char *value, const char *registered_file="", short registered_line=-1){
			script_type_factory->bindConstantVariable(var_name,value, registered_file, registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		ScriptType * bindType(const std::string & str_script_type, C  * (*_constructor)(ZetScript *_zs)=NULL, void (*_destructor)(ZetScript *_zs,C *)=NULL, const char *registered_file="",short registered_line=-1){
			return script_type_factory->bindType<C>(str_script_type, _constructor, _destructor, registered_file,registered_line);
		}

		/**
		 * Register C function
		 */
		template <typename F>
		void bindFunction( const std::string & _name_script_function,F ptr_function, const char *registered_file="",short registered_line=-1){
			script_type_factory->bindFunction( _name_script_function,ptr_function, registered_file,registered_line);
		}

		template<class C, class B>
		void extends(){
			script_type_factory->extends<C,B>();
		}

		template<typename C,typename F>
		void bindConstructor(
				 F function_type
				 , const char *registered_file=""
				,short registered_line=-1){

			script_type_factory->bindConstructor<C>(function_type, registered_file,registered_line );
		}

		template <typename C, typename R>
		void bindStaticConstMember(
				const std::string & var_name
				, const R var_pointer
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindStaticConstMember<C>(var_name,var_pointer, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberFunction(
				const std::string & _name_script_function
				,F function_type
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberFunction<C>(_name_script_function,function_type, registered_file,registered_line );
		}


		template <typename C,typename F>
		void	bindMemberPropertySetter(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertySetter<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberPropertyGetter(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertyGetter<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberPropertyPostIncrement(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertyPostIncrement<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberPropertyPostDecrement(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertyPostDecrement<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberPropertyPreIncrement(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertyPreIncrement<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	bindMemberPropertyPreDecrement(
				const std::string & _property_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_type_factory->bindMemberPropertyPreDecrement<C>(_property_name,ptr_function, registered_file,registered_line );
		}

		/**
		 * Register Static Function Member Class
		 */
		template <typename C,typename F>
		void bindStaticMemberFunction(const std::string & _name_script_function,F fun, const char *registered_file="",short registered_line=-1){
			script_type_factory->bindStaticMemberFunction<C>(_name_script_function,fun, registered_file, registered_line);
		}

		//cpp binding
		void unrefLifetimeObject(ScriptObject *so);
		void makeScriptObjectShared(ScriptObject *so);


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun ,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 7 PARAMS
		//
		// template when parameters argIdx == 7
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 7)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 7)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 8 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 8)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 8)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 9 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 9)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 9)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 10 PARAMS
		//
		// template when parameters argIdx == 8
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 10)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObject *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 10)>::type;




		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *file,int line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *file,int line,void **ptr_fun, ScriptObject *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getScriptObject(const std::string &function_access,ScriptObject **calling_obj,ScriptFunction **fun_obj );

		template <  typename F>
		std::function<F> bindScriptFunction(const std::string & function_access, const char *file="", int line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(ScriptObjectMemberFunction *sf, const char *file="", int line=-1);


		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *sf, ScriptObject *calling_object, const char *file="", int line=-1);

		template <  typename F>
		std::function<F> bindScriptFunction(ScriptFunction *sf, const char *file="", int line=-1);



		//-----------------------------------------------------------------------------------------------------------------------
		/**
		* clear: clear compile information.
		*/
		void clear();
		void saveState();

		void addUnresolvedSymbol(ScriptFunction *sf, zs_int idx_instruction);
		void link();

		 ~ZetScript();

	private:

		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedFile;


		//--------
		// VARS
		std::map<std::string,std::string *>			*script_filenames_by_ref;
		std::vector<ParsedFile *>					parsed_files;
		std::vector<ScriptFunction *>				functions_with_unresolved_symbols;

		//ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptTypeFactory *script_type_factory;

		zs_float eval_float;
		zs_int eval_int;
		bool eval_bool;
		std::string eval_string;
		int idx_current_global_variable_checkpoint;
		int idx_current_global_function_checkpoint;
		int idx_current_script_types_checkpoint;

		std::map<std::string,std::string*>	*compiled_symbol_name;

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
		void clearGlobalVariables(int _idx_start_variable=ZS_IDX_UNDEFINED, int _idx_start_function=ZS_IDX_UNDEFINED);
		bool getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf);


	};

}

#include 	"zetscript.tcc"


