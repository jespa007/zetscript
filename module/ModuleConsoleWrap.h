#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	ModuleConsoleWrap_readChar();
	void 	ModuleConsoleWrap_outln(ZetScript *zs,StackElement *str, StackElement *args);
	void	ModuleConsoleWrap_out(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ModuleConsoleWrap_errorln(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ModuleConsoleWrap_error(ZetScript *zs,StackElement *str, StackElement *args);

}

