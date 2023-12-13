#include "zetscript.h"

using zetscript::ScriptEngine;

void sayHelloWorld(){
	printf("Say hello world\n");
}


int main(){
	ScriptEngine script_engine;
	script_engine.registerFunction("sayHelloWorld",sayHelloWorld);
	script_engine.compileAndRun("sayHelloWorld()");
	return 0;
}
