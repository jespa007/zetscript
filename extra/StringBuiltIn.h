#pragma once


namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn{
	public:
		static ScriptObjectString * format(ScriptObjectString *str, ScriptObjectVector *args);
	};

}
