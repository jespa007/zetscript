#pragma once


namespace zetscript{

	class ScriptVarString;
	class ScriptVarVector;
	class StringBuiltIn{
	public:
		static ScriptVarString * format(ScriptVarString *str, ScriptVarVector *args);
	};

}
