#pragma once

namespace zetscript{

	class ScriptVarString;
	class ScriptVarVector;
	class ConsoleBuiltIn{
	public:
		static void log(ScriptVarString *str, ScriptVarVector *args);

	};

}
