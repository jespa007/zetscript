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
	 void destroy();





}

#include "helper/CppBindingHelper.h"

