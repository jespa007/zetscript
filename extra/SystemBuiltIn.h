#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class SystemBuiltIn{
	public:
		// clock
		static intptr_t clock();
		static void print(ScriptObjectString *str, ScriptObjectVector *args);
	};

}
