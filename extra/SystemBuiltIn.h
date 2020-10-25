#pragma once

namespace zetscript{

	class ScriptObjectString;
	class ScriptObjectVector;
	class SystemBuiltIn{
	public:
		// clock
		static zs_int clock();
		static void print(ScriptObjectString *str, ScriptObjectVector *args);
	};

}
