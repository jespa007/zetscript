#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;

	namespace json{
		std::string serialize(ZetScript *zs, StackElement *stk, bool is_formatted);
	}

	void JsonModule_deserialize(ScriptObjectString *str_json);
	ScriptObjectString * JsonModule_serialize(ZetScript *zs,StackElement *stk,bool *minimized);
	ScriptObjectString * JsonModule_serialize(ZetScript *zs,StackElement *stk);
}
