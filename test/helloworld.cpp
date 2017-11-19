/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

using namespace zetscript;

void say_helloworld(){
	printf("Hello World!");
}

int main(){

	CZetScript *zs = CZetScript::getInstance(); // instance zetscript

	register_C_Function("say_helloworld",say_helloworld);

	zs->eval("say_helloworld();"); // Call c function and prints hello world!
	return 0;
}
