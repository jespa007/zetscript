#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;
	class ModuleSystem{
	public:
		static zs_int clock();
		static void makeReadOnly(StackElement *stk);
		static void eval(ZetScript *zs,StackElement *str, StackElement *args);
		static void assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);
	};

}

