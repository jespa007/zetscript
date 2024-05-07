
#include "zetscript.h"

int main(){

	// instance ScriptEngine
	zetscript::ScriptEngine script_engine;

	// Registers a constant Float
	script_engine.registerConstantFloat("MY_CONSTANT_FLOAT",2.5e-3);

	// Prints the value of each registered constant
	script_engine.compileAndRun(
		"Console::outln(\"MY_CONSTANT_FLOAT:\"+MY_CONSTANT_FLOAT);\n"
	);
}