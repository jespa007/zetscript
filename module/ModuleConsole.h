#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;
	class ModuleConsole{
	public:
		static zs_int readChar();
		static void println(ZetScript *zs,StackElement *str, StackElement *args);
		static void print(ZetScript *zs,StackElement *str, StackElement *args);
		static void errorln(ZetScript *zs,StackElement *str, StackElement *args);
		static void error(ZetScript *zs,StackElement *str, StackElement *args);

	};

}

