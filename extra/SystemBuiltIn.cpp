#include "zetscript.h"

namespace zetscript{

	zs_int SystemBuiltIn::clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}

	void SystemBuiltIn::print(ScriptObjectString *str, ScriptObjectVector *args){
		ScriptObjectString *str_out=StringBuiltIn::format(str,args);
		printf("not implemented");
		delete str_out;
	}
}

