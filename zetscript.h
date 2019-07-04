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
#include 		<mutex>


// Prototypes & structs
#ifdef __MEMMANAGER__
#include "MEM_ram.h"
#endif





//#include "CSharedPointerManager.h"
#include "Common.h"
#include "CZetScriptUtils.h"
#include "exception.h"
#include "functor.h"
//#include "string_utils.h"
//#include "io_utils.h"
//#include "logger.h"
#include "var/CScriptVariable.h"
#include "var/CStringScriptVariable.h"
#include "var/CVectorScriptVariable.h"
#include "var/CFunctorScriptVariable.h"
#include "var/CStructScriptVariable.h"
#include "CScope.h"
#include "CScopeFactory.h"
#include "CCommonClassFunctionBase.h"
#include "CScriptFunction.h"
#include "CScriptFunctionFactory.h"
#include "CScriptClass.h"
#include "CScriptClassFactory.h"
#include "CVirtualMachine.h"
#include "eval/CEval.h"

#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0


#define CURRENT_VM	zs::get_vm()


//#define ZS_WRITE_ERROR_MSG 		writeErrorMsg
//#define ZS_GET_ERROR_MSG		CZetScript::getErrorMsg
//#define ZS_CLEAR_ERROR_MSG		CZetScript::clearErrorMsg

//#define MAX_BUFFER_STR_ERROR	4096
#define MAX_BUFFER_AUX_TMP		4096

#define REGISTER_C_FUNCTION(text,s) 			zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_FunctionInt(text,s)
#define REGISTER_C_VARIABLE(text,s) 			zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_VariableInt(text,&s,typeid(decltype(&s)).name())

#define REGISTER_C_CLASS 						zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_ClassInt
#define REGISTER_C_SINGLETON_CLASS				zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_SingletonClassInt
#define REGISTER_C_STATIC_FUNCTION_MEMBER		zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_StaticFunctionMemberInt

#define REGISTER_C_VARIABLE_MEMBER				zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_VariableMemberInt
#define REGISTER_C_FUNCTION_MEMBER				zs::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zs::CScriptClassFactory::getInstance()->register_C_FunctionMemberInt //<o>(s,&f)

namespace zs{


	 void	set_callback_on_error(tPrintFunctionCallback _fun);

	 int eval_int_value(const std::string & str_to_eval);
	 bool eval_bool_value(const std::string & str_to_eval);
	 float eval_float_value(const std::string & str_to_eval);
	 std::string eval_string_value(const std::string & str_to_eval);

	/**
	 * Clear: Clear compile information.
	 */
	 void clear();
	 void execute();

	 CVirtualMachine *get_vm();

	 bool eval_string(const std::string & expresion, bool execute=true, const char *filename_ref=NULL,bool show_bytecode=false);
	 bool eval_file(const std::string & filename,bool execute=true,bool show_bytecode=false);

		/**
		 * REGISTER C Stuff
		 */

		/**
		 * Register C function
		 */
		template <typename F>
		bool register_C_FunctionInt( const char * function_name,F function_ptr);

		/**
		 * Register C variable
		 */
		 tVariableSymbolInfo * register_C_VariableInt(const std::string & var_str,void * var_ptr, const std::string & var_type);
		 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);


		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		 bool register_C_SingletonClassInt(const std::string & class_name);

		/**
		 * Register C Class. Return index registered class
		 */
		template<class _T>
		bool register_C_ClassInt(const std::string & class_name);


		template<class _T, class _B>
		bool class_C_BaseOfInt();

		/**
		 * Register C Member function Class
		 */
		template < typename _C, typename _R, class _T, typename..._A>
		bool register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) );

		/**
		 * Register C Member function Class
		 */
		template <typename _T, typename _F>
		bool register_C_StaticFunctionMemberInt(const char *function_name,_F function_type);


		/**
		 * Register C Member var
		 */
		template <typename _C, typename _R,typename _T>
		bool register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer);

	 void destroy();





}

#include "zetscript.inc"
#include "helper/CppBindingHelper.h"

