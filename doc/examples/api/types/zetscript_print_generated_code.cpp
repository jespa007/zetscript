#include "zetscript.h"
int main(){

	zetscript::ZetScript zs;

	zs.compileFileAndRun(
		"file.zs"
	);

	zs.printGeneratedCode();
}
