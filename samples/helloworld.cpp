/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

// A function wrapper to bind C function into ZetScript
// All ZetScript wrappers needs to have zetscript::ZetScript *_zs as FIRST parameter
void say_helloworld(zetscript::ZetScript *_zs){
	printf("Hello World from C!\n");
}

int main(){

	// ZetScript instance
	zetscript::ZetScript zs;

	// Binds 'say_helloworld' function to be accessible from script
	zs.bindFunction("say_helloworld",say_helloworld);

	 // Prints 'Hello World from script!'
	zs.eval("Console::outln(\"Hello World from script!\")");

	// Call C function and prints 'Hello World from C!'
	zs.eval("say_helloworld();");

	return 0;
}
