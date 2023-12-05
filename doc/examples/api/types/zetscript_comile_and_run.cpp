#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.compileAndRun(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
