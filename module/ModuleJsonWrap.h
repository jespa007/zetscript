#pragma once

namespace zetscript{
	class ScriptObjectObject;
	class ScriptObjectString;
	class ModuleJson{

		ScriptObjectObject * deserialize(ScriptObjectString *obj_str);
		ScriptObjectString * serialize(ScriptObjectObject *obj,bool *minimized);
	};
}
