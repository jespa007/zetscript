#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class SystemBuiltIn{
	public:
		// clock
		static zs_int clock();
		static void println(StackElement *str, StackElement *args);
		static void print(StackElement *str, StackElement *args);
		static void eval(StackElement *str, StackElement *args);
		static void makeReadOnly(StackElement *stk);
	};

}
