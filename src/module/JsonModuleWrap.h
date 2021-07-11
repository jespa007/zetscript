#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;

	void JsonModuleWrap_deserialize(ScriptObjectString *str_json);
	ScriptObjectString * JsonModuleWrap_serialize(ZetScript *zs,StackElement *stk,bool *minimized);
	ScriptObjectString * JsonModuleWrap_serialize(ZetScript *zs,StackElement *stk);
}
