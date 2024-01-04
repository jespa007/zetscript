#include "zetscript.h"

void sayHelloWorld(){
	printf("Say hello world\n");
}

int main(){
	zetscript::ScriptEngine script_engine;
	script_engine.registerFunction("sayHelloWorld",sayHelloWorld);
	script_engine.compileAndRun("sayHelloWorld()");
	return 0;
}
