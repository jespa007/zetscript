#include "zetscript.h"

using zetscript::ZetScript;

void sayHelloWorld(){
	printf("Say hello world\n");
}


int main(){
	ZetScript zs;
	zs.registerFunction("sayHelloWorld",sayHelloWorld);
	zs.compileAndRun("sayHelloWorld()");
	return 0;
}
