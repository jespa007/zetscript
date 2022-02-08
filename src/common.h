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
#define ZS_IDX_EVAL_FUNCTION					-10
#define MAX_NATIVE_FUNCTION_ARGS				6

#define GET_METAMETHOD(m)														((this->zs))->getMetamethod(m)
#define IS_IDX_TYPE_INSTANCEOF(zs,idx_type,idx_type_instance)	((this->script_type_factory))->isScriptClassTypeInheritsFrom(idx_type,idx_type_instance)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#ifndef MAX
#define MAX(a,b) (a)>(b)?(a):(b)
#endif


#ifndef MIN
#define MIN(a,b) (a)<(b)?(a):(b)
#endif

#define 	ZS_UNUSUED_PARAM(x) 				((void)(x))
#define 	ZS_UNUSUED_2PARAMS(x1,x2) 			((void)(x1),(void)(x2))
#define 	ZS_UNUSUED_3PARAMS(x1,x2,x3) 		((void)(x1),(void)(x2),(void)(x3))
#define 	ZS_UNUSUED_4PARAMS(x1,x2,x3,x4)		((void)(x1),(void)(x2),(void)(x3),(void)(x4))

namespace zetscript{
	typedef intptr_t zs_int;
	typedef char zs_char;
}

