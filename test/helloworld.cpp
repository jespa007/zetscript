/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!\n");
}

int main(){

	ZetScript *zs = new ZetScript(); // create zetscript instance

	zs->registerFunction("say_helloworld",say_helloworld);

	zs->eval("say_helloworld();"); // Call c function and prints hello world!

	delete zs; // destroy zetscript instance when not used anymore

	return 0;
}
