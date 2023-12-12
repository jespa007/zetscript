#include "zetscript.h"
int main(){

	zetscript::ScriptEngine se;

	se.compileAndRun(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
