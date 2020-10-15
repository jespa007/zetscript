#include "zetscript.h"

namespace zetscript{

	void ConsoleBuiltIn::log(ScriptVarString *str, ScriptVarVector *args){
		ScriptVarString *str_out=StringBuiltIn::format(str,args);

		printf("not implemented");

		delete str_out;
	}
}
