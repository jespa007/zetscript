#include "zetscript.h"
int main(){

	zetscript::ScriptEngine zs;

	zs.compileAndRun(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
