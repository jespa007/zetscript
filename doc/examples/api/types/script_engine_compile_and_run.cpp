#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.compileAndRun(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
