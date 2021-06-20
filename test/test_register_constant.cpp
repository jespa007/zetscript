
#include "zetscript.h"

using namespace zetscript;

int main(){

	ZetScript *zs = new ZetScript(); // instance ZetScript

	zs->registerConstantVariable("MY_CONSTANT_INT",10);
	zs->registerConstantVariable("MY_CONSTANT_STR","my_string");
	zs->registerConstantVariable("MY_CONSTANT_BOOL",true);
	zs->registerConstantVariable("MY_CONSTANT_FLOAT",2.5e-3);

	zs->eval(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);

	delete zs;

#ifdef __MEMMANAGER__
	MEMMGR_print_status();
#endif
}
