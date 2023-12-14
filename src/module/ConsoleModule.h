/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringScriptObject;
	class ArrayScriptObject;
	class StringMod;

	zs_int 	ConsoleModule_readChar(ScriptEngine *_script_engine);
	void 	ConsoleModule_outln(ScriptEngine *_script_engine,StackElement *str, StackElement *args);
	void	ConsoleModule_out(ScriptEngine *_script_engine,StackElement *str, StackElement *args);
	void 	ConsoleModule_errorln(ScriptEngine *_script_engine,StackElement *str, StackElement *args);
	void 	ConsoleModule_error(ScriptEngine *_script_engine,StackElement *str, StackElement *args);

}

