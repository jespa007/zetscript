/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringScriptObject;
	class ArrayScriptObject;
	class StringMod;

	zs_float	SystemModule_clock(ZetScript *_zs);
	void 	SystemModule_makeReadOnly(ZetScript *_zs,StackElement *stk);
	void 	SystemModule_eval(ZetScript *zs,StackElement *str_eval,StackElement *object_args);
	//void 	SystemModule_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);
	void 	SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args);

}

