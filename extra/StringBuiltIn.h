#pragma once


namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn{
	public:
		static ScriptObjectString * format(StackElement *str, StackElement *args);
	};

}
