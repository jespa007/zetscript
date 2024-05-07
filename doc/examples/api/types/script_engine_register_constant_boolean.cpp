
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	// Registers a constant Boolean
	script_engine.registerConstantBoolean("MY_CONSTANT_BOOL",true);

	// Prints the value of each registered constant
	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_BOOL:\"+MY_CONSTANT_BOOL);\n"
	);
}