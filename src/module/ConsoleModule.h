/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class StringScriptObject;
	class ArrayScriptObject;
	class StringMod;

	zs_int 	ConsoleModule_readChar(ZetScript *_zs);
	void 	ConsoleModule_outln(ZetScript *zs,StackElement *str, StackElement *args);
	void	ConsoleModule_out(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModule_errorln(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModule_error(ZetScript *zs,StackElement *str, StackElement *args);

}

