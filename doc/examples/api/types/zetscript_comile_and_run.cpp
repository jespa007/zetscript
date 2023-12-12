#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	script_engine.eval(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
