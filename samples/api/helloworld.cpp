#include "zetscript.h"

using zetscript::ScriptEngine;

void sayHelloWorld(){
	printf("Say hello world\n");
}


int main(){
	ScriptEngine se;
	se.registerFunction("sayHelloWorld",sayHelloWorld);
	se.compileAndRun("sayHelloWorld()");
	return 0;
}
