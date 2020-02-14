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

// Prototypes & structs
#ifdef __MEMMANAGER__
#include "memmgr.h"
#endif


#include "common.h"
#include "CByteCode.h"


// utils
#include "util/string.h"
#include "util/io.h"
#include "util/log.h"
#include "util/rtti.h"
#include "util/path.h"

#include "exception.h"
#include "CMemberFunctionPointer.h"

#include "var/CScriptVar.h"
#include "var/CScriptVarString.h"
#include "var/CScriptVarVector.h"
#include "var/CScriptVarFunctor.h"
#include "var/CScriptVarDictionary.h"
#include "CScope.h"
#include "CScopeFactory.h"
#include "CScriptClassBase.h"
#include "CScriptFunction.h"
#include "CScriptFunctionFactory.h"
#include "CScriptClass.h"
#include "CScriptClassFactory.h"
#include "CZetScript.h"
#include "CEval.h"


#define ZETSCRIPT_MAJOR_VERSION 2
#define ZETSCRIPT_MINOR_VERSION 0
#define ZETSCRIPT_PATCH_VERSION 0









