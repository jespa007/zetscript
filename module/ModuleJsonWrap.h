#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;

	void ModuleJsonWrap_deserialize(ScriptObjectString *str_json);
	ScriptObjectString * ModuleJsonWrap_serialize(ScriptObjectObject *obj,bool *minimized);
	ScriptObjectString * ModuleJsonWrap_serialize(ScriptObjectObject *obj);
}
