#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	SystemModule_clock();
	void 	SystemModule_makeReadOnly(StackElement *stk);
	void 	SystemModule_eval(ZetScript *zs,StackElement *str_eval,StackElement *object_args);
	void 	SystemModule_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);
	void 	SystemModule_error(ZetScript *zs, StackElement *str, StackElement *args);

}

