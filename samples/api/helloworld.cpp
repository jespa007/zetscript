#include "zetscript.h"

void sayHelloWorld(zetscript::ScriptEngine *_script_engine){
	printf("Hello world from ZetScript\n");
}

int main(){
	zetscript::ScriptEngine script_engine;
	script_engine.registerFunction("sayHelloWorld",sayHelloWorld);
	script_engine.compileAndRun("sayHelloWorld()");
	return 0;
}
