#pragma once


namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn{
	public:
		ZetScript *zs;
		static void constructorSf(StringBuiltIn *built_in_string,ZetScript *zs);
		static ScriptObjectString * formatSf(StringBuiltIn *built_in_string,StackElement *str, StackElement *args);
	private:
		ScriptObjectString * format(StackElement *str, StackElement *args);
	};

}
