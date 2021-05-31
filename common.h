/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif

#define ZS_IDX_UNDEFINED 						-1
#define MAX_NATIVE_FUNCTION_ARGS				6

#define GET_METAMETHOD(m)														((this->zs))->getMetamethod(m)
#define IS_IDX_BUILTIN_TYPE_INSTANCEOF(zs,idx_class,idx_class_instance)	((this->script_class_factory))->isClassInheritsFrom(idx_class,idx_class_instance)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

namespace zetscript{
	typedef intptr_t zs_int;
	typedef char zs_char;
};

