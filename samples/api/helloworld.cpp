#include "zetscript.h"

using zetscript::ScriptEngine;

void sayHelloWorld(){
	printf("Say hello world\n");
}


int main(){
	ScriptEngine zs;
	zs.registerFunction("sayHelloWorld",sayHelloWorld);
	zs.compileAndRun("sayHelloWorld()");
	return 0;
}
