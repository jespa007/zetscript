#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringMod;

	zs_int 	SystemModuleWrap_clock();
	void 	SystemModuleWrap_makeReadOnly(StackElement *stk);
	void 	SystemModuleWrap_eval(ZetScript *zs,StackElement *str_eval,StackElement *object_args);
	void 	SystemModuleWrap_assert(ZetScript *zs,bool *chk_assert, StackElement *str, StackElement *args);
	void 	SystemModuleWrap_error(ZetScript *zs, StackElement *str, StackElement *args);

}

