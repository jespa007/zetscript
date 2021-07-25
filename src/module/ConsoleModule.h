#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	ConsoleModule_readChar();
	void 	ConsoleModule_outln(ZetScript *zs,StackElement *str, StackElement *args);
	void	ConsoleModule_out(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModule_errorln(ZetScript *zs,StackElement *str, StackElement *args);
	void 	ConsoleModule_error(ZetScript *zs,StackElement *str, StackElement *args);

}
