#pragma once

namespace zetscript{

	class JsonBuiltIn{

		ScriptObjectObject * deserialize(ScriptObjectString *str_json);
		ScriptObjectString * serialize(ScriptObjectObject *obj,bool minimized);
	};
}
