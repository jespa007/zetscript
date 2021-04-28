#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;
	class ModuleConsole{
	public:
		static zs_int readChar();
		static void outln(ZetScript *zs,StackElement *str, StackElement *args);
		static void out(ZetScript *zs,StackElement *str, StackElement *args);
		static void errorln(ZetScript *zs,StackElement *str, StackElement *args);
		static void error(ZetScript *zs,StackElement *str, StackElement *args);

	};

}

