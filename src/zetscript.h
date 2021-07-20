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

#ifdef _WIN32
#include <direct.h>
#endif


#include        <memory.h>
#include        <math.h>
#include        <random>
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
#include 		<time.h>       /* time_t, struct tm, time, gmtime */

#include 		<type_traits>

// Prototypes & structs
#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif

#include "config.h"
#include "common.h"
#include "BuiltinType.h"
#include "StackElement.h"


// utils
#include "util/zs_util.h"

#include "zs_exception.h"
#include "ByteCode.h"
#include "Instruction.h"
#include "Symbol.h"


#include "scope/zs_scope.h"
#include "module/zs_module.h"
#include "script/zs_script.h"





#include "eval/eval.h"
#include "zetscript.h"


#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0


#define ZS_CLASS_INHERITS_FROM(zs,C,B)											(zs)->classInheritsFrom<C,B>()
#define ZS_REGISTER_FUNCTION(zs,text,s) 										(zs)->registerFunction(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_VARIABLE(zs,text,s) 										(zs)->registerVariable(text,s,__FILE__, __LINE__)
#define ZS_REGISTER_CLASS(zs,class_type,s) 										(zs)->registerClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_SINGLETON_CLASS(zs,class_type,s)							(zs)->registerSingletonClass<class_type>(s,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_FUNCTION_STATIC(zs,class_type,s,ptr_fun)				(zs)->registerMemberFunctionStatic<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_FUNCTION(zs,class_type,s,ptr_fun)					(zs)->registerMemberFunction<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_FUNCTION(zs,class_type,s,ptr_fun)					(zs)->registerMemberFunction<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_SETTER(zs,class_type,s,ptr_fun)			(zs)->registerMemberAttributeSetter<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_GETTER(zs,class_type,s,ptr_fun)			(zs)->registerMemberAttributeGetter<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_POST_INCREMENT(zs,class_type,s,ptr_fun)	(zs)->registerMemberAttributePostIncrement<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_POST_DECREMENT(zs,class_type,s,ptr_fun)	(zs)->registerMemberAttributePostDecrement<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_PRE_INCREMENT(zs,class_type,s,ptr_fun)		(zs)->registerMemberAttributePreIncrement<class_type>(s,ptr_fun,__FILE__, __LINE__)
#define ZS_REGISTER_MEMBER_ATTRIBUTE_PRE_DECREMENT(zs,class_type,s,ptr_fun)		(zs)->registerMemberAttributePreDecrement<class_type>(s,ptr_fun,__FILE__, __LINE__)



#define ZS_BIND_SCRIPT_FUNCTION(zs,_T,access_name)						(zs)->bindScriptFunction<_T>(access_name, __FILE__, __LINE__)
#define	ZS_UNREF_LIFETIME_OBJECT(zs,so)									(zs)->unrefLifetimeObject(so);
#define ZS_EVAL(zs,s) 													(zs)->eval(s,0,"",__FILE__,__LINE__)


#define ZS_REGISTER_CONSTANT_VAR(zs,constant_name,v)					(zs)->registerConstantVariable(constant_name,v,__FILE__,__LINE__)


namespace zetscript{

	typedef enum{
		EVAL_OPTION_NO_EXECUTE=0x1<<0,
		EVAL_OPTION_SHOW_USER_CODE=0x1<<1,
		EVAL_OPTION_SHOW_SYSTEM_CODE=0x1<<2,
	}EvalOption;


	extern const char *	k_str_void_type;				// 	typeid(void).name()
	extern const char * k_str_zs_int_type_ptr;			//	typeid(zs_int *).name()
	extern const char * k_str_const_zs_int_type_ptr;	//	typeid(zs_int *).name()
	extern const char * k_str_float_type_ptr;			//	typeid(zs_float *).name()
	extern const char * k_str_const_float_type_ptr;		//	typeid(zs_float *).name()
	extern const char * k_str_string_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_char_type_ptr;			//	typeid(std::string *).name()
	extern const char * k_str_const_char_type_ptr;		//	typeid(std::string *).name()
	extern const char * k_str_bool_type_ptr;			//	typeid(bool *).name()
	extern const char * k_str_const_bool_type_ptr;		//	typeid(bool *).name()
	extern const char *	k_str_zs_int_type;				//	typeid(zs_int).name()

	extern const char * k_str_float_type;				//	typeid(int).name()
	extern const char * k_str_bool_type;				//	typeid(bool).name()
	extern const char * k_str_stack_element_type;		//	typeid(bool).name()



	struct VirtualMachine;
	class ScriptEval;
	class ZetScript{

	public:

		//===================================================================================================
		//
		// PRINT BYTE CODE INFO
		void printGeneratedCode(bool show_system_code=false);

