
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	// Registers a constant Integer, String, Boolean and Float
	script_engine.registerConstant("MY_CONSTANT_INT",10);
	script_engine.registerConstant("MY_CONSTANT_STR","my_string");
	script_engine.registerConstant("MY_CONSTANT_BOOL",true);
	script_engine.registerConstant("MY_CONSTANT_FLOAT",2.5e-3);

	// Prints the value of each registered constant
	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);
}