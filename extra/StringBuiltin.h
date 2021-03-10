#pragma once


namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class StringBuiltin{
	public:

		static ScriptObjectString * formatSf(ZetScript *zs,StackElement *str, StackElement *args);
	};

}
