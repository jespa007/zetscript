#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn;
	class SystemBuiltIn{
	public:
		ZetScript *zs;
		StringBuiltIn *string_builtin;


		static void constructorSf(SystemBuiltIn *system_builtin,ZetScript *zs, StringBuiltIn *string_builtin);
		// clock
		static zs_int clock();
		static void printlnSf(SystemBuiltIn *system_builtin, StackElement *str, StackElement *args);
		static void printSf(SystemBuiltIn *system_builtin,StackElement *str, StackElement *args);
		static void evalSf(SystemBuiltIn *system_builtin,StackElement *str, StackElement *args);


		static void makeReadOnly(StackElement *stk);
	private:
		void println(StackElement *str, StackElement *args);
		void print(StackElement *str, StackElement *args);
		void eval(StackElement *str, StackElement *args);

	};

}

