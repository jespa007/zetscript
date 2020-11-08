#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn;
	class SystemBuiltIn{
	public:
		static zs_int clock();
		static void makeReadOnly(StackElement *stk);

		static void printlnSf(ZetScript *zs,StackElement *str, StackElement *args);
		static void printSf(ZetScript *zs,StackElement *str, StackElement *args);
		static void evalSf(ZetScript *zs,StackElement *str, StackElement *args);

	};

}

