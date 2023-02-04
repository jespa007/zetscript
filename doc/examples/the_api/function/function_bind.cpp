#include "zetscript.h"

zetscript::zs_int add(zetscript::ZetScript *_zs, zetscript::zs_int  op1, zetscript::zs_int op2)
{
	return op1+op2;
}

int main(int argc, char *argv[])
{
	zetscript::ZetScript zs;

	// binds native function "add"
	zs.bindFunction("add",add);

 	zs.eval(
 		"Console::outln(\"result:\"+add(5,4));" // prints "result: 9"
 	);

 	return 0;
}

