#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.eval(
		"Console::outln(\"Hello world\")"
	);

	return 0;
}