		// PRINT BYTE CODE INFO
		//---------------------------------------------------------------------------------------------------------------------------------------

		ZetScript();

		inline VirtualMachine * getVirtualMachine() { return virtual_machine;}
		inline ScopeFactory * getScopeFactory() { return scope_factory;}
		inline ScriptFunctionFactory *getScriptFunctionFactory() { return script_function_factory;}
		inline ScriptClassFactory *getScriptClassFactory() { return script_class_factory;}

		StackElement	eval(const std::string & expresion,unsigned short options=0, const char * filename="", const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	evalFile(const std::string & filename,unsigned short options=0, const char *__invoke_file__="", int __invoke_line__=-1);

		// CONSTANT TOOLS
		StackElement * registerStkStringObject(const std::string & key_name,const std::string & const_name);
		StackElement * getStkStringObject(const std::string & const_name);

		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		bool isFilenameAlreadyParsed(const std::string & filename);
		//-----------------------------------------------
		/**
		 * Register C variable
		 */
		template <typename V>
		 void registerVariable(const std::string & var_name,V var_ptr, const char *registered_file="",short registered_line=-1){
			 script_class_factory->registerNativeGlobalVariable(var_name,var_ptr, registered_file, registered_line);
		 }

		void registerConstantVariable(const std::string & var_name, int value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,value, registered_file, registered_line);
		}

		void registerConstantVariable(const std::string & var_name, bool value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,value, registered_file, registered_line);
		}

