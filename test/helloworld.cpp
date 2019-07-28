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

	CZetScript zs;

	zs.register_C_Function("say_helloworld",say_helloworld);

	zs.evalString("say_helloworld();"); // Call c function and prints hello world!



	return 0;
}
