#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;
	class ModuleSystem{
	public:
		static zs_int clock();
		static void makeReadOnly(StackElement *stk);
		static void eval(ZetScript *zs,StackElement *str_eval,StackElement *arg_names, StackElement *arg_values);
		static void assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);
	};

}

