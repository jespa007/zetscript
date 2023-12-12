
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine zs;

	zs.registerConstant("MY_CONSTANT_INT",10);
	zs.registerConstant("MY_CONSTANT_STR","my_string");
	zs.registerConstant("MY_CONSTANT_BOOL",true);
	zs.registerConstant("MY_CONSTANT_FLOAT",2.5e-3);

	zs.compileAndRun(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);
}
