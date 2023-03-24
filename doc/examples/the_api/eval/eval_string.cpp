#include "zetscript.h"
int main(int argc, char *argv[]){

	zetscript::ZetScript zs;

	zs.eval(
		"Console::outln(\"Hello world\")"
	);
}