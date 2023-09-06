#include "zetscript.h"

using zetscript::ZetScript;

void sayHelloWorld(){
	printf("Say hello world\n");
}


int main(){
	ZetScript zs;
	zs.registerFunction("sayHelloWorld",sayHelloWorld);
	zs.eval("sayHelloWorld()");
	return 0;
}
