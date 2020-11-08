#pragma once


namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltIn{
	public:

		static ScriptObjectString * formatSf(ZetScript *zs,StackElement *str, StackElement *args);
	};

}
