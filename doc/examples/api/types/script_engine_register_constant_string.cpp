
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	// Registers a constant String
	script_engine.registerConstantString("MY_CONSTANT_STR","my_string");

	// Prints the value of each registered constant
	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_STR:\"+MY_CONSTANT_STR);\n"
	);
}