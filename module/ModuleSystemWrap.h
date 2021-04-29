#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	ModuleSystemWrap_clock();
	void 	ModuleSystemWrap_makeReadOnly(StackElement *stk);
	void 	ModuleSystemWrap_eval(ZetScript *zs,StackElement *str_eval,StackElement *object_args);
	void 	ModuleSystemWrap_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);

}

