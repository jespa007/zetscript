/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringScriptObject;
	class ObjectScriptObject;
	class ArrayScriptObject;
	class StringMod;

	zs_float	SystemModule_clock(ScriptEngine *_script_engine);
	void 	SystemModule_eval(ScriptEngine *_script_engine, StringScriptObject *_so_str_eval, ObjectScriptObject *_oo_param);
	void 	SystemModule_eval(ScriptEngine *_script_engine, StringScriptObject *_so_str_eval);
	void 	SystemModule_error(ScriptEngine *_script_engine, StackElement *str, StackElement *args);

}

