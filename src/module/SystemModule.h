/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringObject;
	class ObjectObject;
	class ArrayObject;
	class StringMod;

	zs_float	SystemModule_clock(ZetScript *_zs);
	void 	SystemModule_eval(ZetScript *zs, StringObject *_so_str_eval, ObjectObject *_oo_param);
	void 	SystemModule_eval(ZetScript *zs, StringObject *_so_str_eval);
	void 	SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args);

}

