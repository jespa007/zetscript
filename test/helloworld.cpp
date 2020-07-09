/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!\n");
}

int main(){

	ZetScript *zs = new ZetScript();

	zs->Register_C_Function("say_helloworld",say_helloworld);

	zs->evalString("say_helloworld();"); // Call c function and prints hello world!



	return 0;
}
