#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;
	class ModuleSystem{
	public:
		static zs_int clock();
		static zs_int readChar();
		static void makeReadOnly(StackElement *stk);

		static void printlnSf(ZetScript *zs,StackElement *str, StackElement *args);
		static void printSf(ZetScript *zs,StackElement *str, StackElement *args);
		static void evalSf(ZetScript *zs,StackElement *str, StackElement *args);

	};

}

