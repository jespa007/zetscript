#pragma once

namespace zetscript{

	class JsonBuiltIn{

		ScriptObject * 		 deserialize(ScriptObjectString *str_json);
		ScriptObjectString * serialize(ScriptObject *obj,bool minimized);
	};
}
