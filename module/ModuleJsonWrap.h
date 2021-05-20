#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;

	void ModuleJsonWrap_deserialize(ScriptObjectString *str_json);
	ScriptObjectString * ModuleJsonWrap_serialize(ZetScript *zs,StackElement *stk,bool *minimized);
	ScriptObjectString * ModuleJsonWrap_serialize(ZetScript *zs,StackElement *stk);
}
