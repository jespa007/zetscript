
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine se;

	se.registerConstant("MY_CONSTANT_INT",10);
	se.registerConstant("MY_CONSTANT_STR","my_string");
	se.registerConstant("MY_CONSTANT_BOOL",true);
	se.registerConstant("MY_CONSTANT_FLOAT",2.5e-3);

	se.compileAndRun(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);
}
