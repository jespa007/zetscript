/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#ifdef _MSC_VER
	#define PRAGMA_PUSH					__pragma(warning(push))
	#define PRAGMA_DISABLE_WARNING(W)	__pragma(warning(disable:W))
	#define PRAGMA_POP  				__pragma(warning(pop))
#else
	#define PRAGMA_PUSH
	#define PRAGMA_DISABLE_WARNING(w)
	#define PRAGMA_POP
#endif

#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif

#define ZS_IDX_UNDEFINED 						-1
#define IDX_ZS_SCRIPT_FUNCTION_EVAL				-10
#define MAX_NATIVE_FUNCTION_ARGS				10

#define GET_METAMETHOD(m)												((this->zs))->getMetamethod(m)
#define IS_IDX_TYPE_INSTANCEOF(zs,idx_script_type,idx_type_instance)	((this->script_type_factory))->scriptTypeInheritsFrom(idx_script_type,idx_type_instance)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#define ZS_MAX(a,b) (a)>(b)?(a):(b)
#define ZS_MIN(a,b) (a)<(b)?(a):(b)

#define		ZS_MAX_STR_BUFFER 					4096
#define 	ZS_UNUSUED_PARAM(x) 				((void)(x))
#define 	ZS_UNUSUED_2PARAMS(x1,x2) 			((void)(x1),(void)(x2))
#define 	ZS_UNUSUED_3PARAMS(x1,x2,x3) 		((void)(x1),(void)(x2),(void)(x3))
#define 	ZS_UNUSUED_4PARAMS(x1,x2,x3,x4)		((void)(x1),(void)(x2),(void)(x3),(void)(x4))

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

namespace zetscript{
	typedef intptr_t zs_int;
	typedef int8_t zs_char;
}