		void registerConstantVariable(const std::string & var_name, float value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,value, registered_file, registered_line);
		}

		void registerConstantVariable(const std::string & var_name, double value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,(float)value, registered_file, registered_line);
		}


		void registerConstantVariable(const std::string & var_name, const std::string & value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,value, registered_file, registered_line);
		}

		void registerConstantVariable(const std::string & var_name, const char *value, const char *registered_file="", short registered_line=-1){
			script_class_factory->registerConstantVariable(var_name,value, registered_file, registered_line);
		}

		/**
		 * Register C function
		 */
		template <typename F>
		void registerFunction( const char * function_name,F function_ptr, const char *registered_file="",short registered_line=-1){
			script_class_factory->registerNativeGlobalFunction( function_name,function_ptr, registered_file,registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		 void registerSingletonClass(const std::string & class_name, const char *registered_file="",short registered_line=-1){
			script_class_factory->registerNativeSingletonClass<C>(class_name, registered_file, registered_line);
		}

		/**
		 * Register C Class. Return index registered class
		 */

		template<typename C>
		ScriptClass * registerClass(const std::string & class_name, C  * (*_constructor)(), void (*_destructor)(C *), const char *registered_file="",short registered_line=-1){
			return script_class_factory->registerNativeClassStatic<C>(class_name, _constructor, _destructor, registered_file,registered_line);
		}

		template<class C, class B>
		void classInheritsFrom(){
			script_class_factory->nativeClassInheritsFrom<C,B>();
		}

		void	registerBaseSymbols(bool r){
			script_class_factory->registerNativeBaseSymbols(r);
		}

		template<typename C,typename F>
		void registerConstructor(
				 F function_type
				 , const char *registered_file=""
				,short registered_line=-1){

			script_class_factory->registerNativeConstructor<C>(function_type, registered_file,registered_line );
		}


		template <typename C, typename R>
		void registerNativeStaticConstMember(
				const char *var_name
				, const R var_pointer
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeStaticConstMember<C>(var_name,var_pointer, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberFunction(
				const char *function_name
				,F function_type
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberFunction<C>(function_name,function_type, registered_file,registered_line );
		}


		template <typename C,typename F>
		void	registerMemberAttributeSetter(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributeSetter<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberAttributeGetter(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributeGetter<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberAttributePostIncrement(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributePostIncrement<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberAttributePostDecrement(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributePostDecrement<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberAttributePreIncrement(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributePreIncrement<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		template <typename C,typename F>
		void	registerMemberAttributePreDecrement(
				const char *attr_name
				,F ptr_function
				 , const char *registered_file=""
				,short registered_line=-1
		){
			script_class_factory->registerNativeMemberAttributePreDecrement<C>(attr_name,ptr_function, registered_file,registered_line );
		}

		/**
		 * Register Static Function Member Class
		 */
		template <typename C,typename F>
		void registerMemberFunctionStatic(const char *function_name,F fun, const char *registered_file="",short registered_line=-1){
			script_class_factory->registerNativeMemberFunctionStatic<C>(function_name,fun, registered_file, registered_line);
		}

		//cpp binding
		// Helpers...
		inline StackElement convertVarToStackElement(zs_int var_ptr, short idx_builtin_type);
		inline bool convertStackElementToVar(StackElement *stack_element, int idx_builtin_type, zs_int *result, std::string & error);

		inline void unrefLifetimeObject(ScriptObject *so);

		template<typename T>
		static ScriptObjectVector * convertStdVectorToScriptObjectVector(const std::vector<T> & v,ZetScript *zs_instance){
			ScriptObjectVector *vsv = ZS_NEW_OBJECT_VECTOR(zs_instance);

			for ( unsigned i = 0; i < v.size(); i++){
				StackElement *stk = vsv->pushNewUserSlot();
				//zs_int uvar = (zs_int)(v[i]);
				*stk = zs_instance->convertVarToStackElement((zs_int)(v[i]),zs_instance->script_class_factory->getIdxClassFromItsNativeType(typeid(T).name()));
			}

			return vsv;
		}

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 0 PARAMS
		//
		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<std::is_same<R,void>::value>::type;


		template <typename R,typename T>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
		->typename std::enable_if<!std::is_same<R,void>::value>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 1 PARAMS
		//
		// template for last parameter argIdx == 1
		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun ,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		template<typename R,typename T,  typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line,void **ptr_fun ,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 1)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 2 PARAMS
		//
		// template when parameters argIdx == 2
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 2)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 3 PARAMS
		//
		// template when parameters argIdx == 3
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 3)>::type;



		//--------------------------------------------------------------------------------------------------------------------
		//
		// 4 PARAMS
		//
		// template when parameters argIdx == 4
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 4)>::type;


		//--------------------------------------------------------------------------------------------------------------------
		//
		// 5 PARAMS
		//
		// template when parameters argIdx == 5
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;



		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 5)>::type;

		//--------------------------------------------------------------------------------------------------------------------
		//
		// 6 PARAMS
		//
		// template when parameters argIdx == 6
		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(std::is_same<R,void>::value) && (sizeof...(ArgTypes) == 6)>::type;


		template <typename R,typename T, typename... ArgTypes>
		auto bindScriptFunctionBuilder(const char *file,int line, void **ptr_fun,ScriptObjectClass *calling_obj,ScriptFunction *fun_obj)
			-> typename std::enable_if<(!std::is_same<R,void>::value) &&(sizeof...(ArgTypes) == 6)>::type;

		//
		//
		//--------------------------------------------------------------------------------------------------------------------

		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *file,int line,void **ptr_fun, ScriptObjectClass *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity > 0)>::type;


		 template <typename F, std::size_t... Is>
		 auto bindScriptFunctionBuilderBase(const char *file,int line,void **ptr_fun, ScriptObjectClass *calling_obj,ScriptFunction *fun_obj,IndexSequence<Is...>)
		 -> typename std::enable_if<(F::arity == 0)>::type;

		void getScriptObject(const std::string &function_access,ScriptObjectClass **calling_obj,ScriptFunction **fun_obj );

		template <  typename F>
		std::function<F> * bindScriptFunction(const std::string & function_access, const char *file="", int line=-1);

		template <  typename F>
		std::function<F> * bindScriptFunction(ScriptFunction *sf, ScriptObjectClass *calling_object, const char *file="", int line=-1);

		template <  typename F>
		std::function<F> * bindScriptFunction(ScriptFunction *sf, const char *file="", int line=-1);



		//-----------------------------------------------------------------------------------------------------------------------
		/**
		* clear: clear compile information.
		*/
		void reset();
		void clear();
		void saveState();


		 ~ZetScript();

	private:

		typedef struct {
			std::string filename;
			//unsigned char *data;
		} ParsedFile;


		//--------
		// VARS
		std::map<std::string,StackElement *> 	 	*stk_constants;
		std::vector<ParsedFile *> 			 		parsed_files;

		//ScriptEval * eval_obj;
		VirtualMachine * virtual_machine;
		ScopeFactory * scope_factory;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory *script_class_factory;

		zs_float eval_float;
		zs_int eval_int;
		bool eval_bool;
		std::string eval_string;
		int idx_current_global_variable_checkpoint;

		//===================================================================================================
		//
		// PRINT ASM INFO
		//----
		StackElement evalInternal(const char * code, unsigned short options=0, const char  *filename="", const char *__invoke_file__="", int __invoke_line__=-1);

		// FUNCTIONS
		static 									void  print(const char *s);

		static 									void (* print_out_callback)(const char *);

		//void setClearGlobalVariablesCheckpoint();
		void resetParsedFiles();
		void clearGlobalVariables(int _idx_start=ZS_IDX_UNDEFINED);


	};

}

#include	"vm/vm.h"
#include 	"zetscript.tcc"

