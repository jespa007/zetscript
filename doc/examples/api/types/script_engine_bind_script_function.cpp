#include "zetscript.h"

int main(){
	zetscript::ScriptEngine script_engine;

	// Compiles 'printHelloWorld' function
	script_engine.compile("function printHelloWorld(){\n"
		"Console::outln(\"Hello world!!\");\n"
	"}");

	// binds 'printHelloWorld'
	auto print_hello_world=script_engine.bindScriptFunction<void()>("printHelloWorld");

	// invoke 'printHelloWorld'
	print_hello_world();
	
	return 0;
}