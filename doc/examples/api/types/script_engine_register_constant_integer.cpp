
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	// Registers a constant Integer
	script_engine.registerConstantInteger("MY_CONSTANT_INT",10);

	// Prints the value of each registered constant
	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_INT:\"+MY_CONSTANT_INT);\n"
	);
}