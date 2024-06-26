
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	script_engine.registerConstantInteger("MY_CONSTANT_INT",10);
	script_engine.registerConstantString("MY_CONSTANT_STR","my_string");
	script_engine.registerConstantBoolean("MY_CONSTANT_BOOL",true);
	script_engine.registerConstantFloat("MY_CONSTANT_FLOAT",2.5e-3);

	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);
}
