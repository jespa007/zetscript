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
#include "Config.h"
#include "Common.h"


#include "System.h"
#include "Character.h"
#include "Buffer.h"
#include "Vector.h"
#include "String.h"
#include "Exception.h"
#include "Integer.h"
#include "Float.h"
#include "Boolean.h"
#include "List.h"
#include "File.h"
#include "Log.h"
#include "Rtti.h"
#include "Path.h"
#include "MapIntIterator.h"
#include "MapInt.h"
#include "MapStringIterator.h"
#include "MapString.h"
#include "Directory.h"
#include "TimeSpan.h"
#include "DateTime.h"
#include "Console.h"
#include "Memory.h"
#include "Vector.tcc"



