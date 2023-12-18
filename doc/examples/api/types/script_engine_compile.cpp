#include "zetscript.h"
int main(){

	zetscript::ScriptEngine script_engine;

	// Compiles function "add"
	script_engine.compile(
		"function add(_o1,_o2){\n"
		"	return _o1+_o2;\n"
		"}\n"
	);
	
	// Compiles code that uses "adds" function previusly defined
	script_engine.compile(
		"var b=add(2,3)\n"
		"Console::outln(\"b = \"+b)\n"
	);

	// It runs the code compiled (Prints result of variable 'b')
	script_engine.run();

	return 0;
}