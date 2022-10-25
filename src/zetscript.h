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
#include        <memory>
#include        <math.h>
#include		<vector>
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
#include "zs_exception.h"
#include "util/zs_util.h"

#include "Type.h"
#include "StackElement.h"

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
 class_type * class_type##Wrap_New(ZetScript *_zs){ return new class_type(_zs);} \
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

		StackElement	eval(const zs_string & expresion, const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	eval(const zs_string & expresion,unsigned short _eval_options, const char * _script_file_by_ref="", const char *__invoke_file__="", int __invoke_line__=-1);
		StackElement	evalFile(const zs_string & _filename,unsigned short _eval_options=0, EvalData *_eval_data_from=NULL, const char *__invoke_file__="", int __invoke_line__=-1);


		inline zs_map * getCompiledSymbolName(){
			return compiled_symbol_name;
		}

		//---------------------------------------------------------------------------------------------------------------------------------------
		// FILE MANAGEMENT
		//
		bool isFilenameAlreadyParsed(const zs_string & filename);
		//
		ObjectScriptObject * newScriptObjectObject();
		StringScriptObject * newStringScriptObject();
		VectorScriptObject * newVectorScriptObject();

		template<typename _C>
		ClassScriptObject * newClassScriptObject(_C  *_instance=NULL);

		//-----------------------------------------------
		/**
		 * Register C variable
		 */
		/*template <typename V>
		 void bindVariable(const zs_string & _var_name,V var_ptr, const char *_registered_file="",short _registered_line=-1){
			try{
				script_type_factory->bindGlobalVariable(_var_name,var_ptr, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		 }*/

		void bindConstantVariable(const zs_string & _var_name, int value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		}

		void bindConstantVariable(const zs_string & _var_name, bool value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		}

		void bindConstantVariable(const zs_string & _var_name, float value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		}

		void bindConstantVariable(const zs_string & _var_name, double value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,(float)value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		}


		void bindConstantVariable(const zs_string & _var_name, const zs_string & value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}
		}

		void bindConstantVariable(const zs_string & _var_name, const char *value, const char *_registered_file="", short _registered_line=-1){
			try{
				script_type_factory->bindConstantVariable(_var_name,value, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\",...)': %s",__func__,_var_name.c_str(),_ex.getDescription());
			}

		}

		/**
		 * Register C Class. Return index registered class
		 */
		template<typename C>
		ScriptType * bindType(const zs_string & str_script_type, C  * (*_constructor)(ZetScript *_zs)=NULL, void (*_destructor)(ZetScript *_zs,C *)=NULL, const char *_registered_file="",short _registered_line=-1){
			try{
				return script_type_factory->bindType<C>(str_script_type, _constructor, _destructor, _registered_file,_registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\")': %s",__func__,zs_rtti::demangle(typeid(C).name()).c_str(),str_script_type.c_str(),_ex.getDescription());
				return NULL;
			}
		}

		/**
		 * Register C function
		 */
		template <typename F>
		void bindFunction( const zs_string & _name_script_function,F ptr_function, const char *_registered_file="",short _registered_line=-1){
			try{
				script_type_factory->bindFunction( _name_script_function,ptr_function, _registered_file,_registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s(\"%s\")': %s",__func__,_name_script_function.c_str(),_ex.getDescription());
			}
		}

		template<class C, class B>
		void extends(){
			script_type_factory->extends<C,B>();
		}

		template<typename C,typename F>
		void bindConstructor(
				 F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindConstructor<C>(function_type, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_ex.getDescription());
			}
		}

		template <typename C, typename R>
		void bindStaticConstMember(
				const zs_string & _var_name
				, const R _var_pointer
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindStaticConstMember<C>(_var_name,_var_pointer, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_var_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberFunction(
				const zs_string & _name_script_function
				,F function_type
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberFunction<C>(_name_script_function,function_type, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()).c_str(),_name_script_function.c_str(),_ex.getDescription());
			}
		}


		template <typename C,typename F>
		void	bindMemberPropertySetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertySetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyGetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyGetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()).c_str(),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyNeg(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyNeg<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()).c_str(),_property_name.c_str(),_ex.getDescription());
			}
		}


		template <typename C,typename F>
		void	bindMemberPropertyAddSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyAddSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertySubSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertySubSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyMulSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyMulSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyDivSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyDivSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyModSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyModSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyXorSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyXorSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyOrSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyOrSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyAndSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyAndSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyShrSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyShrSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyShlSetter(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyShlSetter<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyPostInc(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyPostInc<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyPostDec(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyPostDec<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyPreInc(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyPreInc<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		template <typename C,typename F>
		void	bindMemberPropertyPreDec(
				const zs_string & _property_name
				,F ptr_function
				 , const char *_registered_file=""
				,short _registered_line=-1
		){
			try{
				script_type_factory->bindMemberPropertyPreDec<C>(_property_name,ptr_function, _registered_file,_registered_line );
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_property_name.c_str(),_ex.getDescription());
			}
		}

		/**
		 * Register Static Function Member Class
		 */
		template <typename C,typename F>
		void bindStaticMemberFunction(const zs_string & _name_script_function,F _fun, const char *_registered_file="",short _registered_line=-1){
			try{
				script_type_factory->bindStaticMemberFunction<C>(_name_script_function,_fun, _registered_file, _registered_line);
			}catch(zs_exception & _ex){
				THROW_RUNTIME_ERROR("Exception in '%s<%s>(\"%s\",...)': %s",__func__,zs_rtti::demangle(typeid(C).name()),_name_script_function.c_str(),_ex.getDescription());
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
		void clearGlobalVariables(int _idx_start_variable=ZS_IDX_UNDEFINED, int _idx_start_function=ZS_IDX_UNDEFINED);
		bool getFunctionWithUnresolvedSymbolExists(ScriptFunction *_sf);


	};

}

#include 	"zetscript.tcc"


