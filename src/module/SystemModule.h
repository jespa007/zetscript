/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringScriptObject;
	class DictionaryScriptObject;
	class ArrayScriptObject;
	class StringMod;

	zs_float	SystemModule_clock(ZetScript *_zs);
	void 	SystemModule_eval(ZetScript *zs, StringScriptObject *_so_str_eval, DictionaryScriptObject *_oo_param);
	void 	SystemModule_eval(ZetScript *zs, StringScriptObject *_so_str_eval);
	void 	SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args);

}

