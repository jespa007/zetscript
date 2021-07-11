#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	ConsoleModuleWrap_readChar();
	void 	ConsoleModuleWrap_outln(ZetScript *zs,StackElement *str, StackElement *args);
	void	ConsoleModuleWrap_out(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModuleWrap_errorln(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModuleWrap_error(ZetScript *zs,StackElement *str, StackElement *args);

}

